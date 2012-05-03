/**
 *  @file
 *  
 *  ISR Vectoring support for the Synova Mongoose-V.
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
#include <libcpu/mongoose-v.h>

#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#include <rtems/irq.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <rtems/bspIo.h>  /* for printk */

int mips_default_isr( int vector );
int assertSoftwareInterrupt( uint32_t   n );
void mips_vector_isr_handlers( CPU_Interrupt_frame *frame );

int mips_default_isr( int vector )
{
  unsigned int sr, sr2;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  sr2 = sr & ~0xffff;
  mips_set_sr(sr2);

  printk( "Unhandled isr exception: vector 0x%02x, cause 0x%08X, sr 0x%08X\n", vector, cause, sr );
  rtems_fatal_error_occurred(1);
  return 0;
}

/* userspace routine to assert either software interrupt */

int assertSoftwareInterrupt( uint32_t   n )
{
   if( n<2 )
   {
      uint32_t   c;

      mips_get_cause(c);
      c = ((n+1) << CAUSE_IPSHIFT);
      mips_set_cause(c);

      return n;
   }
   else return -1;
}


/*
 * Instrumentation tweaks for isr timing measurement, turning them off
 * via this #if will remove the code entirely from the RTEMS kernel.
 */

#if 0
#define SET_ISR_FLAG( offset )	*((uint32_t*)(0x8001e000+offset)) = 1;
#define CLR_ISR_FLAG( offset )	*((uint32_t*)(0x8001e000+offset)) = 0;
#else
#define SET_ISR_FLAG( offset )
#define CLR_ISR_FLAG( offset )
#endif






static volatile uint32_t   _ivcause, _ivsr;


static uint32_t   READ_CAUSE(void)
{
   mips_get_cause( _ivcause );
   _ivcause &= SR_IMASK;	/* mask off everything other than the interrupt bits */

   return ((_ivcause & (_ivsr & SR_IMASK)) >> CAUSE_IPSHIFT);
}



/*
 * This rather strangely coded routine enforces an interrupt priority
 * scheme.  As it runs thru finding whichever interrupt caused it to get
 * here, it test for other interrupts arriving in the meantime (maybe it
 * occured while the vector code is executing for instance).  Each new
 * interrupt will be served in order of its priority.  In an effort to
 * minimize overhead, the cause register is only fetched after an
 * interrupt is serviced.  Because of the intvect goto's, this routine
 * will only exit when all interrupts have been serviced and no more
 * have arrived, this improves interrupt latency at the cost of
 * increasing scheduling jitter; though scheduling jitter should only
 * become apparent in high interrupt load conditions.
 */
