LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O0 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mtune=i8086 -mcmodel=small -I../lib2/_headers

all : hello.out setjmp.out double.out
	
%.out : %.o
	$(LD) -T ../test/cmds_tiny.ld -e start -o $@ \
		../lib/crtso.o $< -L../lib -L../lib2 -lc2 -lsys -M > $@.map
		
%.sep : %.o
	$(LD) -T ../test/cmds_small.ld -e start -o $@ \
		../lib/crtso.o $< -L../lib -L../lib2 -lc2 -lsys -M > $@.map
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
clean :
	rm -fr *.tmp *.o *.out *.map *.sep
	