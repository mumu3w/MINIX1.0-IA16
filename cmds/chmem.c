/* chmem - set total memory size for execution	   Author: Andy Tanenbaum */

/* modified for use with Turbo C under DOS by Eelco van Asperen, 87/08/26
*
* Changes made involve the use of unsigned long's rather than long's and
* unsigned int's; and the mode-parameter for open() has to include O_BINARY
* for Turbo C.
*/

#ifdef DOS
# include <fcntl.h>
# include <stdlib.h>

# define  RWMODE   (O_RDWR|O_BINARY)	
#else
# define  RWMODE   (2)
#endif


#define HLONG            8	/* header size in longs */
#define TEXT             2	/* where is text size in header */
#define DATA             3	/* where is data size in header */
#define BSS              4	/* where is bss size in header */
#define TOT              6	/* where in header is total allocation */
#define TOTPOS          24	/* where is total in header */

#ifdef DOS
# define SEPBIT 0x00200000L	  /* this bit is set for separate I/D */
# define MAXPARAM   65520lu   /* maximum for parameter */
#else
# define SEPBIT  0x00200000	  /* this bit is set for separate I/D */
# define MAXPARAM     65520   /* maximum for parameter */
#endif

#define MAGIC       0x0301	/* magic number for executable progs */
#define MAX         65536L	/* maximum allocation size */

main(argc, argv)
int argc;
char *argv[];
{
/* The 8088 architecture does not make it possible to catch stacks that grow
 * big.  The only way to deal with this problem is to let the stack grow down
 * towards the data segment and the data segment grow up towards the stack.
 * Normally, a total of 64K is allocated for the two of them, but if the
 * programmer knows that a smaller amount is sufficient, he can change it
 * using chmem.
 *
 * chmem =4096 prog  sets the total space for stack + data growth to 4096
 * chmem +200  prog  increments the total space for stack + data growth by 200
 */

  char *p;
  int fd, separate;
#ifdef DOS
  unsigned long lsize, olddynam, newdynam, newtot, overflow, header[HLONG];
#else
  unsigned int n;
  long lsize, olddynam, newdynam, newtot, overflow, header[HLONG];
#endif

  p = argv[1];
  if (argc != 3) usage();
  if (*p != '=' && *p != '+' && *p != '-') usage();
#ifdef DOS
  lsize = (unsigned long) atol(p+1);
#else
  n = atoi(p+1);
  lsize = n;
#endif
  if (lsize > MAXPARAM) stderr3("chmem: ", p+1, " too large\n");

  fd = open(argv[2], RWMODE);
  if (fd < 0) stderr3("chmem: can't open ", argv[2], "\n");

  if (read(fd, header, sizeof(header)) != sizeof(header))
	stderr3("chmem: ", argv[2], "bad header\n");
  if ( (header[0] & 0xFFFF) != MAGIC)
	stderr3("chmem: ", argv[2], " not executable\n");
  separate = (header[0] & SEPBIT ? 1 : 0);
  olddynam = header[TOT] - header[DATA] - header[BSS];
  if (separate == 0) olddynam -= header[TEXT];

  if (*p == '=') newdynam = lsize;
  else if (*p == '+') newdynam = olddynam + lsize;
  else if (*p == '-') newdynam = olddynam - lsize;
  newtot = header[DATA] + header[BSS] + newdynam;
  overflow = (newtot > MAX ? newtot - MAX : 0);	/* 64K max */
  newdynam -= overflow;
  newtot -= overflow;
  if (separate == 0) newtot += header[TEXT];
  lseek(fd, (long) TOTPOS, 0);
  if (write(fd, &newtot, 4) < 0)
	stderr3("chmem: can't modify ", argv[2], "\n");

#ifdef DOS
  printf("%s: Stack+malloc area changed from %lu to %lu bytes.\n",
			 argv[2],  olddynam, newdynam);
#else
  printf("%s: Stack+malloc area changed from %D to %D bytes.\n",
			 argv[2],  olddynam, newdynam);
#endif

  exit(0);
}

usage()
{
  std_err("usage:\tchmem {=+-} amount file\n");
  exit(1);
}

stderr3(s1, s2, s3)
char *s1, *s2, *s3;
{
  std_err(s1);
  std_err(s2);
  std_err(s3);
  exit(1);
}

#ifdef DOS

std_err(s)
char *s;
{
  char *p = s;

  while(*p != 0) p++;
  write(2, s, p - s);
}

#endif
