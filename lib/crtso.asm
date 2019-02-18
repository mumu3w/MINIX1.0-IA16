Title $MAIN    C-86 run-time start-off for Minix; Now for Turbo C [EVAS]
page,132


	PAGE	60,132
;[]------------------------------------------------------------[]
;|      start-up module for using Turbo C under MINIX; 		|
;|	derived from:						|
;|								|
;|	C0.ASM -- Start Up Code					|
;|								|
;|	Turbo-C Run Time Library	version 1.0		|
;|								|
;|	Copyright (c) 1987 by Borland International Inc.	|
;|	All Rights Reserved.					|
;[]------------------------------------------------------------[]

include	prologue.h

	ASSUME	CS:_TEXT, DS:DGROUP

;	External References


PUBLIC	__end, _brksize, edata, $main, _environ, kamikaze


STACKSIZE EQU 2560	; default stack is 5 Kb (2K words)

_DATA	segment	para public 'DATA'

_brksize	DW	offset dgroup:__end+2  	; dynamic changeable end of bss
_environ	DW	0			; save environment pointer here

_DATA	ends


_DATAEND	segment	para public 'DATA'

; _DATAEND holds nothing. The label just tells us where
; the initialized data (.data) ends.

edata	label byte

_DATAEND ENDS

_BSSEND SEGMENT BYTE PUBLIC 'BSS'

; _BSSEND holds nothing. The label in the segment just tells
; us where the data+bss ends.

__end	label	byte

_BSSEND ENDS


@STACK	SEGMENT	BYTE STACK 'STACK'
	DW	STACKSIZE dup(?)	; add stack segment to bss
@STACK	ENDS


_TEXT	segment	byte public 'CODE'
	ASSUME	CS:_TEXT,DS:DGROUP

EXTRN   _main:NEAR, _exit:NEAR

; This is the C run-time start-off routine.  It's job is to take the
; arguments as put on the stack by EXEC, and to parse them and set them up the
; way main expects them.
;

$main:
	mov	ax,DGROUP	; force relocation of data & bss
	mov	bx,sp		; set stackframe pointer (ds=ss)
	mov	cx,[bx]		; get argc
	add	bx,2		; point at next parameter
	mov	ax,cx
	inc	ax		; calculate envp
	shl	ax,1
	add	ax,bx
	mov	_environ,ax	; save envp
	push	ax		; stack envp
	push	bx		; stack argv
	push	cx		; stack argc

	call	_main		; call _main(arc,argv,envp)

	add	sp,6
	push	ax		; stack program-termination status
	call	_exit		; this will never return

	; DEBUG from here on

kamikaze: 	int 3
		ret

_TEXT	ENDS


	END	$main	; program entry-point (could be anywhere)
