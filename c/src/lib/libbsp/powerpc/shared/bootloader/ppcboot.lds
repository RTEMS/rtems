OUTPUT_ARCH(powerpc)
/* OUTPUT_FORMAT(ppcboot) */
/* Do we need any of these for elf?
   __DYNAMIC = 0;    */
SECTIONS
{
  .text :
  {
    /* We have to build the header by hand, painful since ppcboot
	format support is very poor in binutils. 
	objdump -b ppcboot zImage --all-headers can be used to check. */
    /* The following line can be added as a branch to use the same image
     * for netboot as for prepboots, the only problem is that objdump
     * did not in this case recognize the format since it insisted
     * in checking the x86 code area held only zeroes. 
     */
    LONG(0x48000000+start);
    . = 0x1be; BYTE(0x80); BYTE(0)
    BYTE(2); BYTE(0); BYTE(0x41); BYTE(1);
    BYTE(0x12); BYTE(0x4f); LONG(0);
    BYTE(((_edata + 0x1ff)>>9)&0xff); 
    BYTE(((_edata + 0x1ff)>>17)&0xff); 
    BYTE(((_edata + 0x1ff)>>25)&0xff);
    . = 0x1fe;
    BYTE(0x55);
    BYTE(0xaa);
    BYTE(start&0xff);
    BYTE((start>>8)&0xff);
    BYTE((start>>16)&0xff);
    BYTE((start>>24)&0xff);
    BYTE(_edata&0xff);
    BYTE((_edata>>8)&0xff);
    BYTE((_edata>>16)&0xff);
    BYTE((_edata>>24)&0xff);
    BYTE(0); /* flags */
    BYTE(0); /* os_id */
    BYTE(0x4C); BYTE(0x69); BYTE(0x6e); 
    BYTE(0x75); BYTE(0x78); /* Partition name */
    . = 0x400;
    *(.text)
    *(.sdata2)
    *(.rodata)
    *(.rodata*)
  }
/*  . = ALIGN(16); */
  .image :
  {
    rtems.gz(*)
    . = ALIGN(4);
    *.gz(*)
  }
  /* Read-write section, merged into data segment: */
  /* . = ALIGN(4096); */
  .reloc   :
  {
    *(.got)
    _GOT2_TABLE_ = .;
    *(.got2)
    _FIXUP_TABLE_ = .;
    *(.fixup)
  }

  __got2_entries = (_FIXUP_TABLE_ - _GOT2_TABLE_) >>2;
  __fixup_entries = (. - _FIXUP_TABLE_)>>2;

  .handlers : 
  {
    *(.exception)
  }

  .data    :
  {
    *(.data)
    *(.data*)
    *(.sdata)
    . = ALIGN(4);
    _edata = .;
  }
  PROVIDE(_binary_initrd_gz_start = 0);
  PROVIDE(_binary_initrd_gz_end = 0);
  _rtems_gz_size = _binary_rtems_gz_end - _binary_rtems_gz_start;
  _rtems_size = __rtems_end - __rtems_start;
  .bss :
  {
    *(.sbss)
    *(.bss)
    . = ALIGN(4);
  }
  __bss_words = SIZEOF(.bss)>>2;
  __size = . ;
  /DISCARD/ : 
  {
    *(.comment)
  }
}

