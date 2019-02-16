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
		getutils.o sendrec.o catchsig.o \
		call.o exec.o exit.o fork.o open.o wait.o write.o close.o signal.o \
		sync.o read.o message.o brk.o syslib.o printk.o creat.o fstat.o 
CRTS	= head.o

all : $(OBJS) $(CRTS)
	$(AR) rcs libsys.a $(OBJS)
	
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
	