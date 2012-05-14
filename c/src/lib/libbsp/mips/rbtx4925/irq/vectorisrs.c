/**
 *  @file
 *  
 *  TX4925 Interrupt Vectoring
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
#include <libcpu/tx4925.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <rtems/bspIo.h>  /* for printk */

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame );
void mips_default_isr( int vector );

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;
  unsigned int pending;

  mips_get_sr( sr );
  mips_get_cause( cause );

  pending = (cause & sr & 0x700) >> CAUSE_IPSHIFT;

  if ( pending & 0x4 ) {     /* (IP[2] == 1) ==> IP[3-7] are valid */
    unsigned int v = (cause >> (CAUSE_IPSHIFT + 3)) & 0x1f;
    bsp_interrupt_handler_dispatch( MIPS_INTERRUPT_BASE + v );
  }

  if ( pending & 0x01 )       /* IP[0] */
    bsp_interrupt_handler_dispatch( TX4925_IRQ_SOFTWARE_1 );

  if ( pending & 0x02 )       /* IP[1] */
    bsp_interrupt_handler_dispatch( TX4925_IRQ_SOFTWARE_2 );
}

void mips_default_isr( int vector )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled isr exception: vector 0x%02x, cause 0x%08X, sr 0x%08X\n",
      vector, cause, sr );

  while(1);	/* Lock it up */

  rtems_fatal_error_occurred(1);
}

