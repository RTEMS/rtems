/*
 *  $Id$
 */

#include <efi68k.h>
#define __START_C__
#include "bsp.h"

m68k_isr_entry M68Kvec[256];
m68k_isr_entry vectors[256];
char * const __argv[]= {"main", ""};
char * const __env[]= {""};

/*
 *  This prototype really should have the noreturn attribute but
 *  that causes a warning since it appears that the routine does
 *  return.
 *
 *   void dumby_start ()  __attribute__ ((noreturn));
 */

void dumby_start ();
void  dumby_start() {

  /* We need to by-pass the link instruction since the RAM chip-
     select pins are not yet configured. */
  asm volatile ( ".global start ;
                  start:");

  /* disable interrupts, load stack pointer */
  asm volatile ( "oriw  #0x0700, %sr;
                  movel  #_end, %d0;
                  addl   " STACK_SIZE ",%d0;
                  movel  %d0,%sp;
                  link %a6, #0"
		  );
  /*
   * Initialize RAM by copying the .data section out of ROM (if
   * needed) and "zero-ing" the .bss section.
   */
  {
    register char *src = _endtext;
    register char *dst = _sdata;

    if (_copy_data_from_rom)
      /* ROM has data at end of text; copy it. */
      while (dst < _edata)
	*dst++ = *src++;
    
    /* Zero bss */
    for (dst = __bss_start; dst< _end; dst++)
      *dst = 0;
  }

  /*
   * Initalize the board.
   */
  Spurious_Initialize();
  console_init();
  watch_dog_init();
  tcp_init();

  /*
   * Execute main with arguments argv and environment env
   */
  /* main(1, __argv, __env); */
  boot_card();

  reboot();
}
