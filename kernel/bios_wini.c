/* This file contains a driver for "Generic" winchester disks; it uses
 * the ROM BIOS winchester disk routines.
 *
 * It was written by Eric Roskos, based on Adri Koppes's XT_WINI driver.
 *
 * If you use this driver, you have to define GENERIC_FDISK and recompile
 * main.c and klib88.asm, since those files have ifdef's in them for this
 * driver: main.c is changed to not overwrite the BIOS reserved vectors
 * and int 13H, and klib88.asm has C call interfaces to the BIOS.
 *
 * The driver supports two operations: read a block and
 * write a block.  It accepts two messages, one for reading and one for
 * writing, both using message format m2 and with the same parameters:
 *
 *    m_type      DEVICE    PROC_NR     COUNT    POSITION  ADRRESS
 * ----------------------------------------------------------------
 * |  DISK_READ | device  | proc nr |  bytes  |  offset | buf ptr |
 * |------------+---------+---------+---------+---------+---------|
 * | DISK_WRITE | device  | proc nr |  bytes  |  offset | buf ptr |
 * ----------------------------------------------------------------
 *
 * The file contains one entry point:
 *
 *   winchester_task:	main entry when system is brought up
 *
 */

#include "../h/const.h"
#include "../h/type.h"
#include "../h/callnr.h"
#include "../h/com.h"
#include "../h/error.h"
#include "const.h"
#include "type.h"
#include "proc.h"

/*
 * define WN_SAFETY to cause the driver to panic if you try to access any
 * sectors below the start of partition 2 -- this is supposed to prevent an
 * errant program from causing the DOS partition to be overwritten.  It
 * assumes your DOS partition is partition 1 (the first one).
 */
//#define WN_SAFETY	/* */

/* Parameters for the disk drive. */
#define SECTOR_SIZE      512	/* physical sector size in bytes */
#define NR_SECTORS      0x11	/* number of sectors per track */

/* Error codes */
#define ERR		  -1	/* general error */

/* Miscellaneous. */
#define MAX_ERRORS         4	/* how often to try rd/wt before quitting */
#define MAX_RESULTS        4	/* max number of bytes controller returns */
#define NR_DEVICES        10	/* maximum number of drives */
#define MAX_WIN_RETRY  10000	/* max # times to try to output to WIN */
#define PART_TABLE     0x1C6	/* IBM partition table starts here in sect 0 */
#define DEV_PER_DRIVE      5	/* hd0 + hd1 + hd2 + hd3 + hd4 = 5 */
#define SAFETY_CONST 0xAAAAAAAAL/* table integrity check */

/* Variables. */
PRIVATE struct wini {		/* main drive struct, one entry per drive */
  int wn_opcode;		/* DISK_READ or DISK_WRITE */
  int wn_procnr;		/* which proc wanted this operation? */
  int wn_drive;			/* drive number addressed */
  int wn_cylinder;		/* cylinder number addressed */
  int wn_sector;		/* sector addressed */
  int wn_head;			/* head number addressed */
  int wn_heads;			/* maximum number of heads */
  long wn_low;			/* lowest cylinder of partition */
  long wn_size;			/* size of partition in blocks */
  int wn_trksize;		/* size of track on this drive */
  int wn_count;			/* byte count */
  vir_bytes wn_address;		/* user virtual address */
  char wn_results[MAX_RESULTS];	/* the controller can give lots of output */
} wini[NR_DEVICES];

PRIVATE int w_need_reset = FALSE;	 /* set to 1 when controller must be reset */
PRIVATE int nr_drives;          /* Number of drives */

PRIVATE message w_mess;		/* message buffer for in and out */

/* this buffer is sort of a waste of space, I think? */
PRIVATE unsigned char buf[BLOCK_SIZE];  /* Buffer used by the startup routine */

PRIVATE struct param {
	int nr_cyl;		/* Number of cylinders */
	int nr_heads;		/* Number of heads */
	int nr_drives;		/* Number of drives on this drive's ctroller */
	int nr_sectors;		/* number of sectors per track */
} param0, param1;


PRIVATE int w_do_rdwt(message *m_ptr);
PRIVATE w_reset();
PRIVATE init_params();
PRIVATE copy_prt(int drive);
PRIVATE sort(register struct wini *wn);
PRIVATE swap(register struct wini *first, register struct wini *second);


