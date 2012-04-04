/**
 *  @file
 *
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <stdlib.h>
#include <bsp/irq-generic.h>
#include <bsp/pci.h>
#include <bsp/i8259.h>
#include <bsp.h>

void mips_default_isr( int vector );
void mips_vector_isr_handlers( CPU_Interrupt_frame *frame );

#include <rtems/bspIo.h>  /* for printk */

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;
  unsigned int pending;

  mips_get_sr( sr );
  mips_get_cause( cause );

  pending = (cause & sr & 0xff00) >> CAUSE_IPSHIFT;

  /* SW Bits */
  if ( pending & 0x01) {
    printk("Pending IRQ Q 0x%x\n", pending );
  }

  if ( pending & 0x02) {
    printk("Pending IRQ Q 0x%x\n", pending );
  }

  /* South Bridge Interrupt */
  if ( pending & 0x04) {
     BSP_i8259s_int_process();
  }

  /* South Bridge SMI */
  if (pending & 0x08){
    printk( "Pending IRQ 0x%x\n", pending );
  }

  /* TTY 2 */
  if (pending & 0x10) {
    printk( "Pending IRQ 0x%x\n", pending );
  }
  /* Core HI */
  if (pending & 0x20) {
    printk( "Pending IRQ 0x%x\n", pending );
  }
   /* Core LO */
  if (pending & 0x40) {
    printk( "Pending IRQ 0x%x\n", pending );
  }

  if ( pending & 0x80 ) {
    bsp_interrupt_handler_dispatch( MALTA_INT_TICKER );
  }
}

void mips_default_isr( int vector )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled isr exception: vector 0x%02x, cause 0x%08X, sr 0x%08X\n",
      vector, cause, sr );

  while(1);      /* Lock it up */

  rtems_fatal_error_occurred(1);
}

