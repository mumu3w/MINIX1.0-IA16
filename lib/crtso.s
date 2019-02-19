
.code16
.section .text

# _end   = _endbss
# _edata = _begbss
.extern main, exit
.extern _endbss

.global start
start:
        mov %sp, %bx
        mov (%bx), %cx
        add $2, %bx
        mov %cx, %ax
        inc %ax
        shl $1, %ax
        add %bx, %ax
        mov %ax, environ
        push %ax
        push %bx
        push %cx

        call main

        add $6, %sp
        push %ax
        call exit

.section .data
.global brksize
brksize:
        .word _endbss + 2
.global environ
environ:
        .word 0
