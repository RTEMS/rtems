/*------------------------------------*/
/* rom.ld                             */
/* Last change : 19. 4.95             */
/*------------------------------------*
 * To build ROM based i960 image.
 *------------------------------------*/
/*
 *  $Id$
 */



MEMORY
{
  dram   : org = 0xa2000000, len = 120K
}


_bootAddr = 0xa0200000;
_HEAP = 0xA0100000 ;

SECTIONS 
{
    /* Only monitor start point and fault handler
     * will live in ROM as far as text is concerned.
     * Only fault table will live in ROM as far as data
     * is concerned.  
     */
  romCode :
    {
        /* Make sure that the monitor start point
         * is the first location in EPROM.
         */
      rommon.o960(.text)
        /* Procedures to copy code and
         * initialize bss in RAM.
         */
      sctns.o960(.text)   
        /* Make Sure Fault Table (and its handler's data)
         * live here so that they wouldn't get destroyed).
         */
      asmfault.o960   
      flttbl.o960 
        /* 16 byte aligned PRCB.
         */
      . = ALIGN(16);
      rom_prcb.o960(.data)
        /* 16 byte aligned Control Table.
         */
      . = ALIGN(16);
      rom_cntrltbl.o960(.data)       
      . = ALIGN(16);
	intrtbl.o960(.data)
      . = ALIGN(16);
	nulsystbl.o960(.data)
      . = ALIGN(16);
        /* I need this symbol to know where code which is
         * to be copied reside in ROM. Align it on a 16
         * boundary.
         */        
      . = ALIGN(16);  
      _codeRomStart = .;   
    } > dram 
    
    /* All the rest of the code will live in RAM.
     * Relocation are created as though code resides
     * in RAM, while code is placed right after romCode.
     * This is responsiblity of the ROM monitor to
     * copy code into ROM.
     */    
  ramCode : AT(ADDR(romCode) + SIZEOF(romCode)) 
    { 
        /* RAM-based code section start.
         * I need this symbol to know where to copy code
         * at initialization time .
         */
      _codeRamStart = .;
        /* RAM based fault recovery stuff.
         */
      _faultStart = .;
      fault.o960
      _faultEnd = .;
        /* Check sum to gurantee that
         * the above section wasn't broken.
         */
      . = ALIGN(16); 
      _faultCheckSum = .;
      . += 4;
        /* Fault Buffer to keep the state of
         * the fauled procedure.
         */
      _faultBuffer = .;
      . += 256;   
        /* All the rest of the text goes here.
         */
      . = ALIGN(16);   
      *(.text)
        /* 16 byte aligned PRCB.
         */
      . = ALIGN(16);
      prcb.o960(.data)
        /* 16 byte aligned Control Table.
         */
      . = ALIGN(16);
      cntrltbl.o960(.data)                 
      . = ALIGN(16);
	systbl.o960(.data)
        /* All the rest of program defined data goes here.
         */
      *(.data) 
        /* RAM-based code section end.
         * I need this symbol to know where to copy code
         * at initialization time .
         */   
      _codeRamEnd = .;
    } > sram
 
    /* RAM based uninitialized data.
     */   
  bss (NOLOAD) :
    {
        /* BSS section start. I need this symbol to
         * zero BSS on initialization.
         */
      _bssStart = .;
        /* Supervisor Stack. Aligned on a 16 boundary.
         */
      . = ALIGN(16);
      _svrStackPtr = .;
      . += 4K;
        /* Interrupt Stack. Aligned on a 16 boundary.
         */
      . = ALIGN(16);
      _intStackPtr = .;
      . += 4K;
        /* Program defined BSS.
         */
      *(.bss)
        /* Program defined COMMON.
         */
      *(COMMON) 
        /* BSS section end. I need this symbol to
         * zero BSS on initialization.
         */
      _bssEnd = .;
      _bssStart_1 = .;
      _bssEnd_1 = .;
    } > dram

}

/*-------------*/
/* End of file */
/*-------------*/
