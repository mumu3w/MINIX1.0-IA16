LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O0 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mtune=i8086 -mcmodel=small -I../include \
#		-DDEBUG

all : hello.out hello.sep setjmp.out setjmp.sep \
	t10a.out t11b.out test10.out test1.out test3.out \
	test5.out test7.out test9.out t11a.out test0.out \
	test11.out test2.out test4.out test6.out test8.out
	
test9.out : test9.c
	$(CC) -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mtune=i8086 -mcmodel=small -I../include -c $<
	$(LD) -T cmds_tiny.ld -e start -o $@ \
		../lib/crtso.o test9.o -L../lib -lsys -M > $@.map
	
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
	