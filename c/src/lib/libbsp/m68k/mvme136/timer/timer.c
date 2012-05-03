/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/btimer.h>
#include <bsp.h>
#include <rtems/zilog/z8036.h>

#define TIMER 0xfffb0000   /* address of Z8036 on MVME136 */

int Ttimer_val;
bool benchmark_timer_find_average_overhead;

rtems_isr timerisr(void);

void benchmark_timer_initialize(void)
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

  *((uint16_t*)0xfffb0016) = 0x0000; /* write countdown value */
/*
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_MSB, 0x00 );
  Z8x36_WRITE( TIMER, CT1_TIME_CONST_LSB, 0x00 );
*/
  Z8x36_WRITE( TIMER, MASTER_CFG,         0xc4 ); /* enable timer1        */

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS,     0xc6 ); /* set INTR enable (IE), */
                                                 /*  trigger command      */
                                                 /*  (TCB) and gate       */
                                                 /* command (GCB) bits    */
  *((uint8_t*)0xfffb0038) &= 0xfd;      /* enable timer INTR on  */
                                                 /*    VME controller     */
}

#define AVG_OVERHEAD      6  /* It typically takes 3.0 microseconds */
                             /* (6 countdowns) to start/stop the timer. */
#define LEAST_VALID       10 /* Don't trust a value lower than this */

uint32_t benchmark_timer_read(void)
{
/*
  uint8_t          msb, lsb;
*/
  uint32_t         remaining, total;

  Z8x36_WRITE( TIMER, CT1_CMD_STATUS,  0xce ); /* read the counter value */
remaining = 0xffff - *((uint16_t*) 0xfffb0010);
/*
  Z8x36_READ(  TIMER, CT1_CUR_CNT_MSB, msb );
  Z8x36_READ(  TIMER, CT1_CUR_CNT_LSB, lsb );

  remaining = 0xffff - ((msb << 8) + lsb);
*/
  total = (Ttimer_val * 0x10000) + remaining;

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in one-half microsecond units */

  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
    return (total-AVG_OVERHEAD) >> 1;
  }
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
