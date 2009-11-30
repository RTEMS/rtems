/*
 *  TX4925 Interrupt Vectoring
 *
 *  vectorisrs.c,v 1.6 2004/06/23 18:16:36
 */

#include <rtems.h>
#include <stdlib.h>
#include <libcpu/tx4925.h>

void mips_default_isr( int vector );

#define CALL_ISR(_vector,_frame) \
  do { \
    if ( _ISR_Vector_table[_vector] ) \
      (_ISR_Vector_table[_vector])(_vector,_frame); \
    else \
      mips_default_isr(_vector); \
  } while (0)

#include <rtems/bspIo.h>  /* for printk */

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
    CALL_ISR( MIPS_INTERRUPT_BASE + v, frame );
  }

  if ( pending & 0x01 )       /* IP[0] */
    CALL_ISR( TX4925_IRQ_SOFTWARE_1, frame );

  if ( pending & 0x02 )       /* IP[1] */
    CALL_ISR( TX4925_IRQ_SOFTWARE_2, frame );
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

