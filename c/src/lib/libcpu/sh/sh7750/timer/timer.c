/*
 *  timer driver for the Hitachi SH 7750
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  benchmark_timer_initialize() and benchmark_timer_read().  benchmark_timer_read() usually returns
 *  the number of microseconds since benchmark_timer_initialize() exitted.
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
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <rtems/score/sh_io.h>
#include <rtems/score/iosh7750.h>

extern uint32_t bsp_clicks_per_second;

#ifndef TIMER_PRIO
#define TIMER_PRIO 15
#endif

/* Timer prescaler division ratio */
#define TIMER_PRESCALER 4
#define TCR1_TPSC       SH7750_TCR_TPSC_DIV4

#define TIMER_VECTOR SH7750_EVT_TO_NUM(SH7750_EVT_TUNI1)

extern rtems_isr timerisr(void);

static uint32_t   Timer_interrupts;

/* Counter should be divided to this value to obtain time in microseconds */
static uint32_t   microseconds_divider;

/* Interrupt period in microseconds */
static uint32_t   microseconds_per_int;

bool benchmark_timer_find_average_overhead;

/* benchmark_timer_initialize --
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
benchmark_timer_initialize(void)
{
    uint8_t               temp8;
    uint16_t              temp16;
    rtems_interrupt_level level;
    rtems_isr            *ignored;
    int                   cpudiv = 1;
    int                   tidiv = 1;

    Timer_interrupts  = 0;
    rtems_interrupt_disable(level);

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

    microseconds_divider = bsp_clicks_per_second * cpudiv / (tidiv * 1000000);
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


    rtems_interrupt_enable(level);

    /* Start the Timer 1 */
    temp8 = read8(SH7750_TSTR);
    temp8 |= SH7750_TSTR_STR1;
    write8(temp8, SH7750_TSTR);

}

/*
 *  The following controls the behavior of benchmark_timer_read().
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
benchmark_timer_read(void)
{
    uint32_t              clicks;
    uint32_t              ints;
    uint32_t              total;
    rtems_interrupt_level level;
    uint32_t              tcr;


    rtems_interrupt_disable(level);

    clicks = 0xFFFFFFFF - read32(SH7750_TCNT1);
    tcr = read32(SH7750_TCR1);
    ints = Timer_interrupts;

    rtems_interrupt_enable(level);

    /* Handle the case when timer overflowed but interrupt was not processed */
    if ((clicks > 0xFF000000) && ((tcr & SH7750_TCR_UNF) != 0))
    {
        ints++;
    }

    total = microseconds_per_int * ints + (clicks / microseconds_divider);

    if ( benchmark_timer_find_average_overhead )
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

/* timerisr --
 *     Timer interrupt handler routine. This function invoked on timer
 *     underflow event; once per 2^32 clocks. It should reset the timer
 *     event and increment timer interrupts counter.
 */
void
timerisr(void)
{
  uint8_t   temp8;

  /* reset the flags of the status register */
  temp8 = read8(SH7750_TCR1) & ~SH7750_TCR_UNF;
  write8(temp8, SH7750_TCR1);

  Timer_interrupts += 1;
}
