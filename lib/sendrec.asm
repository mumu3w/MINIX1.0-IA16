title Send/Receive  -  C86 version; now for Turbo C [EVAS]
page,132

include prologue.h

	PUBLIC _send, _receive, _send_rec, _sendrec


; The following definitions are from  ../h/com.h

	__SEND	= 1
	__RECEIVE= 2
	__BOTH	= 3
	__SYSVEC	= 32


_TEXT	SEGMENT byte public 'CODE'

	ASSUME	CS:_TEXT,DS:DGROUP

;========================================================================
;                           send and receive                              
;========================================================================
; send(), receive(), send_rec() all save bp, but destroy ax, bx, and cx.

_send:	mov cx,__SEND		; send(dest, ptr)
	jmp L0

_receive:
	mov cx,__RECEIVE		; receive(src, ptr)
	jmp L0

_sendrec:
_send_rec:
	mov cx,__BOTH		; send_rec(srcdest, ptr)
	jmp L0

  L0:	push bp			; save bp
	mov bp,sp		; can't index off sp
	mov ax,4[bp]		; ax = dest-src
	mov bx,6[bp]		; bx = message pointer
	int __SYSVEC		; trap to the kernel
	pop bp			; restore bp
	ret			; return

_TEXT	ENDS

	END	; end of assembly-file
