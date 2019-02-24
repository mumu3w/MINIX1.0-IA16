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

all : bootblok.bin build minixfsutils mkfs \
		fsck.out init.out fsck.sep init.sep \
		Image.out Image.sep

Image.out : ../kernel/kernel.out ../mm/mm.out ../fs/fs.out fsck.out init.out bootblok.bin build
	./build bootblok.bin ../kernel/kernel.out ../mm/mm.out \
			../fs/fs.out init.out fsck.out Image.out
			
Image.sep : ../kernel/kernel.sep ../mm/mm.sep ../fs/fs.sep fsck.sep init.sep bootblok.bin build
	./build bootblok.bin ../kernel/kernel.sep ../mm/mm.sep \
			../fs/fs.sep init.sep fsck.sep Image.sep

fsck.out : fsck1.o fsck.o kernel_tiny.ld
	$(LD) -T kernel_tiny.ld -e start -o fsck.out \
		fsck1.o fsck.o -L../lib -lsys -M > fsck.out.map
	
init.out : init.o kernel_tiny.ld
	$(LD) -T kernel_tiny.ld -e start -o init.out \
		../lib/head.o init.o -L../lib -lsys -M > init.out.map
		
fsck.sep : fsck1.o fsck.o kernel_small.ld
	$(LD) -T kernel_small.ld -e start -o fsck.sep \
		fsck1.o fsck.o -L../lib -lsys -M > fsck.sep.map
	
init.sep : init.o kernel_small.ld
	$(LD) -T kernel_small.ld -e start -o init.sep \
		../lib/head.o init.o -L../lib -lsys -M > init.sep.map
	
bootblok.bin : bootblok.asm
	nasm -f bin -o $@ $<
	
minixfsutils : minixfsutils.c
	gcc -o $@ $<
	
mkfs : mkfs.c
	gcc -DFLOPPY_IMAGE -o $@ $<

build : build.c
	gcc -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
%.o : %.s
	$(CPP) $(CFLAGS) -E $< -o $<.tmp
	$(AS) $<.tmp -o $@
	
clean :
	rm -fr *.tmp *.o *.out *.sep *.map build bootblok.bin \
		Image.out Image.sep minixfsutils mkfs ram.img
	