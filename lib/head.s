
.code16
.section .text

.extern main
.extern stackpt, _endbss

.global start
start:
        jmp .L0
        .org 0x10
    .L0:
        mov stackpt, %sp
        call main
    .L1:
        jmp .L1

.section .begdata
.global data_org
data_org:
        .fill 8, 2, 0xdada

.section .data
.global brksize
brksize:
        .word _endbss
.global sp_limit
sp_limit:
        .word 0
