
OUTPUT_FORMAT(binary)

SECTIONS {

.hdr : {
/* SHORT (0x0301); magic number */
/* BYTE (0x20);    flags for sep I&D etc */
/* BYTE (0x04);    cpu-type */
LONG (0x04100301)
/* BYTE (0x20);    length of header */
/* BYTE (0x0);     sic */
/* SHORT (0x0);    version stamp */
LONG (0x00000020)
LONG (_begdata - _begtext);	/* size of text segment in bytes */
LONG (_begbss - _begdata);	/* size of data-segment in bytes */
LONG (_endbss - _begbss);	/* size of bss (stack) segment   */
LONG (start);					/* entry point */
LONG (_endbss - _begtext);/* total memory allocated */
LONG (0);							/* symbol table size */
}

_begintext = ALIGN(0x20);
.text 0 : AT(_begintext) { _begtext = .; *(.text); . = ALIGN(0x10);
_begdata = .; *(.begdata) *(.data) *(.rodata*) . = ALIGN(0x10);}
_begbss = .; 
.bss : { *(.bss) *(COMMON); . = ALIGN(0x10); *(.endbss). = ALIGN(0x10); } 
_endbss = .;
/DISCARD/ : { *(.comment) }

}
