/*
 *  $Id$
 */

#include <rtems.h>
#include <stdlib.h>
#include <libcpu/tx3904.h>

#define mips_get_cause( _cause ) \
  do { \
    asm volatile( "mfc0 %0, $13; nop" : "=g" (_cause) :  ); \
  } while (0)

#define CALL_ISR(_vector) \
    (_ISR_Vector_table[_vector])(_vector);

void mips_vector_isr_handlers( void )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  if ( cause & 0x80 )       /* IP[5] ==> INT0 */
    CALL_ISR( TX3904_IRQ_INT0 );

  if ( cause & 0x40 ) {     /* (IP[4] == 1) ==> IP[0-3] are valid */
    unsigned int v = (cause >> 2) & 0x0f;
    CALL_ISR( v );
  }
   
  if ( cause & 0x02 )       /* SW[0] */
    CALL_ISR( TX3904_IRQ_SOFTWARE_1 );

  if ( cause & 0x01 )       /* IP[1] */
    CALL_ISR( TX3904_IRQ_SOFTWARE_2 );
}
