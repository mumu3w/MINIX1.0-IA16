LD = ia16-elf-ld
CC = ia16-elf-gcc
CPP = ia16-elf-cpp
AS = ia16-elf-as
AR = ia16-elf-ar
OBJCOPY = ia16-elf-objcopy
OBJDUMP = ia16-elf-objdump
CFLAGS = -O0 -nostdinc -nostdlib -fno-builtin -ffreestanding \
		-march=i8086 -mcmodel=small -I./_headers 

# Floating point number
LIBGCCS	=  _addsub_df.o _mul_df.o _div_df.o _df_to_si.o _thenan_df.o _muldi3.o \
		_lshrdi3.o _ashldi3.o _pack_df.o _unpack_df.o _eq_df.o _ne_df.o \
		_gt_df.o _lt_df.o _fpcmp_parts_df.o _si_to_df.o _sf_to_df.o \
		_le_df.o _ge_df.o _clzsi2.o _unpack_sf.o _make_df.o _clz.o _mul_sf.o \
		_df_to_sf.o _pack_sf.o _thenan_sf.o _make_sf.o _ne_sf.o \
		_fpcmp_parts_sf.o _usi_to_df.o _fixunsdfsi.o

LIBGCCA	= /usr/lib/x86_64-linux-gnu/gcc/ia16-elf/6.3.0/libgcc.a

all : 
	$(AR) xv $(LIBGCCA) $(LIBGCCS)
	$(CC) $(CFLAGS) -c assert/*.c
	$(CC) $(CFLAGS) -c stdio/*.c
	$(CC) $(CFLAGS) -c string/*.c
	$(CC) $(CFLAGS) -c time/*.c
	$(CC) $(CFLAGS) -c ctype/*.c
	$(CC) $(CFLAGS) -c float/*.c
	$(CC) $(CFLAGS) -c locale/*.c
	$(CC) $(CFLAGS) -c math/*.c
	$(CC) $(CFLAGS) -c stdlib/*.c
	$(CC) $(CFLAGS) -c errno/*.c
	cp ../lib/exit.o _exit.o
	cp ../lib/time.o _time.o
	-$(OBJCOPY) --redefine-sym exit=_exit _exit.o
	-$(OBJCOPY) --redefine-sym time=_time _time.o
	$(AR) rcs libc2.a *.o
