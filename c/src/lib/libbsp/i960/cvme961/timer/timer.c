/*  Timer_init()
 *
 *  This routine initializes the Z8536 timer on the SQSIO4 SQUALL
 *  board for the CVME961 board.  The timer is setup to provide a
 *  tick every 1 millisecond.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: This routine will not work if the optimizer is enabled
 *        for most compilers.  The multiple writes to the Z8536
 *        will be optimized away.
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
#include <zilog/z8536.h>

#define TIMER       0xc00000a0

int Ttimer_val;
rtems_boolean Timer_driver_Find_average_overhead;

void flush_reg();
rtems_isr timerisr();

void Timer_initialize()
{
  set_vector( timerisr, 4, 0 );               /* install ISR */

  i960_mask_intr( 5 );                        /* disable VIC068 tick */
  flush_reg();                                /* timed code starts clean */
  Ttimer_val = 0;                             /* clear timer ISR count */
  Z8x36_WRITE( TIMER, MASTER_INTR,    0x01 ); /* reset              */
  Z8x36_WRITE( TIMER, MASTER_INTR,    0x00 ); /* clear reset        */
  Z8x36_WRITE( TIMER, MASTER_CFG,     0x00 ); /* disable everything */
  Z8x36_WRITE( TIMER, CNT_TMR_VECTOR, 0x72 ); /* clear intr vector  */
  Z8x36_WRITE( TIMER, MASTER_CFG,     0x20 ); /* clear intr info    */
  Z8x36_WRITE( TIMER, MASTER_CFG,     0xe0 ); /* disable interrupts */
  Z8x36_WRITE( TIMER, MASTER_CFG,     0x20 ); /* clear intr info    */
  Z8x36_WRITE( TIMER, MASTER_CFG,     0xe0 ); /* disable interrupts */
  Z8x36_WRITE( TIMER, MASTER_INTR,    0xe2 ); /* disable lower chain,   */
                                              /*   no vector, set right */
                                              /*   justified addr and   */
                                              /*   master int enable    */
  Z8x36_WRITE( TIMER, CT1_MODE_SPEC,      0x80 ); /* T1 continuous, and   */
                                                  /*   cycle/pulse output */
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_MSB, 0x00 );
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_LSB, 0x00 );
  Z8x36_WRITE( TIMER, CT1_CMD_STATUS,     0xc0 ); /* set INTR enable (IE) */
  Z8x36_WRITE( TIMER, MASTER_CFG,         0x40 ); /* enable timer1        */
  Z8x36_WRITE( TIMER, CT1_CMD_STATUS,     0x06 ); /* set trigger command  */
                                                  /*   (TCB) and gate     */
                                                  /* command (GCB) bits   */
}

#define AVG_OVERHEAD      11 /* It typically takes 5.5 microseconds */
                             /* (11 countdowns) to start/stop the timer. */
#define LEAST_VALID       15 /* Don't trust a value lower than this */

int Read_timer()
{
  rtems_unsigned8  msb, lsb;
  rtems_unsigned32 remaining, total;

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS,  0xce );  /* read the counter value */
  Z8x36_READ(  TIMER, CT1_CUR_CNT_MSB, msb );
  Z8x36_READ(  TIMER, CT1_CUR_CNT_LSB, lsb );

  remaining = 0xffff - ((msb << 8) + lsb);
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
