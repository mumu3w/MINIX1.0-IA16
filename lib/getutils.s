
.code16
.section .text

.extern _endbss

#========================================================================
#                           utilities                                     
#========================================================================

.global get_base
get_base:                       # return click at which prog starts
        # mov %ds, %ax
	mov %cs, %ax
        ret

.global get_size
get_size:                       # return prog size in bytes [text+data+bss]
        mov $_endbss, %ax       # end is label at end of bss
        ret

# Find out how much memory the machine has, including vectors, kernel MM, etc.
.global get_tot_mem
get_tot_mem:
        cli
        push %es
        push %di
        mov $8192, %ax          # start search at 128K [8192 clicks]
        sub %di, %di
    .L0:
        mov %ax, %es
        # mov $0xa5a4, %bx
        # mov %bx, %es:(%di)
        movw $0xa5a4, %es:(%di) # write random bit pattern to memory
        xor %bx, %bx
        mov %es:(%di), %bx      # read back pattern just written
        cmp $0xa5a4, %bx        # compare with expected value
        jne .L1                 # if different, no memory present
        add $4096, %ax          # advance counter by 64K
        cmp $0xa000, %ax        # stop seaching at 640K
        jne .L0
    .L1:
        pop %di
        pop %es
        sti
        ret
