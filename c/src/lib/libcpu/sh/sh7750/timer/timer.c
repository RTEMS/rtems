/*
 *  timer driver for the Hitachi SH 7750
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  Timer_initialize() and Read_timer().  Read_timer() usually returns
 *  the number of microseconds since Timer_initialize() exitted.
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>

#include <rtems/score/sh_io.h>
#include <rtems/score/iosh7750.h>

#ifndef TIMER_PRIO
#define TIMER_PRIO 15
#endif

/* Timer prescaler division ratio */
#define TIMER_PRESCALER 4
#define TCR1_TPSC       SH7750_TCR_TPSC_DIV4

#define TIMER_VECTOR SH7750_EVT_TO_NUM(SH7750_EVT_TUNI1)

rtems_isr timerisr();

static rtems_unsigned32 Timer_interrupts;

/* Counter should be divided to this value to obtain time in microseconds */
static rtems_unsigned32 microseconds_divider;

/* Interrupt period in microseconds */
static rtems_unsigned32 microseconds_per_int;

rtems_boolean Timer_driver_Find_average_overhead;

/* Timer_initialize --
 *     Initialize Timer 1 to operate as a RTEMS benchmark timer:
 *        - determine timer clock frequency
 *        - install timer interrupt handler
 *        - configure the Timer 1 hardware
 *        - start the timer
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
    rtems_unsigned8       temp8;
    rtems_unsigned16      temp16;
    rtems_interrupt_level level;
    rtems_isr            *ignored;
    int                   cpudiv = 1;
    int                   tidiv = 1;

    Timer_interrupts  = 0;
    _CPU_ISR_Disable(level);

    /* Get CPU frequency divider from clock unit */
    switch (read16(SH7750_FRQCR) & SH7750_FRQCR_IFC)
    {
        case SH7750_FRQCR_IFCDIV1:
            cpudiv = 1;
            break;
        
        case SH7750_FRQCR_IFCDIV2:
            cpudiv = 2;
            break;
           
        case SH7750_FRQCR_IFCDIV3:
            cpudiv = 3;
            break;
        
        case SH7750_FRQCR_IFCDIV4:
            cpudiv = 4;
            break;
        
        case SH7750_FRQCR_IFCDIV6:
            cpudiv = 6;
            break;
        
        case SH7750_FRQCR_IFCDIV8:
            cpudiv = 8;
            break;
        
        default:
            rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED);
    }

    /* Get peripheral module frequency divider from clock unit */
    switch (read16(SH7750_FRQCR) & SH7750_FRQCR_PFC)
    {
        case SH7750_FRQCR_PFCDIV2:
            tidiv = 2 * TIMER_PRESCALER;
            break;
           
        case SH7750_FRQCR_PFCDIV3:
            tidiv = 3 * TIMER_PRESCALER;
            break;
        
        case SH7750_FRQCR_PFCDIV4:
            tidiv = 4 * TIMER_PRESCALER;
            break;
        
        case SH7750_FRQCR_PFCDIV6:
            tidiv = 6 * TIMER_PRESCALER;
            break;
        
        case SH7750_FRQCR_PFCDIV8:
            tidiv = 8 * TIMER_PRESCALER;
            break;
        
        default:
            rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED);
    }

    microseconds_divider = 
        rtems_cpu_configuration_get_clicks_per_second() * cpudiv / 
        (tidiv * 1000000);
    microseconds_per_int = 0xFFFFFFFF / microseconds_divider;

    /*
     *  Hardware specific initialization
     */

    /* Stop the Timer 0 */
    temp8 = read8(SH7750_TSTR);
    temp8 &= ~SH7750_TSTR_STR1;
    write8(temp8, SH7750_TSTR);

    /* Establish interrupt handler */
    _CPU_ISR_install_raw_handler( TIMER_VECTOR, timerisr, &ignored );

    /* Reset timer constant and counter */
    write32(0xFFFFFFFF, SH7750_TCOR1);
    write32(0xFFFFFFFF, SH7750_TCNT1);
    
    /* Select timer mode */
    write16(
        SH7750_TCR_UNIE |        /* Enable Underflow Interrupt */
        SH7750_TCR_CKEG_RAISE |  /* Count on rising edge */
        TCR1_TPSC,               /* Timer prescaler ratio */
        SH7750_TCR1);

    /* Set timer interrupt priority */
    temp16 = read16(SH7750_IPRA);
    temp16 = (temp16 & ~SH7750_IPRA_TMU1) | (TIMER_PRIO << SH7750_IPRA_TMU1_S);
    write16(temp16, SH7750_IPRA);


    _CPU_ISR_Enable(level);

    /* Start the Timer 1 */
    temp8 = read8(SH7750_TSTR);
    temp8 |= SH7750_TSTR_STR1;
    write8(temp8, SH7750_TSTR);

}

/*
 *  The following controls the behavior of Read_timer().
 *
 *  AVG_OVERHEAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD      0  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       0 /* 20 */ /* Don't trust a clicks value lower than this */

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
Read_timer(void)
{
    rtems_unsigned32 clicks;
    rtems_unsigned32 ints;
    rtems_unsigned32 total ;
    rtems_interrupt_level level;
    rtems_unsigned32 tcr;
    

    _CPU_ISR_Disable(level);

    clicks = 0xFFFFFFFF - read32(SH7750_TCNT1);
    tcr = read32(SH7750_TCR1);
    ints = Timer_interrupts;
    
    _CPU_ISR_Enable(level);
    
    /* Handle the case when timer overflowed but interrupt was not processed */
    if ((clicks > 0xFF000000) && ((tcr & SH7750_TCR_UNF) != 0))
    {
        ints++;
    }

    total = microseconds_per_int * ints + (clicks / microseconds_divider);

    if ( Timer_driver_Find_average_overhead )
        return total;          /* in microsecond units */
    else 
    {
        if ( total < LEAST_VALID )
            return 0;            /* below timer resolution */
        /*
         *  Somehow convert total into microseconds
         */
        return (total - AVG_OVERHEAD) ;
    }
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
Empty_function( void )
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

/* timerisr --
 *     Timer interrupt handler routine. This function invoked on timer 
 *     underflow event; once per 2^32 clocks. It should reset the timer
 *     event and increment timer interrupts counter.
 */
void 
timerisr(void)
{
  unsigned8 temp8;

  /* reset the flags of the status register */
  temp8 = read8(SH7750_TCR1) & ~SH7750_TCR_UNF;
  write8(temp8, SH7750_TCR1);

  Timer_interrupts += 1;
}