void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
   uint32_t  	cshifted;

   /* mips_get_sr( sr ); */
   _ivsr = frame->c0_sr;

   cshifted = READ_CAUSE();

  intvect:

   if( cshifted & 0x3 )
   {
      /* making the software interrupt the highest priority is kind of
       * stupid, but it makes the bit testing lots easier.  On the other
       * hand, these ints are infrequently used and the testing overhead
       * is minimal.  Who knows, high-priority software ints might be
       * handy in some situation.
       */

      /* unset both software int cause bits */
      mips_set_cause( _ivcause & ~(3 << CAUSE_IPSHIFT) );

      if ( cshifted & 0x01 )       /* SW[0] */
      {
	 bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_SOFTWARE_1 );
      }
      if ( cshifted & 0x02 )       /* SW[1] */
      {
	 bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_SOFTWARE_2 );
      }
      cshifted = READ_CAUSE();
   }


   if ( cshifted & 0x04 )       /* IP[0] ==> INT0 == TIMER1 */
   {
      SET_ISR_FLAG( 0x4 );
      bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_TIMER1 );
      CLR_ISR_FLAG( 0x4 );
      if( (cshifted = READ_CAUSE()) & 0x3 ) goto intvect;
   }

   if ( cshifted & 0x08 )       /* IP[1] ==> INT1 == TIMER2*/
   {
      SET_ISR_FLAG( 0x8 );
      bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_TIMER2 );
      CLR_ISR_FLAG( 0x8 );
      if( (cshifted = READ_CAUSE()) & 0x7 ) goto intvect;
   }

   if ( cshifted & 0x10 )       /* IP[2] ==> INT2 */
   {
      SET_ISR_FLAG( 0x10 );
      bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_INT2 );
      CLR_ISR_FLAG( 0x10 );
      if( (cshifted = READ_CAUSE()) & 0xf ) goto intvect;
   }

   if ( cshifted & 0x20 )       /* IP[3] ==> INT3 == FPU interrupt */
   {
      SET_ISR_FLAG( 0x20 );
      bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_INT3 );
      CLR_ISR_FLAG( 0x20 );
      if( (cshifted = READ_CAUSE()) & 0x1f ) goto intvect;
   }

   if ( cshifted & 0x40 )       /* IP[4] ==> INT4, external interrupt */
   {
      SET_ISR_FLAG( 0x40 );
      bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_INT4 );
      CLR_ISR_FLAG( 0x40 );
      if( (cshifted = READ_CAUSE()) & 0x3f ) goto intvect;
   }

   if ( cshifted & 0x80 )       /* IP[5] ==> INT5, peripheral interrupt */
   {
      uint32_t    bit;
      uint32_t    pf_icr, pf_mask, pf_reset = 0;
      uint32_t    i, m;

      pf_icr = MONGOOSEV_READ( MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_CAUSE_REGISTER );

/*
      for (bit=0, pf_mask = 1;    bit < 32;     bit++, pf_mask <<= 1 )
      {
	 if ( pf_icr & pf_mask )
	 {
	    SET_ISR_FLAG( 0x80 + (bit*4) );
	    bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_PERIPHERAL_BASE + bit );
	    CLR_ISR_FLAG( 0x80 + (bit*4) );
	    pf_reset |= pf_mask;
	    if( (cshifted = READ_CAUSE()) & 0xff ) break;
	 }
      }
*/

      /*
       * iterate thru 32 bits in 4 chunks of 8 bits each.  This lets us
       * quickly get past unasserted interrupts instead of flogging our
       * way thru a full 32 bits.  pf_mask shifts left 8 bits at a time
       * to serve as a interrupt cause test mask.
       */
      for( bit=0, pf_mask = 0xff;    (bit < 32 && pf_icr);     (bit+=8, pf_mask <<= 8) )
      {
	 if ( pf_icr & pf_mask )
	 {
	    /* one or more of the 8 bits we're testing is high */

	    m = (1 << bit);

	    /* iterate thru the 8 bits, servicing any of the interrupts */
	    for(i=0; (i<8 && pf_icr); (i++, m <<= 1))
	    {
	       if( pf_icr & m )
	       {
		  SET_ISR_FLAG( 0x80 + ((bit + i) * 4) );
		  bsp_interrupt_handler_dispatch( MONGOOSEV_IRQ_PERIPHERAL_BASE + bit + i );
		  CLR_ISR_FLAG( 0x80 + ((bit + i) * 4) );

		  /* or each serviced interrupt into our interrupt clear mask */
		  pf_reset |= m;

		  /* xor off each int we service so we can immediately
		   * exit once we get the last one
                   */
		  pf_icr   %= m;

		  /* if another interrupt has arrived, jump out right
		   * away but be sure to reset all the interrupts we've
		   * already serviced
                   */
		  if( READ_CAUSE() & 0xff ) goto pfexit;
	       }
	    }
	 }
      }
     pfexit:
      MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_STATUS_REGISTER, pf_reset );
   }

   /*
    * this is a last ditch interrupt check, if an interrupt arrives
    * after this step, servicing it will incur the entire interrupt
    * overhead cost.
    */
   if( (cshifted = READ_CAUSE()) & 0xff ) goto intvect;
}
