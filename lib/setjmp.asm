title setjmp/longjmp  -  C86 version
page,132

include ..\lib\prologue.h


	PUBLIC _setjmp, _longjmp
	EXTRN _exit:near


; 	struct jmpbuf {
;		int bp;
;	 	int sp;
;	 	int ret-addr;
; 	}

JMPERR	EQU	-99		; call _exit(JMPERR) when jmp-error

_TEXT	SEGMENT byte public 'CODE'

	ASSUME	CS:_TEXT,DS:DGROUP

_setjmp:
	mov	bx,sp
	mov	ax,[bx]		; ret-addr.
	mov	bx,2[bx]	; addr of jmp-struct
	mov	[bx],bp
	mov	2[bx],sp
	mov	4[bx],ax
	xor	ax,ax
	ret

_longjmp:
	push	bp
	mov	bp,sp		; set new frame pointer to sp
	mov	bx,4[bp]	; get address of jmp-structure
	mov	ax,6[bp]	; get ret-code
	or	ax,ax
	jne	L1
	inc	ax		; return code may not be zero
    L1:	mov	sp,[bx+2]
	mov	bp,[bx]
	or	bp,bp		; test if last frame-pointer (error)
	jne	L2		; else execute the longjmp
	pop	bp
	mov	ax,JMPERR
	push	ax
	call	_exit		; should never return
	hlt
    L2:	mov	bx,[bx+4]
	pop	cx		; dump the bp we pushed upon entry
	jmp	bx

_TEXT	ENDS

	END
