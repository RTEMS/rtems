/*
 *  ISR Vectoring support for the Synova Mongoose-V.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <stdlib.h>
#include <libcpu/mongoose-v.h>

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

#include <bspIo.h>  /* for printk */

void mips_default_exception( int vector )
{
  printk( "Unhandled exception %d\n", vector );
  rtems_fatal_error_occurred(1);
}

void mips_vector_isr_handlers( void )
{
  unsigned int sr;
  unsigned int cause;
  int          bit;
  unsigned int pf_icr;

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  if ( cause & 0x04 )       /* IP[0] ==> INT0 == TIMER1 */
    CALL_ISR( MONGOOSEV_IRQ_TIMER1 );
    
  if ( cause & 0x08 )       /* IP[1] ==> INT1 == TIMER2*/
    CALL_ISR( MONGOOSEV_IRQ_TIMER2 );
    
  if ( cause & 0x10 )       /* IP[2] ==> INT2 */
    CALL_ISR( MONGOOSEV_IRQ_INT2 );
    
  if ( cause & 0x20 )       /* IP[3] ==> INT4 */
    CALL_ISR( MONGOOSEV_IRQ_INT4 );
    
  if ( cause & 0x40 ) {     /* IP[4] ==> INT5 */
    pf_icr =
      MONGOOSEV_READ( MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_CAUSE_REGISTER );
    /* XXX if !pf_icr */
    for ( bit=0 ; bit <= 31 ; bit++, pf_icr >>= 1 ) {
      if ( pf_icr & 1 )
        CALL_ISR( MONGOOSEV_IRQ_PERIPHERAL_BASE + bit );
    } 
  }

  if ( cause & 0x02 )       /* SW[0] */
    CALL_ISR( MONGOOSEV_IRQ_SOFTWARE_1 );

  if ( cause & 0x01 )       /* IP[1] */
    CALL_ISR( MONGOOSEV_IRQ_SOFTWARE_2 );
}
