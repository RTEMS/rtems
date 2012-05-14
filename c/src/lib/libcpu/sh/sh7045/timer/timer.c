/*
 *  timer for the Hitachi SH 704X
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  benchmark_timer_initialize() and benchmark_timer_read().  benchmark_timer_read() usually returns
 *  the number of microseconds since benchmark_timer_initialize() exitted.
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
#include <rtems/score/iosh7045.h>

extern uint32_t bsp_clicks_per_second;

/*
 *  We use a Phi/4 timer
 */
#define SCALE (Timer_MHZ/4)

#define MTU1_STARTMASK 	0xfd
#define MTU1_SYNCMASK 	0xfd
#define MTU1_MODEMASK 	0xc0
#define MTU1_TCRMASK 	0x01
#define MTU1_TIORMASK 	0x88
#define MTU1_STAT_MASK 	0xf8
#define MTU1_TIERMASK 	0xfc
#define IPRC_MTU1_MASK	0xfff0

#ifndef MTU1_PRIO
#define MTU1_PRIO 15
#endif

#define MTU1_VECTOR 86

extern rtems_isr timerisr(void);

static uint32_t   Timer_interrupts;

bool benchmark_timer_find_average_overhead;

static uint32_t   Timer_MHZ ;

void benchmark_timer_initialize( void )
{
  uint8_t                temp8;
  uint16_t               temp16;
  rtems_interrupt_level  level;
  rtems_isr	        *ignored;

  Timer_MHZ = bsp_clicks_per_second / 1000000 ;

  /*
   *  Timer has never overflowed.  This may not be necessary on some
   *  implemenations of timer but ....
   */

  Timer_interrupts /* .i */ = 0;
  rtems_interrupt_disable( level );

  /*
   *  Somehow start the timer
   */
  /* stop Timer 1  */
  temp8 = read8(MTU_TSTR) & MTU1_STARTMASK;
  write8( temp8, MTU_TSTR );

  /* initialize counter 1 */
  write16( 0, MTU_TCNT1);

  /* Timer 1 is independent of other timers */
  temp8 = read8(MTU_TSYR) & MTU1_SYNCMASK;
  write8( temp8, MTU_TSYR );

  /* Timer 1, normal mode */
  temp8 = read8(MTU_TMDR1) & MTU1_MODEMASK;
  write8( temp8, MTU_TMDR1 );

  /* x0000000
   * |||||+++--- Internal Clock
   * |||++------ Count on rising edge
   * |++-------- disable TCNT clear
   * +---------- don`t care
   */
  write8( MTU1_TCRMASK, MTU_TCR1 );

  /* gra and grb are not used */
  write8( MTU1_TIORMASK, MTU_TIOR1 );

  /* reset all status flags */
  temp8 = read8(MTU_TSR1) & MTU1_STAT_MASK;
  write8( temp8, MTU_TSR1 );

  /* enable overflow interrupt */
  write8( MTU1_TIERMASK, MTU_TIER1 );

  /* set interrupt priority */
  temp16 = read16(INTC_IPRC) & IPRC_MTU1_MASK;
  temp16 |= MTU1_PRIO;
  write16( temp16, INTC_IPRC);

  /* initialize ISR */
  _CPU_ISR_install_raw_handler( MTU1_VECTOR, timerisr, &ignored );
  rtems_interrupt_enable( level );

  /* start timer 1 */
  temp8 = read8(MTU_TSTR) | ~MTU1_STARTMASK;
  write8( temp8, MTU_TSTR );
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

#define AVG_OVERHEAD      1  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       0 /* 20 */ /* Don't trust a clicks value lower than this */

int benchmark_timer_read( void )
{
  uint32_t   clicks;
  uint32_t   total ;
  /*
   *  Read the timer and see how many clicks it has been since we started.
   */


  clicks = read16( MTU_TCNT1 );   /* XXX: read some HW here */

  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */

  total = clicks + Timer_interrupts * 65536;

  if ( benchmark_timer_find_average_overhead )
    return total / SCALE;          /* in XXX microsecond units */
  else
  {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
  /*
   *  Somehow convert total into microseconds
   */
    return (total / SCALE - AVG_OVERHEAD) ;
  }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}

/* Timer 1 is used */

#pragma interrupt
void timerisr( void )
{
  uint8_t   temp8;

  /* reset the flags of the status register */
  temp8 = read8(MTU_TSR1) & MTU1_STAT_MASK;
  write8( temp8, MTU_TSR1 );

  Timer_interrupts += 1;
}
