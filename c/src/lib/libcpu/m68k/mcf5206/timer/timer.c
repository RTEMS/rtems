/*
 *  Timer Init
 *
 *  This module initializes TIMER 2 for on the MCF5206E for benchmarks.
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
#include "mcf5206/mcf5206e.h"

#define TRR2_VAL 65530

uint32_t   Timer_interrupts;

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
    /* Catch timer2 interrupts */
    set_vector(timerisr, BSP_INTVEC_TIMER2, 0);

    /* Initialize interrupts for timer2 */
    *MCF5206E_ICR(MBAR, MCF5206E_INTR_TIMER_2) =
        MCF5206E_ICR_AVEC |
        ((BSP_INTLVL_TIMER2 << MCF5206E_ICR_IL_S) & MCF5206E_ICR_IL) |
        ((BSP_INTPRIO_TIMER2 << MCF5206E_ICR_IP_S) & MCF5206E_ICR_IP);

    /* Enable interrupts from timer2 */
    *MCF5206E_IMR(MBAR) &= ~MCF5206E_INTR_BIT(MCF5206E_INTR_TIMER_2);

    /* Reset Timer */
    *MCF5206E_TMR(MBAR, 2) = MCF5206E_TMR_RST;
    *MCF5206E_TMR(MBAR, 2) = MCF5206E_TMR_ICLK_STOP;
    *MCF5206E_TMR(MBAR, 2) = MCF5206E_TMR_RST;
    *MCF5206E_TCN(MBAR, 2) = 0; /* Zero timer counter */
    Timer_interrupts = 0; /* Clear timer ISR counter */
    *MCF5206E_TER(MBAR, 2) = MCF5206E_TER_REF | MCF5206E_TER_CAP; /*clr pend*/
    *MCF5206E_TRR(MBAR, 2) = TRR2_VAL - 1;
    *MCF5206E_TMR(MBAR, 2) =
        (((BSP_SYSTEM_FREQUENCY / 1000000) << MCF5206E_TMR_PS_S) &
          MCF5206E_TMR_PS) |
        MCF5206E_TMR_CE_NONE | MCF5206E_TMR_ORI | MCF5206E_TMR_FRR |
        MCF5206E_TMR_RST;
    *MCF5206E_TMR(MBAR, 2) |= MCF5206E_TMR_ICLK_MSCLK;
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
    uint16_t   clicks;
    uint32_t   total;

    /*
     *  Read the timer and see how many clicks it has been since counter
     *  rolled over.
     */
    clicks = *MCF5206E_TCN(MBAR, 2);

    /* Stop Timer... */
    *MCF5206E_TMR(MBAR, 2) = MCF5206E_TMR_ICLK_STOP |
                             MCF5206E_TMR_RST;

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
 *     find_flag - boolean flag, true if overhead must not be subtracted.
 *
 * RETURNS:
 *     none
 */
void
benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
