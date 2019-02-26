LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mtune=i8086 -mcmodel=small -I../include \
		-I../
#		-DDEBUG

all : sh.out sh.sep mined.out mined.sep cmds.out cmds.sep

cmds.out : cat.out getlf.out ls.out mknod.out mount.out pwd.out \
		touch.out update.out cp.out login.out mkdir.out more.out \
		mv.out rm.out umount.out rmdir.out passwd.out chmem.out \
		kill.out ln.out date.out time.out sync.out mkfs.out echo.out \
		fdisk.out chmod.out df.out clr.out grep.out find.out make.out \
		fix.out gres.out head.out libpack.out libupack.out ar.out \
		basename.out cal.out cc.out cmp.out comm.out cpdir.out dd.out \
		diff.out dosread.out od.out pr.out printenv.out rev.out \
		readfs.out shar.out size.out sleep.out sort.out split.out \
		tail.out tar.out tee.out test.out tr.out uniq.out uudecode.out \
		uuencode.out wc.out
		
cmds.sep : cat.sep getlf.sep ls.sep mknod.sep mount.sep pwd.sep \
		touch.sep update.sep cp.sep login.sep mkdir.sep more.sep \
		mv.sep rm.sep umount.sep rmdir.sep passwd.sep chmem.sep \
		kill.sep ln.sep date.sep time.sep sync.sep mkfs.sep echo.sep \
		fdisk.sep chmod.sep df.sep clr.sep grep.sep find.sep make.sep \
		fix.sep gres.sep head.sep libpack.sep libupack.sep ar.sep \
		basename.sep cal.sep cc.sep cmp.sep comm.sep cpdir.sep dd.sep \
		diff.sep dosread.sep od.sep pr.sep printenv.sep rev.sep \
		readfs.sep shar.sep size.sep sleep.sep sort.sep split.sep \
		tail.sep tar.sep tee.sep test.sep tr.sep uniq.sep uudecode.sep \
		uuencode.sep wc.sep

sh.out : sh1.o sh2.o sh3.o sh4.o sh5.o sh6.o
	$(LD) -T ../test/cmds_tiny.ld -e start -o $@ \
		../lib/crtso.o $^ -L../lib -lsys -M > $@.map
		
sh.sep : sh1.o sh2.o sh3.o sh4.o sh5.o sh6.o
	$(LD) -T ../test/cmds_small.ld -e start -o $@ \
		../lib/crtso.o $^ -L../lib -lsys -M > $@.map
		
mined.out : mined1.o mined2.o
	$(LD) -T ../test/cmds_tiny.ld -e start -o $@ \
		../lib/crtso.o $^ -L../lib -lsys -M > $@.map
		
mined.sep : mined1.o mined2.o
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
	