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

all : bootblok.bin build minixfsutils mkfs \
		fsck.out init.out fsck.sep init.sep \
		Image.out Image.sep ram.out ram.sep test.out

Image.out : ../kernel/kernel.out ../mm/mm.out ../fs/fs.out fsck.out init.out bootblok.bin build
	./build bootblok.bin ../kernel/kernel.out ../mm/mm.out \
			../fs/fs.out init.out fsck.out Image.out
			
Image.sep : ../kernel/kernel.sep ../mm/mm.sep ../fs/fs.sep fsck.sep init.sep bootblok.bin build
	./build bootblok.bin ../kernel/kernel.sep ../mm/mm.sep \
			../fs/fs.sep init.sep fsck.sep Image.sep
			
ram.out : mkfs minixfsutils 
	dd if=/dev/zero of=ram.out bs=1024 count=360
	sync
	./mkfs 360 ram.out
	./minixfsutils ram.out put /bin/sh ../cmds/sh.out
	./minixfsutils ram.out put /bin/ls ../cmds/ls.out
	./minixfsutils ram.out put /bin/login ../cmds/login.out
	./minixfsutils ram.out put /bin/echo ../cmds/echo.out
	./minixfsutils ram.out put /bin/cat ../cmds/cat.out
	./minixfsutils ram.out put /bin/update ../cmds/update.out
	./minixfsutils ram.out put /bin/mined ../cmds/mined.out
	./minixfsutils ram.out put /bin/chmem ../cmds/chmem.out
	./minixfsutils ram.out put /bin/mount ../cmds/mount.out
	./minixfsutils ram.out put /bin/mkdir ../cmds/mkdir.out
	./minixfsutils ram.out put /bin/chmod ../cmds/chmod.out
	./minixfsutils ram.out put /bin/mkfs ../cmds/mkfs.out
	./minixfsutils ram.out put /etc/passwd ./passwd
	./minixfsutils ram.out put /etc/rc ./rc
	./minixfsutils ram.out put /etc/message ./message
	./minixfsutils ram.out put /etc/ttys ./ttys
	
test.out : mkfs minixfsutils ../test/*.*
	#dd if=/dev/zero of=test.out bs=1024 count=360
	#sync
	#./mkfs 720 test.out
	rm -fr test.out
	unzip test.zip
	./minixfsutils test.out put /run ../test/run
	./minixfsutils test.out put /hello ../test/hello.out
	./minixfsutils test.out put /setjmp ../test/setjmp.out
	./minixfsutils test.out put /t10a ../test/t10a.out
	./minixfsutils test.out put /t11a ../test/t11a.out
	./minixfsutils test.out put /t11b ../test/t11b.out
	./minixfsutils test.out put /test0 ../test/test0.out
	./minixfsutils test.out put /test10 ../test/test10.out
	./minixfsutils test.out put /test11 ../test/test11.out
	./minixfsutils test.out put /test1 ../test/test1.out
	./minixfsutils test.out put /test2 ../test/test2.out
	./minixfsutils test.out put /test3 ../test/test3.out
	./minixfsutils test.out put /test4 ../test/test4.out
	./minixfsutils test.out put /test5 ../test/test5.out
	./minixfsutils test.out put /test6 ../test/test6.out
	./minixfsutils test.out put /test7 ../test/test7.out
	./minixfsutils test.out put /test8 ../test/test8.out
	./minixfsutils test.out put /test9 ../test/test9.out
	./minixfsutils test.out put /t10a ../test/t10a.out
	./minixfsutils test.out put /t11a ../test/t11a.out
	./minixfsutils test.out put /t11b ../test/t11b.out
	
ram.sep : mkfs minixfsutils
	dd if=/dev/zero of=ram.sep bs=1024 count=360
	sync
	./mkfs 360 ram.sep
	./minixfsutils ram.sep put /bin/sh ../cmds/sh.sep
	./minixfsutils ram.sep put /bin/ls ../cmds/ls.sep
	./minixfsutils ram.sep put /bin/login ../cmds/login.sep
	./minixfsutils ram.sep put /bin/echo ../cmds/echo.sep
	./minixfsutils ram.sep put /bin/cat ../cmds/cat.sep
	./minixfsutils ram.sep put /bin/update ../cmds/update.sep
	./minixfsutils ram.sep put /bin/mined ../cmds/mined.sep
	./minixfsutils ram.sep put /bin/chmem ../cmds/chmem.sep
	./minixfsutils ram.sep put /bin/mount ../cmds/mount.sep
	./minixfsutils ram.sep put /bin/mkdir ../cmds/mkdir.sep
	./minixfsutils ram.sep put /bin/chmod ../cmds/chmod.sep
	./minixfsutils ram.sep put /bin/mkfs ../cmds/mkfs.sep
	./minixfsutils ram.sep put /etc/passwd ./passwd
	./minixfsutils ram.sep put /etc/rc ./rc
	./minixfsutils ram.sep put /etc/message ./message
	./minixfsutils ram.sep put /etc/ttys ./ttys
			
fsck.out : fsck1.o fsck.o kernel_tiny.ld
	$(LD) -T kernel_tiny.ld -e start -o fsck.out \
		fsck1.o fsck.o -L../lib -lsys -M > fsck.out.map
	
init.out : init.o kernel_tiny.ld
	$(LD) -T kernel_tiny.ld -e start -o init.out \
		../lib/head.o init.o -L../lib -lsys -M > init.out.map
		
fsck.sep : fsck1.o fsck.o kernel_small.ld
	$(LD) -T kernel_small.ld -e start -o fsck.sep \
		fsck1.o fsck.o -L../lib -lsys -M > fsck.sep.map
	
init.sep : init.o kernel_small.ld
	$(LD) -T kernel_small.ld -e start -o init.sep \
		../lib/head.o init.o -L../lib -lsys -M > init.sep.map
	
bootblok.bin : bootblok.asm
	nasm -f bin -o $@ $<
	
minixfsutils : minixfsutils.c
	gcc -DFLOPPY_IMAGE -o $@ $<
	
mkfs : mkfs.c
	gcc -DFLOPPY_IMAGE -o $@ $<

build : build.c
	gcc -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
%.o : %.s
	$(CPP) $(CFLAGS) -E $< -o $<.tmp
	$(AS) $<.tmp -o $@
	
clean :
	rm -fr *.tmp *.o *.out *.sep *.map build bootblok.bin \
		Image.out Image.sep minixfsutils mkfs ram.img
	