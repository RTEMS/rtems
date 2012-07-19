/*
 *  Timer Init
 *
 *  This module initializes TIMER 2 for on the MCF5272 for benchmarks.
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  Based on work:
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <mcf5272/mcf5272.h>

#define TRR2_VAL 65530

uint32_t Timer_interrupts;

bool benchmark_timer_find_average_overhead;

/* External assembler interrupt handler routine */
extern rtems_isr timerisr(rtems_vector_number vector);


/* benchmark_timer_initialize --
 *     Initialize timer 2 for accurate time measurement.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
benchmark_timer_initialize(void)
{
    uint32_t icr;
    /* Catch timer2 interrupts */
    set_vector(timerisr, BSP_INTVEC_TMR2, 0);

    /* Reset Timer */
    g_timer_regs->tmr2 = MCF5272_TMR_RST;
    g_timer_regs->tmr2 = MCF5272_TMR_CLK_STOP;
    g_timer_regs->tmr2 = MCF5272_TMR_RST;
    g_timer_regs->tcn2 = 0;  /* reset counter */
    Timer_interrupts   = 0;  /* Clear timer ISR counter */
    g_timer_regs->ter2 = MCF5272_TER_REF | MCF5272_TER_CAP;
    g_timer_regs->trr2 = TRR2_VAL -1 ;


    /* Set Timer 2 prescaler so that it counts in microseconds */
    g_timer_regs->tmr2 = (
        (((BSP_SYSTEM_FREQUENCY / 1000000) - 1) << MCF5272_TMR_PS_SHIFT) |
        MCF5272_TMR_CE_DISABLE                                           |
        MCF5272_TMR_ORI                                                  |
        MCF5272_TMR_FRR                                                  |
        MCF5272_TMR_CLK_MSTR                                             |
        MCF5272_TMR_RST);

    /* Initialize interrupts for timer2 */
    icr = g_intctrl_regs->icr1;
    icr = icr & ~(MCF5272_ICR1_TMR2_MASK | MCF5272_ICR1_TMR2_PI);
    icr |= (MCF5272_ICR1_TMR2_IPL(BSP_INTLVL_TMR2) | MCF5272_ICR1_TMR2_PI);
    g_intctrl_regs->icr1 = icr;

}

/*
 *  The following controls the behavior of benchmark_timer_read().
 *
 *  FIND_AVG_OVERHEAD *  instructs the routine to return the "raw" count.
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD      0  /* It typically takes 2.0 microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

/* benchmark_timer_read --
 *     Read timer value in microsecond units since timer start.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     number of microseconds since timer has been started
 */
int
benchmark_timer_read( void )
{
    uint16_t clicks;
    uint32_t total;

    /*
     *  Read the timer and see how many clicks it has been since counter
     *  rolled over.
     */
    clicks = g_timer_regs->tcn2;

    /* Stop Timer... */
    g_timer_regs->tmr2 = MCF5272_TMR_CLK_STOP | MCF5272_TMR_RST;

    /*
     *  Total is calculated by taking into account the number of timer
     *  overflow interrupts since the timer was initialized and clicks
     *  since the last interrupts.
     */

    total = (Timer_interrupts * TRR2_VAL) + clicks;

    if ( benchmark_timer_find_average_overhead == 1 )
        return total;          /* in XXX microsecond units */

    if ( total < LEAST_VALID )
        return 0;            /* below timer resolution */

    /*
     *  Return the count in microseconds
     */
    return (total - AVG_OVERHEAD);
}

/* benchmark_timer_disable_subtracting_average_overhead --
 *     This routine is invoked by the "Check Timer" (tmck) test in the
 *     RTEMS Timing Test Suite. It makes the benchmark_timer_read routine not
 *     subtract the overhead required to initialize and read the benchmark
 *     timer.
 *
 * PARAMETERS:
 *     find_flag - bool flag, true if overhead must not be subtracted.
 *
 * RETURNS:
 *     none
 */
void
benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
