/*
 *-------------------------------------------------------------------
 *
 * This file contains the subroutines necessary to initalize
 * and maintain the MAX791 based watchdog timer used on efi68k
 *
 * This file has been created by John S. Gwynne for the efi68k
 * project.
 * 
 * Redistribution and use in source and binary forms are permitted
 * provided that the following conditions are met:
 * 1. Redistribution of source code and documentation must retain
 *    the above authorship, this list of conditions and the
 *    following disclaimer.
 * 2. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * This software is provided "AS IS" without warranty of any kind,
 * either expressed or implied, including, but not limited to, the
 * implied warranties of merchantability, title and fitness for a
 * particular purpose.
 *
 *------------------------------------------------------------------
 *
 *  $Id$
 */
#include <bsp.h>

void wd_interrupt(void) {
  /* toggle WDI of the MAX791. A more sophisticated routine
     can be inserted into the exception table after booting. */

  /* 3 changes guaranty a pulse independent of initial state */
  *MCR |= OUT1;
  *MCR &= ~OUT1;
  *MCR |= OUT1;
}

/* _catchWDint is the interrupt front-end */
extern void _catchWDint();
asm("   .text
        .align 2
        .globl _catchWDint
_catchWDint:
        lea    %sp@(4),%sp                /* pop return address */
        moveml %d0-%d7/%a0-%a6,%sp@-       /* save registers */
        jbsr    wd_interrupt
        moveml  %sp@+,%d0-%d7/%a0-%a6			        
        rte
    ");

void watch_dog_init(void) {
  set_vector(_catchWDint, WD_ISR_LEVEL+24, 0);
}
