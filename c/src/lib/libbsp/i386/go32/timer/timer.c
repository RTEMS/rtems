/*  Timer_init()
 *
 *  This routine initializes the timer on the IBM 386.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: It is important that the timer start/stop overhead be 
 *        determined when porting or modifying this code.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */


#include <rtems.h>
#include <cpu.h>
#include <bsp.h>

volatile rtems_unsigned32 Ttimer_val;
rtems_boolean Timer_driver_Find_average_overhead;

#if defined(pentium)
static inline unsigned long long rdtsc( void )
{
    /* Return the value of the on-chip cycle counter. */
    unsigned long long result;
    __asm __volatile( ".byte 0x0F, 0x31" : "=A" (result) );
    return result;
}
#else /* pentium */
rtems_isr timerisr();
#endif /* pentium */

void Timer_initialize()
{
#if defined(pentium)
    Ttimer_val = rdtsc();
#else /* pentium */
    static int First = 1;
    if ( First )  {
	/* install ISR */
	set_vector( timerisr, 0x8, 0 );

	/* Wait for ISR to be called at least once */
	Ttimer_val = 0;
	while ( Ttimer_val == 0 )
	    continue;

	/* load timer for 250 microsecond period */
	outport_byte( TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN );
	outport_byte( TIMER_CNTR0, US_TO_TICK(250) >> 0 & 0xff);
	outport_byte( TIMER_CNTR0, US_TO_TICK(250) >> 8 & 0xff);

	First = 0;
    }
    Ttimer_val = 0;                           /* clear timer ISR count */
#endif /* PENTIUM */
}

#define AVG_OVERHEAD	   0  /* 0.1 microseconds to start/stop timer. */
#define LEAST_VALID	   1  /* Don't trust a value lower than this */


int Read_timer()
{
    register rtems_unsigned32 total;
#if defined(pentium)
    total = rdtsc() - Ttimer_val;
#else /* pentium */
    register rtems_unsigned8 lsb, msb;
    register rtems_unsigned32 clicks;
    outport_byte( TIMER_MODE, TIMER_SEL0|TIMER_LATCH );
    inport_byte( TIMER_CNTR0, lsb );
    inport_byte( TIMER_CNTR0, msb );
    clicks = msb << 8 | lsb;
    total = Ttimer_val + 250 - TICK_TO_US( clicks );
#endif /* pentium */

    if ( Timer_driver_Find_average_overhead == 1 )
	return total;
    else if ( total < LEAST_VALID )
	return 0;		/* below timer resolution */
    else
	return total - AVG_OVERHEAD;
}

rtems_status_code Empty_function( void )
{
    return RTEMS_SUCCESSFUL;
}


void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}
