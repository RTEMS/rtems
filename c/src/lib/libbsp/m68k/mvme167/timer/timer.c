/*  timer.c
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test Suite.
 *  Each measured time period is demarcated by calls to benchmark_timer_initialize() and
 *  benchmark_timer_read().  benchmark_timer_read() usually returns the number of microseconds
 *  since benchmark_timer_initialize() exitted.
 *
 *  These functions are prototyped in rtems/c/src/lib/include/timerdrv.h and
 *  must be implemented as part of the BSP.
 *
 *  This port does not allow the application to select which timer on the
 *  MVME167 to use for the timer, nor does it allow the application to
 *  configure the timer. The timer uses the VMEchip2 Tick Timer #1. This timer
 *  is distinct from the clock, which uses Tick Timer #2 in the VMEchip2.
 *
 *  All page references are to the MVME166/MVME167/MVME187 Single Board
 *  Computer Programmer's Reference Guide (MVME187PG/D2) with the April 1993
 *  supplements/addenda (MVME187PG/D2A1).
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications of respective RTEMS file:
 *  Copyright (c) 1998, National Research Council of Canada
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp.h>

/* Periodic tick interval */
#define TICK_INTERVAL         10000UL     /* T1's countdown constant (10 ms) */
#define TIMER_INT_LEVEL       6           /* T1's interrupt level */
#define TIMER_VECTOR (VBR0 * 0x10 + 0x8)  /* T1 is vector $X8 (p. 2-71)*/

/* Number of interrupts since timer was re-initialized */
uint32_t            Ttimer_val;

/*
 *  Set to true to return raw value. Normally zero. Depends on being allocated
 *  in the .bss section and on that section being explicitly zeroed at boot
 *  time.
 */
bool benchmark_timer_find_average_overhead;

rtems_isr_entry timerisr(rtems_vector_number);

/*
 *  This routine initializes the Tick Timer 1 on the MVME167 board.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: This routine may not work if the optimizer is enabled for some
 *        compilers. The multiple writes may be optimized away.
 *
 *        It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  THE VMECHIP2 PRESCALER REGISTER IS ASSUMED TO BE SET!
 *  The prescaler is used by all VMEchip2 timers, including the VMEbus grant
 *  timeout counter, the DMAC time off timer, the DMAC timer on timer, and the
 *  VMEbus global timeout timer. The prescaler value is normally set by the
 *  boot ROM to provide a 1 MHz clock to the timers. For a 25 MHz MVME167, the
 *  prescaler value should be 0xE7 (page 2-63).
 */
void benchmark_timer_initialize(void)
{
  (void) set_vector( timerisr, TIMER_VECTOR, 0 );

  Ttimer_val = 0;                       /* clear timer ISR count */
  lcsr->intr_ena &= 0xFEFFFFFF;         /* disable tick timer 1 interrupt */
  lcsr->intr_clear |= 0x01000000;       /* clear tick timer 1 interrupt */
  lcsr->intr_level[0] =                 /* set int level */
        (lcsr->intr_level[0] & 0xFFFFFFF0) | TIMER_INT_LEVEL;
  lcsr->timer_cmp_1 = TICK_INTERVAL;    /* period in compare register */
  lcsr->timer_cnt_1 = 0;                /* clear tick timer 1 counter */
  lcsr->board_ctl |= 7;                 /* start tick timer 1, reset-on-compare, */
                                        /*   and clear overflow counter */

  lcsr->intr_ena |= 0x01000000;         /* enable tick timer 1 interrupt */
  lcsr->vector_base |= MASK_INT;        /* unmask VMEchip2 interrupts */
}

#define AVG_OVERHEAD      3UL   /* It typically takes 3.0 microseconds */
                                /* (3 countdowns) to start/stop the timer. */
#define LEAST_VALID       3UL   /* Don't trust a value lower than this */

/*
 *  This routine reads the Tick Timer 1 on the MVME167 board.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  time in microseconds
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */
uint32_t benchmark_timer_read(void)
{
  uint32_t            total;

  total = (Ttimer_val * TICK_INTERVAL) + lcsr->timer_cnt_1;

  if ( benchmark_timer_find_average_overhead )
    return total;          /* in one microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return total - AVG_OVERHEAD;
}

/*
 *  This routine sets the benchmark_timer_find_average_overhead flag in this
 *  module.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  time in microseconds
 */
void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