/*===========================================================================*
 *				winchester_task				     * 
 *===========================================================================*/
PUBLIC winchester_task()
{
/* Main program of the winchester disk driver task. */

  int r, caller, proc_nr;

  /* First initialize the controller */
  init_params();

  /* Here is the main loop of the disk task.  It waits for a message, carries
   * it out, and sends a reply.
   */

  while (TRUE) {
	/* First wait for a request to read or write a disk block. */
	receive(ANY, &w_mess);	/* get a request to do some work */
	if (w_mess.m_source < 0) {
		printf("winchester task got message from %d ", w_mess.m_source);
		continue;
	}
	caller = w_mess.m_source;
	proc_nr = w_mess.PROC_NR;

	/* Now carry out the work. */
	switch(w_mess.m_type) {
	    case DISK_READ:
	    case DISK_WRITE:	r = w_do_rdwt(&w_mess);	break;
	    default:		r = EINVAL;		break;
	}

	/* Finally, prepare and send the reply message. */
	w_mess.m_type = TASK_REPLY;	
	w_mess.REP_PROC_NR = proc_nr;

	w_mess.REP_STATUS = r;	/* # of bytes transferred or error code */
	send(caller, &w_mess);	/* send reply to caller */
  }
}


/*===========================================================================*
 *				w_do_rdwt				     * 
 *===========================================================================*/
PRIVATE int w_do_rdwt(m_ptr)
message *m_ptr;			/* pointer to read or write w_message */
{
/* Carry out a read or write request from the disk. */
  register struct wini *wn;
  int r, device, errors = 0;
  long sector;
  phys_bytes user_phys;
  int sb, ib;
  int fRW;
  int csectors;
  int nr_sectors;
  extern phys_bytes umap();

  /* Decode the w_message parameters. */
  device = m_ptr->DEVICE;
  if (device < 0 || device >= NR_DEVICES)
	return(EIO);
  if (m_ptr->COUNT != BLOCK_SIZE)
	return(EINVAL);
  wn = &wini[device];		/* 'wn' points to entry for this drive */
  wn->wn_drive = device/DEV_PER_DRIVE;	/* save drive number */
  if (wn->wn_drive >= nr_drives)
	return(EIO);
  wn->wn_opcode = m_ptr->m_type;	/* DISK_READ or DISK_WRITE */
  if (m_ptr->POSITION % BLOCK_SIZE != 0)
	return(EINVAL);
  sector = m_ptr->POSITION/SECTOR_SIZE;
  if ((sector+BLOCK_SIZE/SECTOR_SIZE) > wn->wn_size)
	return(EOF);
  sector += wn->wn_low;
  nr_sectors = wn->wn_trksize;

  wn->wn_cylinder = sector / (wn->wn_heads * nr_sectors);
  wn->wn_sector =  (sector % nr_sectors);
  wn->wn_head = (sector % (wn->wn_heads * nr_sectors) )/nr_sectors;
  wn->wn_count = m_ptr->COUNT;
  wn->wn_address = (vir_bytes) m_ptr->ADDRESS;
  wn->wn_procnr = m_ptr->PROC_NR;

  /* here we compute the values we pass the BIOS */
  user_phys = umap(proc_addr(wn->wn_procnr), D, wn->wn_address, 
		   (vir_bytes)wn->wn_count);
  sb = (user_phys >> 4) & 0xffff;
  ib = user_phys & 0xf;
  fRW = (wn->wn_opcode==DISK_READ)? 0 : 1;
  csectors = wn->wn_count / SECTOR_SIZE;

  /* This loop allows a failed operation to be repeated. */
  while (errors <= MAX_ERRORS) {
	errors++;		/* increment count once per loop cycle */
	if (errors >= MAX_ERRORS)
		return(EIO);

	/* First check to see if a reset is needed. */
	if (w_need_reset) w_reset();

	/* Perform the transfer. */
	r = diskio(fRW, wn->wn_cylinder, wn->wn_sector, wn->wn_head,
		   csectors, wn->wn_drive, sb, ib, wn->wn_trksize);
	if (r == OK) break;	/* if successful, exit loop */

  }

  if (r!=OK)
  {
	printf("\ngn_wini: hardware error, BIOS code %02x\n", r);
	w_need_reset = TRUE;
  }

  return(r == OK ? BLOCK_SIZE : EIO);
}


