title catchsig  -  C86 version; now TurboC version [EVAS]
page,132

include	prologue.h

_DATA	segment	para public 'DATA'

	extrn  _vectab:word, _M:word

	dummy	 DW	 0

_DATA	ends


_TEXT	segment	byte public 'CODE'

	assume	CS:_TEXT,DS:DGROUP

	public _begsig


MTYPE = 2			; M+mtype =  &M.m_type

_begsig	proc near

	push ax			; after interrupt, save all regs
	push bx
	push cx
	push dx
	push si
	push di
	push bp
	push ds
	push es

	mov bp,sp
	mov bx,18[bp]		; bx = signal number
	dec bx			; vectab[0] is for sig 1
	add bx,bx		; pointers are two bytes on 8088
	mov bx,_vectab[bx]	; bx = address of routine to call
	push _M+mtype		; push status of last system call
	push ax			; func called with signal number as arg
	call bx
back:
	pop ax			; get signal number off stack
	pop _M+mtype		; restore status of previous system call
	pop es			; signal handling finished
	pop ds
	pop bp
	pop di
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
	pop dummy		; remove signal number from stack

	iret

_begsig	endp

_TEXT	ends


	END	; end of assembly-file
