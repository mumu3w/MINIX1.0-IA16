
.code16
.section .text

# _end   = _endbss              # tells us where the data+bss ends
# _edata = _begbss              # tells us where the initialized data 
                                # (.data) ends.
.extern main, exit
.extern _endbss

# This is the C run-time start-off routine.  It's job is to take the
# arguments as put on the stack by EXEC, and to parse them and set them up the
# way main expects them.
#

.global start
start:
        mov %sp, %bx            # set stackframe pointer (ds=ss)
        mov (%bx), %cx          # get argc
        add $2, %bx             # point at next parameter
        mov %cx, %ax
        inc %ax                 # calculate envp
        shl $1, %ax
        add %bx, %ax
        mov %ax, environ        # save envp
        push %ax                # stack envp
        push %bx                # stack argv
        push %cx                # stack argc

        call main               # call main (arc,argv,envp)

        add $6, %sp             
        push %ax                # stack program-termination status
        call exit               # this will never return

.section .data
.global brksize
brksize:                        # dynamic changeable end of bss
        .word _endbss + 2
.global environ
environ:                        # save environment pointer here
        .word 0
