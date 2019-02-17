LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mtune=i8086 -mcmodel=tiny \
#		-DDEBUG

all : fsck.out init.out bootblok.bin build minixfsutils mkfs

Image : ../kernel/kernel.out ../mm/mm.out ../fs/fs.out fsck.out init.out bootblok.bin build
	./build bootblok.bin ../kernel/kernel.out ../mm/mm.out \
			../fs/fs.out init.out fsck.out Image
			
ram.img : mkfs minixfsutils
	dd if=/dev/zero of=ram.img bs=1024 count=360
	./mkfs ram.img
	
bootblok.bin : bootblok.asm
	nasm -f bin -o $@ $<
	
minixfsutils : minixfsutils.c
	gcc -DFLOPPY_IMAGE -o $@ $<
	
mkfs : mkfs.c
	gcc -DFLOPPY_IMAGE -o $@ $<

build : build.c
	gcc -o $@ $<

fsck.out : fsck1.o fsck.o kernel_tiny.ld
	$(LD) -T kernel_tiny.ld -e start -o fsck.out \
		fsck1.o fsck.o -L../lib -lsys -M > fsck.map
	
init.out : init.o kernel_tiny.ld
	$(LD) -T kernel_tiny.ld -e start -o init.out \
		../lib/head.o init.o -L../lib -lsys -M > init.map

%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
%.o : %.s
	$(CPP) $(CFLAGS) -E $< -o $<.tmp
	$(AS) $<.tmp -o $@
	
clean :
	rm -fr *.tmp *.o *.out *.map build bootblok.bin Image minixfsutils mkfs ram.img
	