
.code16
.section .text

STACK_SIZE  = 2048      /* fsck栈大小 */
KERNRL_CS   = 0x60      /* 内核绝对地址(*16bytes) */

.extern main            /* c代码入口 */
.extern _begbss         /* bss段起始位置 */
.extern _endbss         /* bss段结束位置 */

.global start
start:
#ifdef DEBUG
        xchg %bx, %bx
#endif
        mov $_begbss, %bx
        # mov brksize, %cx
        # mov (brksize), %cx
        mov $_endbss, %cx
        sub %bx, %cx
        sar $1, %cx
        xor %ax, %ax
    .start_lp01:
        mov %ax, (%bx)
        add $2, %bx
        loop .start_lp01
#ifdef DEBUG
        xchg %bx, %bx
#endif

        mov $fsck_stack+STACK_SIZE, %sp
        call main
        mov %ax, %bx

        cli
        mov $KERNRL_CS, %dx
        mov %dx, %ds
        mov %dx, %es
        mov %dx, %ss
        ljmp $KERNRL_CS, $0

.global bios_putc
bios_putc:
        push %bp
        mov %sp, %bp
        push %di
        push %si
        push %ax
        push %dx

        mov 4(%bp), %al
        mov $0, %bh
        mov $0x0e, %ah
        int $0x10

        pop %dx
        pop %ax
        pop %si
        pop %di
        mov %bp, %sp
        pop %bp
        ret

.global bios_getc
bios_getc:
        xor %ah, %ah
        int $0x16
        ret

// void fmemcpyb (word_t dst_off, seg_t dst_seg, word_t src_off, seg_t src_seg, word_t count)
// segment after offset to allow LDS & LES from the stack
// assume DS=ES=SS (not ES for GCC-IA16)
	.global fmemcpyb
fmemcpyb:
	mov    %es,%bx
	mov    %si,%ax
	mov    %di,%dx
	mov    %sp,%si
	mov    10(%si),%cx  // arg4:   word count
	les    2(%si),%di   // arg0+1: far destination pointer
	lds    6(%si),%si   // arg2+3: far source pointer
	cld
	rep
	movsb
	mov    %ax,%si
	mov    %dx,%di
	mov    %ss,%ax
	mov    %ax,%ds
	mov    %bx,%es
	ret

// void fmemsetb (word_t off, seg_t seg, byte_t val, word_t count)
// segment after offset to allow LES from the stack
// compiler pushes byte_t as word_t
	.global fmemsetb
fmemsetb:
	mov    %es,%bx
	mov    %di,%dx
	mov    %sp,%di
	mov    6(%di),%ax  // arg2:   value
	mov    8(%di),%cx  // arg3:   byte count
	les    2(%di),%di  // arg0+1: far pointer
	cld
	rep
	stosb
	mov    %dx,%di
	mov    %bx,%es
	ret

// int fmemcmpb (word_t dst_off, seg_t dst_seg, word_t src_off, seg_t src_seg, word_t count)
// segment after offset to allow LDS & LES from the stack
// assume DS=SS (not ES for GCC-IA16)
	.global fmemcmpb
fmemcmpb:
	mov    %es,%bx
	mov    %si,%ax
	mov    %di,%dx
	mov    %sp,%si
	mov    10(%si),%cx  // arg4:   byte count
	les    2(%si),%di   // arg0+1: far destination pointer
	lds    6(%si),%si   // arg2+3: far source pointer
	cld
	repz
	cmpsb
	mov    %ax,%si
	mov    %dx,%di
	jz     fmemcmpb_same
	mov    $1,%ax
	jmp    fmemcmpb_exit

fmemcmpb_same:
	xor    %ax,%ax

fmemcmpb_exit:
	mov    %ss,%dx
	mov    %dx,%ds
	mov    %bx,%es
	ret

.section .begdata
        .global data_org
        data_org:
                # .word 0xdada, 0, 0, 0, 0, 0, 0, 0
                .fill 8, 2, 0xdada              /* data_org必须在data段off(0)起始处 */
                                                /* build工具将在此处写入相应模块的信息 */

.section .data
        .global brksize
        brksize:
                .word _endbss

.section .bss
        .global fsck_stack
        # .comm fsck_stack, STACK_SIZE, 1
        fsck_stack:
                .fill STACK_SIZE, 1, 0          /* 此数组作为fsck的栈 */
