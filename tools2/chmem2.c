/* chmem - set total memory size for execution	   Author: Andy Tanenbaum */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int u32_t;

#define HLONG             8	/* header size in longs */
#define TEXT              2	/* where is text size in header */
#define DATA              3	/* where is data size in header */
#define BSS               4	/* where is bss size in header */
#define TOT               6	/* where in header is total allocation */
#define TOTPOS           24	/* where is total in header */

#define SEPBIT     0x200000     /* this bit is set for separate I/D */
#define MAXPARAM      65520     /* maximum for parameter */

#define MAGIC        0x0301	/* magic number for executable progs */
#define MAX          65520L	/* maximum allocation size */

void usage();
void stderr3(const char *s1, const char *s2, const char *s3);

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
  FILE *fd;
  int separate;
  u32_t lsize, olddynam, newdynam, newtot, overflow, header[HLONG];

  p = argv[1];
  if (argc != 3) usage();
  if (*p != '=' && *p != '+' && *p != '-') usage();
  lsize = (u32_t) atoi(p+1);

  if (lsize > MAXPARAM) stderr3("chmem: ", p+1, " too large\n");

  fd = fopen(argv[2], "rb+");
  if (fd == NULL) stderr3("chmem: can't open ", argv[2], "\n");
  fseek(fd, 0, SEEK_SET);

  if ( fread(header, 1, sizeof(header), fd) != sizeof(header) )
        stderr3("chmem: ", argv[2], " bad header\n");
  if ( (header[0] & 0xFFFF) != MAGIC)
        stderr3("chmem: ", argv[2], " not executable\n");
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
  fseek(fd, TOTPOS, SEEK_SET);
  if (fwrite(&newtot, 1, sizeof(u32_t), fd) != sizeof(u32_t))
	stderr3("chmem: can't modify ", argv[2], "\n");

  printf("%s: Stack+malloc area changed from %u to %u bytes.\n",
			 argv[2],  olddynam, newdynam);

  return 0;
}

void usage()
{
  fprintf(stderr, "usage:\tchmem {=+-} amount file\n");
  exit(1);
}

void stderr3(const char *s1, const char *s2, const char *s3)
{
  fprintf(stderr, "%s %s %s", s1, s2 ,s3);
  exit(1);
}
