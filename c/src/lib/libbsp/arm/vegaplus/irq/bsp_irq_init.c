/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
#include <irq.h>
#include <bsp.h>
#include <registers.h>


void BSP_rtems_irq_mngt_init() {

  /* Initialize the vector table address in internal RAM */
  Regs[INTTAB] = VECTOR_TABLE;
  
  /* Initialize the GLOBAL INT CONTROL register */
  Regs[INTGCNTL] = 0x00;

  /* Mask all the interrupts */
  Regs[INTMASK] = 0xFFFF;

  /* Unmask the 2 arm interrupts IRQ and FIQ on the INT controller */
  Regs[INTMASKALL] = 0x0;

  /* Ack pending interrupt */
  while ( ( Regs[INTSTAT] & 0xF433 ) != 0 ) {
    Regs[INTACK] = 0xFFFF;	
    Regs[INTEOI] = EOI;
  }
}
