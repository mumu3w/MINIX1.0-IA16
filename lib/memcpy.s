
// memcpy.s

        .code16
        .text
        
// void *memcpy(char *dec, char *src, size_t n)
	.global memcpy
memcpy:
	mov %es, %bx
	mov %si, %ax
	mov %di, %dx
	mov %sp, %si

        mov %ds, %cx
        mov %cx, %es
        mov 2(%si), %di
        mov 4(%si), %si
        mov 6(%si), %cx
        cld
	rep
	movsb

	mov %ax, %si
	mov %dx, %di
	mov %bx, %es
        mov %di, %ax
	ret
        