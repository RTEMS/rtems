/*  Timer_init()
 *
 *  This routine initializes the Z8036 timer on the MVME136 board.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: This routine will not work if the optimizer is enabled
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


#include <bsp.h>
#include <zilog/z8036.h>

#define TIMER 0xfffb0000   /* address of Z8036 on MVME136 */

int Ttimer_val;
rtems_boolean Timer_driver_Find_average_overhead;

rtems_isr timerisr();

void Timer_initialize()
{
  (void) set_vector( timerisr, 66, 0 );       /* install ISR */

  Ttimer_val = 0;                             /* clear timer ISR count */
  Z8x36_WRITE( TIMER, MASTER_INTR, 0x01 ); /* reset */
  Z8x36_WRITE( TIMER, MASTER_INTR, 0x00 ); /* clear reset */
  Z8x36_WRITE( TIMER, MASTER_INTR, 0xe2 ); /* disable lower chain, no vec */
                                           /*  set right justified addr */
                                              /*  and master int enable */
  Z8x36_WRITE( TIMER, CT1_MODE_SPEC,  0x80 ); /* T1 continuous, and   */
                                              /* cycle/pulse output   */

  *((rtems_unsigned16 *)0xfffb0016) = 0x0000; /* write countdown value */
/*
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_MSB, 0x00 );
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_LSB, 0x00 );
*/
  Z8x36_WRITE( TIMER, MASTER_CFG,         0xc4 ); /* enable timer1        */

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS,     0xc6 ); /* set INTR enable (IE), */
                                                 /*  trigger command      */
                                                 /*  (TCB) and gate       */
                                                 /* command (GCB) bits    */
  *((rtems_unsigned8 *)0xfffb0038) &= 0xfd;      /* enable timer INTR on  */
                                                 /*    VME controller     */
}

#define AVG_OVERHEAD      6  /* It typically takes 3.0 microseconds */
                             /* (6 countdowns) to start/stop the timer. */
#define LEAST_VALID       10 /* Don't trust a value lower than this */

int Read_timer()
{
/*
  rtems_unsigned8  msb, lsb;
*/
  rtems_unsigned32 remaining, total;

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS,  0xce ); /* read the counter value */
remaining = 0xffff - *((rtems_unsigned16 *) 0xfffb0010);
/*
  Z8x36_READ(  TIMER, CT1_CUR_CNT_MSB, msb );
  Z8x36_READ(  TIMER, CT1_CUR_CNT_LSB, lsb );

  remaining = 0xffff - ((msb << 8) + lsb);
*/
  total = (Ttimer_val * 0x10000) + remaining;

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in one-half microsecond units */

  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
    return (total-AVG_OVERHEAD) >> 1;
  }
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
