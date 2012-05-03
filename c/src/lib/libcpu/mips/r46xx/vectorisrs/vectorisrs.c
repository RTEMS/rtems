#include <rtems.h>
#include <stdlib.h>


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
  unsigned int i;
  unsigned int mask;

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  /* XXX check this and think about it. */

  for ( i=1, mask=0x80 ; i<=8 ; i++, mask >>= 1 ) {
    if ( cause & mask )
      CALL_ISR( MIPS_EXCEPTION_BASE + 8 - i, frame );
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
