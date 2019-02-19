
.code16
.section .text

MTYPE   = 2

.extern vectab, M

.global begsig
begsig:
        push %ax
        push %bx
        push %cx
        push %dx
        push %si
        push %di
        push %bp
        push %ds
        push %es

        mov %sp, %bx
        mov 18(%bx), %bx
        mov %bx, %ax
        dec %bx
        add %bx, %bx
        mov vectab(%bx), %bx
        pushw M+MTYPE
        push %ax
        call *%bx

        pop %ax
        popw M+MTYPE
        pop %es
        pop %ds
        pop %bp
        pop %di
        pop %si
        pop %dx
        pop %cx
        pop %bx
        pop %ax
        popw dummy
        iret

.section .data
dummy:
        .word 0
        