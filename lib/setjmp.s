
.code16
.section .text

.extern exit

JMPERR  = -99

.global setjmp
setjmp:
        mov %sp, %bx
        mov (%bx), %ax
        mov 2(%bx), %bx
        mov %bp, (%bx)
        mov %sp, 2(%bx)
        mov %ax, 4(%bx)
        xor %ax, %ax
        ret

.global longjmp
longjmp:
        push %bp
        mov %sp, %bp
        mov 4(%bp), %bx
        mov 6(%bp), %ax
        or %ax, %ax
        jne .L0
        inc %ax
    .L0:
        mov 2(%bx), %sp
        mov (%bx), %bp
        or %bp, %bp
        jne .L1
        pop %bp
        mov $JMPERR, %ax
        push %ax
        call exit
        hlt
    .L1:
        mov 4(%bx), %bx
        pop %cx
        jmp %bx
        
