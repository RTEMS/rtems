/*
 *-------------------------------------------------------------------
 *
 * This file contains the subroutines necessary to initalize
 * and maintain the MAX791 based watchdog timer used on efi68k
 *
 * This file has been created by John S. Gwynne for the efi68k
 * project.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
