/*  Timer_init()
 *
 *  This routine initializes a timer in efi68k's DP8570A TCP
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: It is important that the timer start/stop overhead be 
 *        determined when porting or modifying this code.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#include <bsp.h>

rtems_boolean Timer_driver_Find_average_overhead;

extern rtems_isr Clock_isr();

void Timer_initialize( void )
{
  /* stop counter */
  *MSR = 0;
  *T0CR = 0;

  /*
   * Make sure isr is installed
   */

  set_vector( Clock_isr, TCP_ISR_LEVEL+24, 1);

  /* clear timer ISR count */
  Timer_interrupts = 0;

  /* load count (count down timer) */
  *MSR = 0;
  *T0_MSB = 0xff;
  *T0_LSB = 0xff;

  /* clear old interrupts */
  *MSR = T0;

  /* enable timer 0 interrupt */
  *MSR = RS;
  *ICR0 |= T0E;

  /* 
     TSS = 1        starts the timer (timer resets on start)
     M1/0 = 0/1     rate generator
     C2/1/0 = 0/0/0 external clock (8MHz) (1/8 usec resolution)
     RD = 0         read data (latchs count)
     CHG = 0        hold
   */
  *MSR = 0;
  *T0CR = (TSS | M0);
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

#define AVG_OVERHEAD      0  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

/*
 * Return timer value in 1/2-microsecond units
 */
int Read_timer( void )
{
  rtems_unsigned16 clicks;
  rtems_unsigned32 total;
  rtems_unsigned32 msb, lsb;


  /*
   *  Read the timer and see how many clicks it has been since counter
   *  rolled over.
   */

  *MSR = 0;
  *T0CR |= RD;
  /* must read MSB first */  
  msb = *T0_MSB;
  lsb = *T0_LSB;
  clicks = 0xffff - ((msb << 8) | lsb);

  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */

  total = (Timer_interrupts * 0x10000 + clicks + 4)/8; /* in micoseconds */
  /*                                            ^^^ round to nearest int */

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return (total - AVG_OVERHEAD);
}


/*
 *  Empty function call used in loops to measure basic cost of looping
 *  in Timing Test Suite.
 */

rtems_status_code Empty_function(void)
{
    return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}
