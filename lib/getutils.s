
.code16
.section .text

.extern _endbss

.global get_base
get_base:
        mov %ds, %ax
        ret

.global get_size
get_size:
        mov $_endbss, %ax
        ret

.global get_tot_mem
get_tot_mem:
        cli
        push %es
        push %di
        mov $8192, %ax
        sub %di, %di
    .L0:
        mov %ax, %es
        # mov $0xa5a4, %bx
        # mov %bx, %es:(%di)
        movw $0xa5a4, %es:(%di)
        xor %bx, %bx
        mov %es:(%di), %bx
        cmp $0xa5a4, %bx
        jne .L1
        add $4096, %ax
        cmp $0xa000, %ax
        jne .L0
    .L1:
        pop %di
        pop %es
        sti
        ret
