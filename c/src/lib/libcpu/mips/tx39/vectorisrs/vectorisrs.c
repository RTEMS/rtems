/*
 *  TX3904 Interrupt Vectoring
 */

#include <rtems.h>
#include <stdlib.h>
#include <libcpu/tx3904.h>

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

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  if ( cause & 0x80 )       /* IP[5] ==> INT0 */
    CALL_ISR( TX3904_IRQ_INT0, frame );

  if ( cause & 0x40 ) {     /* (IP[4] == 1) ==> IP[0-3] are valid */
    unsigned int v = (cause >> 2) & 0x0f;
    CALL_ISR( MIPS_INTERRUPT_BASE + v, frame );
  }

  if ( cause & 0x02 )       /* SW[0] */
    CALL_ISR( TX3904_IRQ_SOFTWARE_1, frame );

  if ( cause & 0x01 )       /* IP[1] */
    CALL_ISR( TX3904_IRQ_SOFTWARE_2, frame );
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
