/**
 *  @file
 *
 *  MRM332 C Start Up Code
 */

/*
 *  COPYRIGHT (c) 2000.
 *  Matt Cross <profesor@gweep.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <mrm332.h>
#include <rtems/m68k/sim.h>
#define __START_C__
#include "bsp.h"

rtems_isr_entry M68Kvec[256];
rtems_isr_entry vectors[256];

void  boot_card(const char *cmdline);

/*
 *  This prototype really should have the noreturn attribute but
 *  that causes a warning. Not sure how to fix that.
 */
/* void dumby_start ()  __attribute__ ((noreturn)); */
void start_c(void);

void start_c(void) {

  /* Synthesizer Control Register */
  /*    see section(s) 4.8 */
  /* end include in ram_init.S */
  *SYNCR = (unsigned short int)
    ( SAM(MRM_W,15,VCO) | SAM(0x0,14,PRESCALE) | SAM(MRM_Y,8,COUNTER) );
  while (! (*SYNCR & SLOCK));	/* protect from clock overshoot */
  /* include in ram_init.S */
  *SYNCR = (unsigned short int)
    ( SAM(MRM_W,15,VCO) | SAM(MRM_X,14,PRESCALE) | SAM(MRM_Y,8,COUNTER) );

  /* System Protection Control Register */
  /*    !!! can only write to once after reset !!! */
  /*    see section 3.8.4 of the SIM Reference Manual */
  *SYPCR = (unsigned char)( HME | BME );

  /* Periodic Interrupr Control Register */
  /*    see section 3.8.2 of the SIM Reference Manual */
  *PICR = (unsigned short int)
    ( SAM(0,8,PIRQL) | SAM(MRM_PIV,0,PIV) );
  /*     ^^^ zero disables interrupt, don't enable here or ram_init will
	 be wrong. It's enabled below. */

  /* Periodic Interrupt Timer Register */
  /*    see section 3.8.3 of the SIM Reference Manual */
  *PITR = (unsigned short int)( SAM(0x09,0,PITM) );
  /*    1.098mS interrupt, assuming 32.768 KHz input clock */

  /* Port C Data */
  /*    load values before enabled */
  *PORTC = (unsigned char) 0x0;

  /* Port E and F Data Register */
  /*    see section 9 of the SIM Reference Manual */
  *PORTE0 = (unsigned char) 0;
  *PORTF0 = (unsigned char) 0;

  /* Port E and F Data Direction Register */
  /*    see section 9 of the SIM Reference Manual */
  *DDRE = (unsigned char) 0xff;
  *DDRF = (unsigned char) 0xfd;

  /* Port E and F Pin Assignment Register */
  /*    see section 9 of the SIM Reference Manual */
  *PEPAR = (unsigned char) 0;
  *PFPAR = (unsigned char) 0;

  /* end of SIM initalization code */
  /* end include in ram_init.S */

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
      {
	*dst = 0;
      }
  }

  /*
   * Initialize vector table.
   */
  {
    rtems_isr_entry *monitors_vector_table;

    m68k_get_vbr(monitors_vector_table);

    M68Kvec[  4 ] = monitors_vector_table[  4 ];   /* breakpoints vector */
    M68Kvec[  9 ] = monitors_vector_table[  9 ];   /* trace vector */
    M68Kvec[ 31 ] = monitors_vector_table[ 31 ];   /* level 7 interrupt */
    M68Kvec[ 47 ] = monitors_vector_table[ 47 ];   /* system call vector */
    M68Kvec[ 66 ] = monitors_vector_table[ 66 ];   /* user defined */

    m68k_set_vbr(&M68Kvec);
  }

  /*
   * Initalize the board.
   */

  /* Spurious should be called in the predriver hook */
  /* Spurious_Initialize(); */
  /*console_init(); */

  /*
   * Execute main with arguments argc and agrv.
   */
  boot_card((void*)0);
  reboot();

}
