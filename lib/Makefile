LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O1 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mcmodel=small
	
OBJS	= divmodsi3.o ldivmod.o memset.o memcpy.o \
		catchsig.o getutils.o sendrec.o setjmp.o \
		access.o chdir.o close.o exec.o getgid.o kill.o mount.o read.o \
		stat.o times.o utime.o alarm.o chmod.o creat.o exit.o getpid.o \
		link.o open.o setgid.o stime.o umask.o wait.o brk2.o chown.o dup2.o \
		fork.o getuid.o lseek.o pause.o setuid.o sync.o umount.o write.o \
		brk.o chroot.o dup.o fstat.o ioctl.o mknod.o pipe.o signal.o time.o \
		unlink.o message.o printk.o call.o syslib.o 
		
OBJS1	= abort.o strlen.o strcmp.o strncpy.o isatty.o geteuid.o stderr.o \
		perror.o fprintf.o fflush.o printdat.o setbuf.o doprintf.o putc.o \
		malloc.o bcopy.o prints.o strcat.o sleep.o crypt.o getpwent.o atoi.o \
		strcpy.o index.o rindex.o
		
CRTS	= head.o crtso.o

all : libsys.a

libsys.a : $(OBJS) $(CRTS) $(OBJS1)
	$(AR) rcs libsys.a $(OBJS) $(OBJS1)
	
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
memset.o : memset.s
	$(CPP) memset.s -o memset.tmp 
	$(AS) -o memset.o memset.tmp

divmodsi3.o : divmodsi3.s
	$(CPP) divmodsi3.s -o divmodsi3.tmp 
	$(AS) -o divmodsi3.o divmodsi3.tmp

ldivmod.o : ldivmod.s
	$(CPP) ldivmod.s -o ldivmod.tmp 
	$(AS) -o ldivmod.o ldivmod.tmp
	
head.o : head.s
	$(CPP) head.s -o head.tmp 
	$(AS) -o head.o head.tmp
	
crtso.o : crtso.s
	$(CPP) crtso.s -o crtso.tmp 
	$(AS) -o crtso.o crtso.tmp
	
getutils.o : getutils.s
	$(CPP) getutils.s -o getutils.tmp 
	$(AS) -o getutils.o getutils.tmp
	
sendrec.o : sendrec.s
	$(CPP) sendrec.s -o sendrec.tmp 
	$(AS) -o sendrec.o sendrec.tmp
	
catchsig.o : catchsig.s
	$(CPP) catchsig.s -o catchsig.tmp 
	$(AS) -o catchsig.o catchsig.tmp
	
clean :
	rm -fr *.tmp *.o *.a
	