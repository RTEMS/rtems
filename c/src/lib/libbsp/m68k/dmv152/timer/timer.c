/*  timer.c
 *
 *  NOTE: These routines will not work if the optimizer is enabled
 *        for some compilers.  The multiple writes to the Z8036
 *        may be optimized away.
 *
 *        It is important that the timer start/stop overhead be
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


#include <rtems.h>
#include <bsp.h>

int Ttimer_val;
rtems_boolean Timer_driver_Find_average_overhead;

rtems_isr timerisr();

void Timer_initialize()
{
  rtems_unsigned8 data;

  (void) set_vector( timerisr, TIMER_VECTOR, 0 );  /* install ISR */

  Ttimer_val = 0;                          /* clear timer ISR count */
  Z8x36_READ ( TIMER, MASTER_INTR, data );
  Z8x36_WRITE( TIMER, MASTER_INTR, (data & 0x01) );

  Z8x36_WRITE( TIMER, MASTER_CFG, 0xd4 );
  Z8x36_READ ( TIMER, MASTER_INTR, data );
  Z8x36_WRITE( TIMER, MASTER_INTR, (data & 0x7E) );
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_MSB, 0x00 );
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_LSB, 0x00 );
  Z8x36_WRITE( TIMER, CT1_MODE_SPEC, 0x87 );
  Z8x36_WRITE( TIMER, CNT_TMR_VECTOR, TIMER_VECTOR );
  Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0x20 );
  Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0x26 );
  Z8x36_READ ( TIMER, MASTER_INTR, data );
  Z8x36_WRITE( TIMER, MASTER_INTR, (data & 0xDA) | 0x80 );

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0xC6 );

  /*
   * ACC_IC54 - interrupt 5 will be vectored and mapped to level 6
   */

  data = (*(rtems_unsigned8 *)0x0D00000B);
  (*(rtems_unsigned8 *)0x0D00000B) = (data & 0x0F) | 0x60;

}

#define AVG_OVERHEAD      9  /* It typically takes 3.65 microseconds */
                             /* (9 countdowns) to start/stop the timer. */
#define LEAST_VALID       10 /* Don't trust a value lower than this */

int Read_timer()
{
  rtems_unsigned8 data;
  rtems_unsigned8  msb, lsb;
  rtems_unsigned32 remaining, total;

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS,  0xce ); /* read the counter value */
  Z8x36_READ(  TIMER, CT1_CUR_CNT_MSB, msb );
  Z8x36_READ(  TIMER, CT1_CUR_CNT_LSB, lsb );

  remaining = 0x10000 - ((msb << 8) + lsb);
  total = (Ttimer_val * 0x10000) + remaining;

  Z8x36_READ ( TIMER, MASTER_INTR, data );
  Z8x36_WRITE( TIMER, MASTER_INTR, (data & 0x01) );

  /* do not restore old vector */
  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in countdown units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  /* Clocked at 2.4615 Mhz */

  return (int)(((float)(total-AVG_OVERHEAD)) / 2.4615 * 2.0);
}

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
