LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
NASM = nasm
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
NASMFLAGS = -DBIOS_WINI -f elf
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mcmodel=small -Di8088 -DBIOS_WINI -I./
	
OBJS	= mpx88.o klib88.o clock.o dmp.o floppy.o main.o memory.o printer.o proc.o system.o \
		table.o tty.o bios_wini.o 

all : kernel.out kernel.sep 

kernel.out : $(OBJS) ../tools/kernel_tiny.ld ../lib/libsys.a
	$(LD) -T ../tools/kernel_tiny.ld -e start -o kernel.out \
				$(OBJS) -L../lib -lsys -M > kernel.out.map
				
kernel.sep : $(OBJS) ../tools/kernel_small.ld ../lib/libsys.a
	$(LD) -T ../tools/kernel_small.ld -e start -o kernel.sep \
				$(OBJS) -L../lib -lsys -M > kernel.sep.map
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
%.o : %.asm
	$(NASM) $(NASMFLAGS) $<
	-$(OBJCOPY) --redefine-sym _lock=lock $@
	
clean :
	rm -fr *.tmp *.o *.out *.map *.sep

