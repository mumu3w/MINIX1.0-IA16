/* chmem - set total memory size for execution	Author: Andy Tanenbaum */
/* Modified for DOS and to accept multiple filenames -- Deborah Mullen */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/unistd.h>

typedef unsigned int u32_t;

#define HLONG            8	/* header size in longs */
#define TEXT             2	/* where is text size in header */
#define DATA             3	/* where is data size in header */
#define BSS              4	/* where is bss size in header */
#define TOT              6	/* where in header is total allocation */
#define TOTPOS          24	/* where is total in header */
#define SEPBIT  0x00200000	/* this bit is set for separate I/D */
#define MAGIC       0x0301	/* magic number for executable progs */
#define MAX         65520L	/* maximum allocation size */

void usage();
void stderr3(const char *s1, const char *s2, const char *s3);
void std_err(const char *s);

int main(int argc, char *argv[])
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
  unsigned int n;
  int fd, separate;
  u32_t lsize, olddynam, newdynam, newtot, overflow, header[HLONG];
  int i;

  p = argv[1];
  if (argc < 3) usage();
  if (*p != '=' && *p != '+' && *p != '-') usage();
  n = atoi(p+1);
  lsize = n;
  if (n > 65520) {
	stderr3("chmem: ", p+1, " too large\n");
	exit(2);
   }

   for (i=2; i < argc; i++ ) {

	  fd = open(argv[i], O_RDWR);
	  if (fd < 0) {
		stderr3("chmem: can't open ", argv[i], "\n");
		continue;
	  }

	  if (read(fd, header, sizeof(header)) != sizeof(header)) {
		stderr3("chmem: ", argv[i], "bad header\n");
		close(fd);
		continue;
	  }

	  if ( (header[0] & 0xFFFFL) != MAGIC) {
	
		stderr3("chmem: ", argv[i], " not executable\n");
		close(fd);
		continue;
	  }

	  separate = (header[0] & SEPBIT ? 1 : 0);
	  olddynam = header[TOT] - header[DATA] - header[BSS];
	  if (separate == 0) olddynam -= header[TEXT];

	  if (*p == '=') newdynam = lsize;
	  else if (*p == '+') newdynam = olddynam + lsize;
	  else if (*p == '-') newdynam = olddynam - lsize;

	  newtot = header[DATA] + header[BSS] + newdynam;
	  if (separate == 0) newtot += header[TEXT];
	  overflow = (newtot > MAX ? newtot - MAX : 0);	/* 64K max */
	  newdynam -= overflow;
	  newtot -= overflow;
	  lseek(fd, (long) TOTPOS, 0);
	
	  if (write(fd, &newtot, 4) < 0) {
		stderr3("chmem: can't modify ", argv[i], "\n");
		close(fd);
		continue;
	  }
	  printf("%s: Stack+malloc area changed from %u to %u bytes.\n",
			 argv[i],  olddynam, newdynam);
	  close(fd);
   }  
   exit(0);
}

void usage()
{
  std_err("Usage: chmem {=+-} amount file\n");
  exit(2);
}

void stderr3(const char *s1, const char *s2, const char *s3)
{
  std_err(s1);
  std_err(s2);
  std_err(s3);
}

void std_err(const char *s)
{
   printf("%s",s);
}
