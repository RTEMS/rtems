/*
 *  timer for the Hitachi SH 703X 
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  Timer_initialize() and Read_timer().  Read_timer() usually returns
 *  the number of microseconds since Timer_initialize() exitted.
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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>

#include <rtems/score/sh_io.h>
#include <rtems/score/ispsh7032.h>
#include <rtems/score/iosh7032.h>

#define I_CLK_PHI_1     0
#define I_CLK_PHI_2     1
#define I_CLK_PHI_4     2
#define I_CLK_PHI_8     3

/*
 * Set I_CLK_PHI to one of the I_CLK_PHI_X values from above to choose
 * a PHI/X clock rate.  
 */
   
#define I_CLK_PHI       I_CLK_PHI_4
#define CLOCK_SCALE     (1<<I_CLK_PHI)

#define ITU1_STARTMASK 	0xfd
#define ITU1_SYNCMASK 	0xfd
#define ITU1_MODEMASK 	0xfd
#define ITU1_TCRMASK 	(0x00 | I_CLK_PHI)
#define ITU1_TIORMASK 	0x88
#define ITU1_STAT_MASK 	0xf8
#define ITU1_TIERMASK 	0xfc
#define IPRC_ITU1_MASK	0xfff0

#ifndef ITU1_PRIO
#define ITU1_PRIO 15
#endif

#define ITU1_VECTOR OVI1_ISP_V

rtems_isr timerisr();

static rtems_unsigned32 Timer_interrupts;

rtems_boolean Timer_driver_Find_average_overhead;

static rtems_unsigned32 Timer_HZ ;

void Timer_initialize( void )
{
  rtems_unsigned8  temp8;
  rtems_unsigned16 temp16;
  rtems_unsigned32 level;
  rtems_isr	   *ignored;

  Timer_HZ = rtems_cpu_configuration_get_clicks_per_second() / CLOCK_SCALE ;

  /*
   *  Timer has never overflowed.  This may not be necessary on some
   *  implemenations of timer but ....
   */

  Timer_interrupts /* .i */ = 0;
  _CPU_ISR_Disable( level);

  /*
   *  Somehow start the timer
   */
  /* stop Timer 1  */
  temp8 = read8( ITU_TSTR) & ITU1_STARTMASK;
  write8( temp8, ITU_TSTR);

  /* initialize counter 1 */
  write16( 0, ITU_TCNT1);

  /* Timer 1 is independent of other timers */
  temp8 = read8( ITU_TSNC) & ITU1_SYNCMASK;
  write8( temp8, ITU_TSNC);

  /* Timer 1, normal mode */
  temp8 = read8( ITU_TMDR) & ITU1_MODEMASK;
  write8( temp8, ITU_TMDR);

  /* Use a Phi/X counter */
  write8( ITU1_TCRMASK, ITU_TCR1);

  /* gra and grb are not used */
  write8( ITU1_TIORMASK, ITU_TIOR1);

  /* reset all status flags */
  temp8 = read8( ITU_TSR1) & ITU1_STAT_MASK;
  write8( temp8, ITU_TSR1);

  /* enable overflow interrupt */
  write8( ITU1_TIERMASK, ITU_TIER1);

  /* set interrupt priority */
  temp16 = read16( INTC_IPRC) & IPRC_ITU1_MASK;
  temp16 |= ITU1_PRIO;
  write16( temp16, INTC_IPRC);

  /* initialize ISR */
  _CPU_ISR_install_raw_handler( ITU1_VECTOR, timerisr, &ignored );
  _CPU_ISR_Enable( level);

  /* start timer 1 */
  temp8 = read8( ITU_TSTR) | ~ITU1_STARTMASK;
  write8( temp8, ITU_TSTR);
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

#define AVG_OVERHEAD      1  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       0 /* 20 */ /* Don't trust a clicks value lower than this */

int Read_timer( void )
{
  rtems_unsigned32 cclicks;
  rtems_unsigned32 total ;
  /*
   *  Read the timer and see how many clicks it has been since we started.
   */
  

  cclicks = read16( ITU_TCNT1);   /* XXX: read some HW here */
  
  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */

  total = cclicks + Timer_interrupts * 65536 ;

  if ( Timer_driver_Find_average_overhead )
    return total / CLOCK_SCALE;          /* in XXX microsecond units */
  else 
  {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
  /*
   *  Somehow convert total into microseconds
   */
    return (total / CLOCK_SCALE - AVG_OVERHEAD) ;
  }
}

/*
 *  Empty function call used in loops to measure basic cost of looping
 *  in Timing Test Suite.
 */

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

/* Timer 1 is used */

#pragma interrupt
void timerisr( void )
{
  unsigned8 temp8;

  /* reset the flags of the status register */
  temp8 = read8( ITU_TSR1) & ITU1_STAT_MASK;
  write8( temp8, ITU_TSR1);

  Timer_interrupts += 1;
}
