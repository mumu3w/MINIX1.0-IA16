
// memset.s

        .code16
        .text
        
// char *memset(char *p, int value, size_t count)

        .global memset
memset:
        mov %es, %dx
        mov %ds, %ax
        mov %ax, %es

        mov %sp, %bx
        push %di
        mov 2(%bx), %di	// address of the memory block
        mov 4(%bx), %ax	// byte to write
        mov 6(%bx), %cx	// loop count
        cld
        rep             // while (cx)
        stosb           // 	cx--, [es:di++] = al
        mov 2(%bx), %ax	// return value = start addr of block
        pop %di
        
        mov %dx, %es
        ret
        