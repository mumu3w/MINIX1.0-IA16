
.code16
.section .text

MTYPE   = 2                     # M+mtype =  &M.m_type

.extern vectab, M

.global begsig
begsig:
        push %ax                # after interrupt, save all regs
        push %bx
        push %cx
        push %dx
        push %si
        push %di
        push %bp
        push %ds
        push %es

        mov %sp, %bx
        mov 18(%bx), %bx        # bx = signal number
        mov %bx, %ax            # ax = signal number
        dec %bx                 # vectab[0] is for sig 1
        add %bx, %bx            # pointers are two bytes on 8088
        mov vectab(%bx), %bx    # bx = address of routine to call
        pushw M+MTYPE           # push status of last system call
        push %ax                # func called with signal number as arg
        call *%bx

        pop %ax                 # get signal number off stack
        popw M+MTYPE            # restore status of previous system call
        pop %es                 # signal handling finished
        pop %ds
        pop %bp
        pop %di
        pop %si
        pop %dx
        pop %cx
        pop %bx
        pop %ax
        popw dummy              # remove signal number from stack
        iret

.section .data
dummy:
        .word 0
        