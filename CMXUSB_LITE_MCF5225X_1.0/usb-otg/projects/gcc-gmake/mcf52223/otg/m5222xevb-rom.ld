OUTPUT_ARCH(m68k)
ENTRY(__start)
SEARCH_DIR(.)
__DYNAMIC  =  0;

MEMORY
{
  rom (rx) : ORIGIN = 0x0, LENGTH = 0x40000
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000
}

/* Place the stack at the end of memory, unless specified otherwise. */
PROVIDE (__SRAM    = 0x20000000);
PROVIDE (_SRAM    = 0x20000000);
PROVIDE (__IPSBAR  = 0x40000000);
PROVIDE (_IPSBAR  = 0x40000000);

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors))  
    *(.text .text.*)
    *(.rodata .rodata.*)
  } >rom

  .data : AT ( ADDR (.text) + SIZEOF (.text) )
  {
    __COPY_DST = .;
    *(.shdata)
    *(.data .data.*)
    __COPY_END = .;
  } >ram

  .bss :
  {
    __ZERO_START = . ;
    *(.shbss)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    __ZERO_END = .;
  } >ram
  
  .bdt : 
  {
    . = ALIGN (512);
    _BDT_BASE = . ;
    . = . + 512 ;
  } >ram

  .stack :
  {
    . = ALIGN (4) ;
    . = . + 4K ;
    __SP_INIT = . ;
  }>ram

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

}
  __COPY_START = LOADADDR (.data);
