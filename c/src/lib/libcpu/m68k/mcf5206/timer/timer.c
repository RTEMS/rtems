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
 *
 *  http://www.OARcorp.com/rtems/license.html.
 * 
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include "mcf5206/mcf5206e.h"

#define TRR2_VAL 65530  

rtems_unsigned32 Timer_interrupts;

rtems_boolean Timer_driver_Find_average_overhead;

/* External assembler interrupt handler routine */
extern rtems_isr timerisr(rtems_vector_number vector);


/* Timer_initialize --
 *     Initialize timer 2 for accurate time measurement.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
Timer_initialize(void)
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
 *  The following controls the behavior of Read_timer().
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

/* Read_timer --
 *     Read timer value in microsecond units since timer start.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     number of microseconds since timer has been started
 */
int
Read_timer( void )
{
    rtems_unsigned16 clicks;
    rtems_unsigned32 total;

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

    if ( Timer_driver_Find_average_overhead == 1 )
        return total;          /* in XXX microsecond units */

    if ( total < LEAST_VALID )
        return 0;            /* below timer resolution */

    /*
     *  Return the count in microseconds
     */
    return (total - AVG_OVERHEAD);
}


/* Empty_function --
 *     Empty function call used in loops to measure basic cost of looping
 *     in Timing Test Suite.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
rtems_status_code
Empty_function(void)
{
    return RTEMS_SUCCESSFUL;
}

/* Set_find_average_overhead --
 *     This routine is invoked by the "Check Timer" (tmck) test in the
 *     RTEMS Timing Test Suite. It makes the Read_timer routine not
 *     subtract the overhead required to initialize and read the benchmark
 *     timer.
 *
 * PARAMETERS:
 *     find_flag - boolean flag, TRUE if overhead must not be subtracted.
 *
 * RETURNS:
 *     none
 */
void
Set_find_average_overhead(rtems_boolean find_flag)
{
  Timer_driver_Find_average_overhead = find_flag;
}
