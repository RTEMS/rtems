MEMORY
{
    NUL : ORIGIN = 0xa00f0000, LENGTH = 0
    RAM : ORIGIN = 0xa00fe000, LENGTH = 8K
    ROM : ORIGIN = 0xbfc00000, LENGTH = 8K
}

SECTIONS
{
  /* Initialized data is _not_ supported.  */
  /* Assign it to an empty region in order */
  /* to force a link error if any exists.  */
  .data 0xa00f0000 (NOLOAD): {
    _fdata = .;
    *(.data)
    . = ALIGN(8);
    _gp = . + 0x8000;
    *(.sdata)
    . = ALIGN(8);
    _edata = .;
  } >NUL

  /* Assign uninitialized read/write data to RAM. */
  .bss 0xa00fe000 (NOLOAD): {
    _fbss = .;
    stubinit.o(.bss)
    *(.sbss)
    *(.bss)
    *(.scommon)
    *(COMMON)
    . = ALIGN(8);
    _end = .;
  } >RAM

  /* Assign code and read-only data to ROM.  This */
  /* section MUST start at the reset address,     */
  /* and the reset code MUST be linked first.     */
  .text 0xbfc00000: {
     _ftext = .;
    stubinit.o(.text)
    . = ALIGN(8);
    r46kstub.o(.text)
    . = ALIGN(8);
    *(.rdata)
    . = ALIGN(8);
    *(.rodata)
    . = ALIGN(8);
     _etext = .;
  } >ROM = 0

}

ENTRY(_reset_exception)
