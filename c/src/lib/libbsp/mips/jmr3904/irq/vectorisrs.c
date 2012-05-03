/**
 *  @file
 *  
 *  TX3904 Interrupt Vectoring
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
#include <bsp/irq.h>
#include <rtems/bspIo.h>  /* for printk */
#include <bsp/irq-generic.h>

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame );

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  if ( cause & 0x80 )       /* IP[5] ==> INT0 */
    bsp_interrupt_handler_dispatch( TX3904_IRQ_INT0 );

  if ( cause & 0x40 ) {     /* (IP[4] == 1) ==> IP[0-3] are valid */
    unsigned int v = (cause >> 2) & 0x0f;
    bsp_interrupt_handler_dispatch( MIPS_INTERRUPT_BASE + v );
  }

  if ( cause & 0x02 )       /* SW[0] */
    bsp_interrupt_handler_dispatch( TX3904_IRQ_SOFTWARE_1 );

  if ( cause & 0x01 )       /* IP[1] */
    bsp_interrupt_handler_dispatch( TX3904_IRQ_SOFTWARE_2 );
}
