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

#include "iregdef.h"
#include "idtcpu.h"


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
  unsigned32  sr, srmaskoff;
  unsigned32  cause, cshifted;
  unsigned32  bit;
  unsigned32  pf_icr;

  /* mips_get_sr( sr ); */
  sr = frame->regs[ R_SR ];

  mips_get_cause( cause );

  /* mask off everything other than the interrupt bits */
  cause &= SR_IMASK;

  /* mask off the pending interrupts in the status register */
  srmaskoff = sr & ~cause;
  mips_set_sr( srmaskoff );

  /* allow nesting for all non-pending interrupts */
  asm volatile( "rfe" );

  cshifted = (cause & (sr & SR_IMASK)) >> CAUSE_IPSHIFT;

  if ( cshifted & 0x04 )       /* IP[0] ==> INT0 == TIMER1 */
    CALL_ISR( MONGOOSEV_IRQ_TIMER1, frame );
    
  if ( cshifted & 0x08 )       /* IP[1] ==> INT1 == TIMER2*/
    CALL_ISR( MONGOOSEV_IRQ_TIMER2, frame );
    
  if ( cshifted & 0x10 )       /* IP[2] ==> INT2 */
    CALL_ISR( MONGOOSEV_IRQ_INT2, frame );
    
  if ( cshifted & 0x20 )       /* IP[3] ==> INT3 == FPU interrupt */
    CALL_ISR( MONGOOSEV_IRQ_INT3, frame );
    
  if ( cshifted & 0x40 )       /* IP[4] ==> INT4, external interrupt */
    CALL_ISR( MONGOOSEV_IRQ_INT4, frame );

  if ( cshifted & 0x80 )       /* IP[5] ==> INT5, peripheral interrupt */
  {
     pf_icr = MONGOOSEV_READ( MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_CAUSE_REGISTER );

     /* if !pf_icr */
     for ( bit=0 ; bit <= 31 ; bit++, pf_icr >>= 1 ) 
     {
	if ( pf_icr & 1 )
	{
	   CALL_ISR( MONGOOSEV_IRQ_PERIPHERAL_BASE + bit, frame );
	}
     } 
  }


  /* all the pending interrupts were serviced, now re-enable them */
  mips_get_sr( sr );

  /* we allow the 2 software interrupts to nest freely, under the
   * assumption that the program knows what its doing...  
   */

  if( cshifted & 0x3 )
  {
     sr |= (SR_IBIT1 | SR_IBIT1);
     cause &= ~(SR_IBIT1 | SR_IBIT1);

     mips_set_cause(cause);
     mips_set_sr(sr);

     if ( cshifted & 0x01 )       /* SW[0] */
     {
	CALL_ISR( MONGOOSEV_IRQ_SOFTWARE_1, frame );
     } 
     if ( cshifted & 0x02 )       /* SW[1] */
     {
	CALL_ISR( MONGOOSEV_IRQ_SOFTWARE_2, frame );
     }
  }

  sr |= cause;
  mips_set_sr( sr );
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

/* userspace routine to assert either software interrupt */

int assertSoftwareInterrupt( unsigned32 n )
{
   if( n >= 0 && n<2 )
   {
      unsigned32 c;

      mips_get_cause(c);
      c = ((n+1) << 8);
      mips_set_cause(c);

      return n;
   }
   else return -1;
}

