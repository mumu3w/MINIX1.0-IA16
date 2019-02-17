LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mtune=i8086 -mcmodel=small -I../include \
#		-DDEBUG

all : sh.out sh.sep \
		cat.out getlf.out ls.out mknod.out mount.out pwd.out \
		touch.out update.out cp.out login.out mkdir.out more.out \
		mv.out rm.out umount.out \
		cat.sep getlf.sep ls.sep mknod.sep mount.sep pwd.sep \
		touch.sep update.sep cp.sep login.sep mkdir.sep more.sep \
		mv.sep rm.sep umount.sep 

sh.out : sh1.o sh2.o sh3.o sh4.o sh5.o sh6.o
	$(LD) -T ../test/cmds_tiny.ld -e start -o $@ \
		../lib/crtso.o $^ -L../lib -lsys -M > $@.map
		
sh.sep : sh1.o sh2.o sh3.o sh4.o sh5.o sh6.o
	$(LD) -T ../test/cmds_small.ld -e start -o $@ \
		../lib/crtso.o $^ -L../lib -lsys -M > $@.map
	
%.out : %.o
	$(LD) -T ../test/cmds_tiny.ld -e start -o $@ \
		../lib/crtso.o $< -L../lib -lsys -M > $@.map
		
%.sep : %.o
	$(LD) -T ../test/cmds_small.ld -e start -o $@ \
		../lib/crtso.o $< -L../lib -lsys -M > $@.map
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
clean :
	rm -fr *.tmp *.o *.out *.map *.sep
	