
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
    LONG (SIZEOF(.text)); /* size of text segment in bytes */
    LONG (SIZEOF(.data)); /* size of data-segment in bytes */
    LONG (SIZEOF(.bss));  /* size of bss (stack) segment   */
    LONG (start);         /* entry point */
    LONG ((SIZEOF(.data) + SIZEOF(.bss) + 8192) / 2 * 2);/* total memory allocated */
    /* total memory allocated to program (? data+bss+malloc+stack) */
    /* mm/exec.c   new_base = alloc_mem(text_clicks + tot_clicks); */
    LONG (0);             /* symbol table size */
    }
    
  _begintext = ALIGN(0x20);
  .text 0 : AT(_begintext) { *(.text); . = ALIGN(0x10); }
  ASSERT(. <= 0xfff8, "Error: text too large for a .sep file.")
  _begindata = _begintext + .;
  .data 0 : AT(_begindata) { *(.data) *(.rodata*) }
  _begbss = .;
  .bss : { *(.bss) *(COMMON); . = ALIGN(0x2); }
  _endbss = .;
  /DISCARD/ : { *(.comment) }
  ASSERT(. <= 0xfff8 - 8192, "Error: data too large for a .sep file.")
}
