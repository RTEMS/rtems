/**
 *  @file
 *
 *  RM5231 Interrupt Vectoring  
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
#include <libcpu/rm5231.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <rtems/bspIo.h>  /* for printk */

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame );
void mips_default_isr( int vector );

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;
  unsigned int i;
  unsigned int mask;

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  for ( i=1, mask=0x80 ; i<=8 ; i++, mask >>= 1 ) {
    if ( cause & mask )
      bsp_interrupt_handler_dispatch( MIPS_INTERRUPT_BASE + 8 - i );
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
  rtems_fatal_error_occurred(1);
}

