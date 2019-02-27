LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mcmodel=small -Di8088 -I./
	
OBJS	= cache.o filedes.o link.o misc.o open.o pipe.o putc.o stadir.o \
		table.o utility.o device.o inode.o main.o mount.o path.o protect.o \
		read.o super.o time.o write.o

all : fs.out fs.sep

fs.out : $(OBJS) ../tools/kernel_tiny.ld ../lib/libsys.a
	$(LD) -T ../tools/kernel_tiny.ld -e start -o fs.out \
			../lib/head.o $(OBJS) -L../lib -lsys -M > fs.out.map

fs.sep : $(OBJS) ../tools/kernel_small.ld ../lib/libsys.a
	$(LD) -T ../tools/kernel_small.ld -e start -o fs.sep \
			../lib/head.o $(OBJS) -L../lib -lsys -M > fs.sep.map
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
clean :
	rm -fr *.tmp *.o *.out *.sep *.map

