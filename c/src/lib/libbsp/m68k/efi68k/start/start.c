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
void  boot_card();

  /* We need to by-pass the link instruction since the RAM chip-
     select pins are not yet configured. */
  asm volatile ( ".global start ;\n\
                  start:");

  /* disable interrupts, load stack pointer */
  asm volatile ( "oriw  #0x0700, %sr;\n\
                  movel  #end, %d0;\n\
                  addl   " STACK_SIZE ",%d0;\n\
                  movel  %d0,%sp;\n\
                  link %a6, #0"\n\
		  );
  /*
   * Initialize RAM by copying the .data section out of ROM (if
   * needed) and "zero-ing" the .bss section.
   */
  {
    register char *src = _etext;
    register char *dst = _copy_start;

    if (_copy_data_from_rom)
      /* ROM has data at end of text; copy it. */
      while (dst < _edata)
	*dst++ = *src++;
    
    /* Zero bss */
    for (dst = _clear_start; dst< end; dst++)
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
