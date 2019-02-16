
.code16
.section .text

SEND    = 1
RECEIVE = 2
BOTH    = 3
SYSVEC  = 32

.global send
send:
        mov $SEND, %cx
        jmp .L0

.global receive
receive:
        mov $RECEIVE, %cx
        jmp .L0

.global sendrec
sendrec:
.global send_rec
send_rec:
        mov $BOTH, %cx
        jmp .L0

    .L0:
        push %bp
        mov %sp, %bp
        mov 4(%bp), %ax
        mov 6(%bp), %bx
        int $SYSVEC
        pop %bp
        ret
        
