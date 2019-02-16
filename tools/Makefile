LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mtune=i8086 -mcmodel=small \
#		-DDEBUG

all : fsck.sep init.sep bootblok.bin build

Image : ../kernel/kernel.sep ../mm/mm.sep ../fs/fs.sep fsck.sep init.sep bootblok.bin build
	./build bootblok.bin ../kernel/kernel.sep ../mm/mm.sep \
			../fs/fs.sep init.sep fsck.sep Image
	
bootblok.bin : bootblok.asm
	nasm -f bin -o $@ $<

build : build.c
	gcc -o $@ $<

fsck.sep : fsck1.o fsck.o fsck_small.ld
	$(LD) -T fsck_small.ld -e start -o fsck.sep fsck1.o fsck.o -L../lib -lsys -M > fsck.map

fsck1.o : fsck1.s
	$(CPP) $(CFLAGS) -E fsck1.s -o fsck1.tmp
	$(AS) fsck1.tmp -o fsck1.o
	
fsck.o : fsck.c
	$(CC) $(CFLAGS) -c fsck.c
	
init.sep : init.o init_small.ld
	$(LD) -T init_small.ld -e start -o init.sep ../lib/head.o init.o -L../lib -lsys -M > init.map
	
init.o : init.c
	$(CC) $(CFLAGS) -c init.c
	
clean :
	rm -fr *.tmp *.o *.sep *.map build bootblok.bin Image
	