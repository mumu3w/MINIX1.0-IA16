
.code16
.section .text

# The following definitions are from  ../h/com.h
SEND    = 1
RECEIVE = 2
BOTH    = 3
SYSVEC  = 32

#========================================================================
#                           send and receive                              
#========================================================================
# send(), receive(), send_rec() all save bp, but destroy ax, bx, and cx.

.global send
send:                           # send(dest, ptr)
        mov $SEND, %cx
        jmp .L0

.global receive
receive:                        # receive(src, ptr)
        mov $RECEIVE, %cx
        jmp .L0

.global sendrec
sendrec:
.global send_rec
send_rec:                       
        mov $BOTH, %cx          # send_rec(srcdest, ptr)
        jmp .L0

    .L0:
        push %bp                # save bp
        mov %sp, %bp            # can't index off sp
        mov 4(%bp), %ax         # ax = dest-src
        mov 6(%bp), %bx         # bx = message pointer
        int $SYSVEC             # trap to the kernel
        pop %bp                 # restore bp
        ret                     # return
        
