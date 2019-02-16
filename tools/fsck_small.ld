
OUTPUT_FORMAT(binary)

SECTIONS {

	.hdr : {
    /* SHORT (0x0301); magic number */
		/* BYTE (0x20);    flags for sep I&D etc */
		/* BYTE (0x04);    cpu-type */
		LONG (0x04200301)
		/* BYTE (0x20);    length of header */
		/* BYTE (0x0);     sic */
		/* SHORT (0x0);    version stamp */
		LONG (0x00000020)
		LONG (SIZEOF(.text));	/* size of text segment in bytes */
		LONG (SIZEOF(.data));	/* size of data-segment in bytes */
		LONG (SIZEOF(.bss));	/* size of bss (stack) segment   */
		LONG (start);					/* entry point */
		LONG (SIZEOF(.text) + SIZEOF(.data) + SIZEOF(.bss));/* total memory allocated */
		LONG (0);							/* symbol table size */
		}
    
	_begintext = ALIGN(0x20);
	.text 0 : AT(_begintext) { *(.text); . = ALIGN(0x10); }
	_begindata = _begintext + .;
	.data 0 : AT(_begindata) { fsck1.o(.begdata) *(.data) *(.rodata*) }
  _begbss = .;
	.bss : { *(.bss) *(COMMON) }
  _endbss = .;
	/DISCARD/ : { *(.comment) }
  
}
