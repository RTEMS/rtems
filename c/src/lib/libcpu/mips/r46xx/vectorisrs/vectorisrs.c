/*
 *  $Id$
 */

#include <rtems.h>
#include <stdlib.h>

#define mips_get_cause( _cause ) \
  do { \
    asm volatile( "mfc0 %0, $13; nop" : "=r" (_cause) :  ); \
  } while (0)

#define CALL_ISR(_vector) \
  do { \
    if ( _ISR_Vector_table[_vector] ) \
      (_ISR_Vector_table[_vector])(_vector); \
    else \
      mips_default_exception(_vector); \
  } while (0)

void mips_default_exception( int vector )
{
  printk( "Unhandled exception %d\n", vector );
  rtems_fatal_error_occurred(1);
}

void mips_vector_isr_handlers( void )
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
      CALL_ISR( 8 - i );
  }
}
