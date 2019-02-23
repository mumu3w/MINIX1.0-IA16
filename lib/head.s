
.code16
.section .text

# _endbss                       # tells us where the data+bss ends.

.extern main
.extern stackpt, _endbss

.global start
start:
        jmp .L0
        .org 0x10               # kernel uses this area as stack for inital IRET
    .L0:
        mov stackpt, %sp
        call main
    .L1:
        jmp .L1                 # this will never be executed

.section .begdata               # fs needs to know where build stuffed table
.global data_org
data_org:                       # 0xDADA is magic number for build
        .fill 8, 2, 0xdada      # first 8 words of MM, FS, INIT are for stack

.section .data
.global brksize
brksize:                        # first free memory
        .word _endbss + 2
.global sp_limit
sp_limit:
        .word 0


.section .endbss
.global bss_end
bss_end:
        .fill 16, 1, 0
