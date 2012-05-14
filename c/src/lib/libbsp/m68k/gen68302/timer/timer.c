/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include <rtems/m68k/m68302.h>

#define TMR2_VAL 0x071b	/* Timer mode register
			 * (section 3.5.2.1 in 68302 manual)
			 * 15-8: "7"	prescaler divide by 8 (x+1)
			 *  7-6: 00	dis. intr. on capture event
			 *    5:  0	active-low pulse
			 *    4:  1	intr. on reaching reference
			 *    3:  1	restart counter on reference
			 *  2-1: 01	master clock input source
			 *    0:  1	enable timer
			 */
#define TRR2_VAL 2000	/* Timer reference register
			 * (section 3.5.2.2 in 68302 manual)
			 * 2000 ticks @ (16MHz/1)/8 = 1-ms count
			 */

uint32_t         Timer_interrupts;

bool benchmark_timer_find_average_overhead;

rtems_isr timerisr(void);

void benchmark_timer_initialize( void )
{
    m302.reg.tmr2 = 0;			/* disable timer */

    Timer_interrupts = 0;		/* clear timer ISR count */

    m302.reg.trr2 = TRR2_VAL;		/* set timer reference register */
    m302.reg.tmr2 = TMR2_VAL;		/* set timer mode register */
    m302.reg.imr |= RBIT_IMR_TIMER2;	/* set 68302 int-mask to allow ints */
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

#define AVG_OVERHEAD      0  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

/*
 * Return timer value in 1/2-microsecond units
 */
uint32_t benchmark_timer_read( void )
{
  uint16_t         clicks;
  uint32_t         total;

  /*
   *  Read the timer and see how many clicks it has been since counter
   *  rolled over.
   */

  clicks = m302.reg.tcn2;

  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */

  total = (Timer_interrupts * TRR2_VAL) + clicks;

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in XXX microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  /*
   *  Convert 1/2-microsecond count into microseconds
   */

  return (total - AVG_OVERHEAD) >> 1;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
