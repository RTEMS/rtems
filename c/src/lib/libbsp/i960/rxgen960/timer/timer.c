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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


#include <bsp.h>
#include <stdlib.h>
#include <rtems/libio.h>


#define TIMER_VECTOR 34

int Ttimer_val;
rtems_boolean Timer_driver_Find_average_overhead = 0;

void flush_reg();
rtems_isr timerisr();

void Timer_initialize()
{
  volatile unsigned int *tmr1 = (unsigned int *) TMR1_ADDR;
  volatile unsigned int *trr1 = (unsigned int *) TRR1_ADDR;
  volatile unsigned int *tcr1 = (unsigned int *) TCR1_ADDR;
  volatile unsigned int *imsk = (unsigned int *) IMSK_ADDR;
  volatile unsigned int *icon = (unsigned int *) ICON_ADDR;
  volatile unsigned int *ipnd = (unsigned int *) IPND_ADDR;
  volatile unsigned int *imap2 = (unsigned int *) IMAP2_ADDR;

    #define BUS_CLOCK_1 0
    #define TMR_WRITE_CNTL 8
    #define TMR_AUTO_RELOAD 4
    #define TMR_ENABLE 2
    #define TMR_TERM_CNT_STAT 1
  
    *tmr1 = BUS_CLOCK_1 | TMR_AUTO_RELOAD; 
     *icon = 0x6000; 


    set_vector( (((unsigned int) timerisr) | 0x2), TIMER_VECTOR, 1 );

    *imap2 = (*imap2 & 0xff0fffff) | (((TIMER_VECTOR >> 4) & 0xf) << 20);

    /* initialize the i960RP timer 1 here */
    
    /* set the timer countdown */
    *trr1 = 33 * BSP_Configuration.microseconds_per_tick;
    *tcr1 = 33 * BSP_Configuration.microseconds_per_tick;
  
    *ipnd &= ~(1<<13);
    *imsk |= (1 << 13); 
    Ttimer_val = 0;
    *tmr1 = BUS_CLOCK_1 | TMR_AUTO_RELOAD | TMR_ENABLE;

}



rtems_isr timerisr(
  rtems_vector_number vector
)
{
  /* enable_tracing(); */
  Ttimer_val++;
  i960_clear_intr( 13 );
}

#define AVG_OVERHEAD      4 /* It typically takes 5.5 microseconds */
                             /* (11 countdowns) to start/stop the timer. */
#define LEAST_VALID       5 /* Don't trust a value lower than this */

int Read_timer()
{
  volatile unsigned int *tcr1 = (unsigned int *) TCR1_ADDR;
  volatile unsigned int *trr1 = (unsigned int *) TRR1_ADDR;
  rtems_unsigned32 remaining, total;

  /* this routine is supposed to count in 1/2 uSec units */
  /* pretty funny when using a 33MHz clock for the counter */
  remaining =  *tcr1;
  remaining =  *trr1 - remaining;
  total = (2 * ((Ttimer_val * *trr1) + remaining)) / 33;
/*
  putnum(remaining);
  console_sps_putc(':');
  putnum(total);
*/

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
