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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
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
#else
#define US_PER_ISR  250
extern rtems_isr timerisr();
static rtems_isr_entry Old_Ticker;
#endif

static void Timer_exit( void )
{
#if defined(pentium)
    CLOCK_ENABLE();
#else /* pentium */
    extern void rtc_set_dos_date( void );

    /* reset to DOS value: */
    outport_byte( TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN );
    outport_byte( TIMER_CNTR0, 0 );
    outport_byte( TIMER_CNTR0, 0 );

    /* reset time-of-day */
    rtc_set_dos_date();
        
    /* re-enable old handler: assume it was one of ours */
    set_vector( (rtems_isr_entry)Old_Ticker, 0x8, 0 );
#endif /* pentium */
}

void Timer_initialize()
{
    extern int atexit( void (*)(void) );

    static int First = 1;

    if ( First )  {
        First = 0;

        /* Try not to hose the system on return to DOS. */
        atexit( Timer_exit );

#if defined(pentium)
        /* Disable the programmable timer so ticks don't interfere. */
        CLOCK_DISABLE();
#else /* pentium */
        /* install a timer ISR */
        Old_Ticker = (rtems_isr_entry) set_vector( timerisr, 0x8, 0 );

        /* Wait for ISR to be called at least once */
        Ttimer_val = 0;
        while ( Ttimer_val == 0 )
            continue;

        /* load timer for US_PER_ISR microsecond period */
        outport_byte( TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN );
        outport_byte( TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 0 & 0xff );
        outport_byte( TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 8 & 0xff );
#endif /* PENTIUM */
    }
#if defined(pentium)
    Ttimer_val = rdtsc();       /* read starting time */
#else
    /* Wait for ISR to be called at least once */
    asm( "sti" );
    Ttimer_val = 0;
    while ( Ttimer_val == 0 )
        continue;
    Ttimer_val = 0;
#endif
}

#define AVG_OVERHEAD       0  /* 0.1 microseconds to start/stop timer. */
#define LEAST_VALID        1  /* Don't trust a value lower than this */


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
    total = (Ttimer_val * US_PER_ISR) + (US_PER_ISR - TICK_TO_US( clicks ));
#endif /* pentium */

    if ( Timer_driver_Find_average_overhead == 1 )
        return total;
    else if ( total < LEAST_VALID )
        return 0;               /* below timer resolution */
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