/*===========================================================================*
 *				w_reset					     * 
 *===========================================================================*/
PRIVATE w_reset()
{
/* Issue a reset to the controller.  This is done after any catastrophe,
 * like the controller refusing to respond.
 */

  return(win_init());
}


/*============================================================================*
 *				init_params				      *
 *============================================================================*/
PRIVATE init_params()
{
/* This routine is called at startup to initialize the partition table,
 * the number of drives and the controller
*/
  unsigned int i, segment, offset;
  phys_bytes address;
  extern phys_bytes umap();
  extern int vec_table[];

  /*
   * Be sure BIOS is all set up before we ask for parameters.  This
   * may not really be necessary; we do it again below.
   */
  win_init();

  /* Copy the parameters to the structures */
  hdisk_params(0, &param0);
  hdisk_params(1, &param1);

  /* Get the number of drives */
  nr_drives = param0.nr_drives;

  /* Set the parameters in the drive structure */
  for (i=0; i<5; i++)
  {
	wini[i].wn_heads = param0.nr_heads;
	wini[i].wn_trksize = param0.nr_sectors;
  }
  wini[0].wn_low = wini[5].wn_low = 0L;
  wini[0].wn_size = (long)((long)param0.nr_cyl * (long)param0.nr_heads * (long)NR_SECTORS);
  for (i=5; i<10; i++)
  {
	wini[i].wn_heads = param1.nr_heads;
	wini[i].wn_trksize = param1.nr_sectors;
  }
  wini[5].wn_size = (long)((long)param1.nr_cyl * (long)param1.nr_heads * (long)NR_SECTORS);


  /* Initialize the controller */
  if ((nr_drives > 0) && (win_init() != OK))
		nr_drives = 0;

  /* Read the partition table for each drive and save them */
  for (i = 0; i < nr_drives; i++) {
	w_mess.DEVICE = i * 5;
	w_mess.POSITION = 0L;
	w_mess.COUNT = BLOCK_SIZE;
	w_mess.ADDRESS = (char *) buf;
	w_mess.PROC_NR = WINCHESTER;
	w_mess.m_type = DISK_READ;
	if (w_do_rdwt(&w_mess) != BLOCK_SIZE)
		panic("Can't read partition table of winchester ", i);
	copy_prt(i * 5);
  }
}


/*============================================================================*
 *				copy_prt				      *
 *============================================================================*/
PRIVATE copy_prt(drive)
int drive;
{
/* This routine copies the partition table for the selected drive to
 * the variables wn_low and wn_size
 */

  register int i, offset;
  struct wini *wn;
  long adjust;

  for (i=0; i<4; i++) {
	adjust = 0;
	wn = &wini[i + drive + 1];
	offset = PART_TABLE + i * 0x10;
	wn->wn_low = *(long *)&buf[offset];
	if ((wn->wn_low % (BLOCK_SIZE/SECTOR_SIZE)) != 0) {
		adjust = wn->wn_low;
		wn->wn_low = (wn->wn_low/(BLOCK_SIZE/SECTOR_SIZE)+1)*(BLOCK_SIZE/SECTOR_SIZE);
		adjust = wn->wn_low - adjust;
	}
	wn->wn_size = *(long *)&buf[offset + sizeof(long)] - adjust;
  }
  sort(&wini[drive + 1]);
}


PRIVATE sort(wn)
register struct wini *wn;
{
  register int i,j;

  for (i=0; i<4; i++)
	for (j=0; j<3; j++)
		if ((wn[j].wn_low == 0) && (wn[j+1].wn_low != 0))
			swap(&wn[j], &wn[j+1]);
		else if (wn[j].wn_low > wn[j+1].wn_low && wn[j+1].wn_low != 0)
			swap(&wn[j], &wn[j+1]);
}


PRIVATE swap(first, second)
register struct wini *first, *second;
{
  register struct wini tmp;

  tmp = *first;
  *first = *second;
  *second = tmp;
}
