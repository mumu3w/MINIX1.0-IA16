LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mcmodel=small -Di8088 -I./
	
OBJS	= alloc.o break.o exec.o forkexit.o getset.o main.o \
		putc.o signal.o table.o utility.o

all : mm.sep

mm.sep : $(OBJS) mm_small.ld
	$(LD) -T mm_small.ld -e start -o mm.sep ../lib/head.o $(OBJS) -L../lib -lsys -M > mm.map
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
clean :
	rm -fr *.tmp *.o *.sep *.map

