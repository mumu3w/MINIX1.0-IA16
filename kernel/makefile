LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mcmodel=small -Di8088 -I./
	
OBJS	= mpx88.o klib88.o clock.o dmp.o floppy.o main.o memory.o printer.o proc.o system.o \
		table.o tty.o wini.o

all : kernel.sep

kernel.sep : $(OBJS) kernel_small.ld
	$(OBJCOPY) --redefine-sym _lock=lock klib88.o
	$(LD) -T kernel_small.ld -e start -o kernel.sep $(OBJS) -L../lib -lsys -M > kernel.map
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
%.o : %.asm
	nasm -f elf $<
	
clean :
	rm -fr *.tmp *.o *.sep *.map

