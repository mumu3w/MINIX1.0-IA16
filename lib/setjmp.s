
#       struct jmpbuf {
#               int bp;
#               int sp;
#               int ret-addr
#       }

.code16
.section .text

.extern exit

JMPERR  = -99                   # call exit(JMPERR) when jmp-error

.global setjmp
setjmp:
        mov %sp, %bx
        mov (%bx), %ax          # ret-addr.
        mov 2(%bx), %bx         # addr of jmp-struct
        mov %bp, (%bx)          
        mov %sp, 2(%bx)
        mov %ax, 4(%bx)
        xor %ax, %ax
        ret

.global longjmp
longjmp:
        push %bp                
        mov %sp, %bp            # set new frame pointer to sp
        mov 4(%bp), %bx         # get address of jmp-structure
        mov 6(%bp), %ax         # get ret-code
        or %ax, %ax
        jne .L0
        inc %ax                 # return code may not be zero
    .L0:
        mov 2(%bx), %sp
        mov (%bx), %bp
        or %bp, %bp             # test if last frame-pointer (error)
        jne .L1                 # else execute the longjmp
        pop %bp
        mov $JMPERR, %ax
        push %ax
        call exit               # should never return
        hlt
    .L1:
        mov 4(%bx), %bx
        pop %cx
        jmp *%bx
        
