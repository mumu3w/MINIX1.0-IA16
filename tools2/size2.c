/* size - tell size of an object file		Author: Andy Tanenbaum */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int u32_t;

#define HLONG            8	/* # longs in the header */
#define TEXT             2
#define DATA             3
#define BSS              4
#define CHMEM            6
#define MAGIC       0x0301	/* magic number for an object file */
#define SEPBIT  0x00200000	/* this bit is set for separate I/D */

int heading;			/* set when heading printed */
int error;

void size(char *name);
void stderr3(const char *s1, const char *s2, const char *s3);

int main(int argc, char *argv[])
{
  int i;

  if (argc == 1) {
	size("a.out");
	exit(error);
  }

  for (i = 1; i < argc; i++) size(argv[i]);
  exit(error);
}

void size(char *name)
{
  FILE *fd;
  int separate;
  u32_t head[HLONG], dynam, allmem;

  if ( (fd = fopen(name, "rb")) == NULL) {
	stderr3("size: can't open ", name, "\n");
	return;
  }

  if ( fread(head, 1, sizeof(head), fd) != sizeof(head) ) {
	stderr3("size: ", name, ": header too short\n");
  	error = 1;
	fclose(fd);
	return;
  }

  if ( (head[0] & 0xFFFF) != MAGIC) {
	stderr3("size: ", name, " not an object file\n");
	fclose(fd);
	return;
  }

  separate = (head[0] & SEPBIT ? 1 : 0);
  dynam = head[CHMEM] - head[TEXT] - head[DATA] - head[BSS];
  if (separate) dynam += head[TEXT];
  allmem = (separate ? head[CHMEM] + head[TEXT] : head[CHMEM]);
  if (heading++ == 0) 
	printf("  text\t  data\t   bss\t stack\tmemory\n");
  printf("%6d\t%6d\t%6d\t%6d\t%6d\t%s\n",head[TEXT], head[DATA], head[BSS],
		dynam, allmem, name);
  fclose(fd);
}

void stderr3(const char *s1, const char *s2, const char *s3)
{
  fprintf(stderr, "%s %s %s", s1, s2 ,s3);
  error = 1;
}
