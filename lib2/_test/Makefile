LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O0 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mtune=i8086 -mcmodel=small -I../_headers \

all : tassert.out tfloat.out tstdio1.out tstdio2.out tctype.out terrno.out \
	tlimits.out tlocale.out tmath1.out tmath2.out tmath3.out tsetjmp.out \
	tsignal.out tstdarg.out tstddef.out tstdlib.out tstring.out ttime.out
	
%.out : %.o
	$(LD) -T ../../test/cmds_tiny.ld -e start -o $@ \
		../../lib/crtso.o $<  -L.. -L../../lib -lc2 -lsys -M > $@.map
		
%.sep : %.o
	$(LD) -T ../../test/cmds_small.ld -e start -o $@ \
		../../lib/crtso.o $<  -L.. -L../../lib -lc2 -lsys -M > $@.map
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
clean :
	rm -fr *.tmp *.o *.out *.map *.sep
	