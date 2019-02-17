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

all : hello.out hello.sep
	
%.out : %.o
	$(LD) -T cmds_tiny.ld -e start -o $@ \
		../lib/crtso.o $< -L../lib -lsys -M > $@.map
		
%.sep : %.o
	$(LD) -T cmds_small.ld -e start -o $@ \
		../lib/crtso.o $< -L../lib -lsys -M > $@.map
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
clean :
	rm -fr *.tmp *.o *.out *.map *.sep
	