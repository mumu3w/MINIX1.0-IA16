title Head  -  Start-off for initt, mm and fs (C86-compiler); now for Turbo C [EVAS]
page ,132

include	prologue.h

EXTRN _main:NEAR
PUBLIC $main, _data_org,	brksize, sp_limit, __end
EXTRN  _stackpt:word


_TEXT	SEGMENT byte public 'CODE'

	assume	cs:_TEXT,ds:DGROUP

$main:	jmp	short L0

	ORG 10h			; kernel uses this area	as stack for inital IRET
L0: 	mov	sp,dgroup:_stackpt
	call	_main
L1:    	jmp L1			; this will never be executed
	mov	ax,DGROUP	; force	relocation for dos2out (never executed)

_TEXT	ENDS


_DATA	SEGMENT
				; fs needs to know where build stuffed table
_data_org DW 0DADAh		; 0xDADA is magic number for build
	 DW 7 dup(0)		; first 8 words of MM, FS, INIT are for stack
brksize	 DW	offset dgroup:__end  ; first free memory
sp_limit DW	0

_DATA	ENDS

_BSSEND SEGMENT

; _BSSEND holds nothing. The label in the segment just tells
; us where the data+bss ends.

__end	label	byte

_BSSEND ENDS


@STACK	SEGMENT	BYTE STACK 'STACK'
@STACK	ENDS				; Add stack to satisfy DOS-linker

	END	$main			; end of assembly & entry-point

