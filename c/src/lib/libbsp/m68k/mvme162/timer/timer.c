/*  Timer_init()
 *
 *  This routine initializes the Tick Timer 1 on the MVME162 board.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: This routine will not work if the optimizer is enabled
 *        for some compilers.  The multiple writes
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
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

/* Periodic tick interval */
#define TICK_INTERVAL         0x10000U
#define TIMER_INT_LEVEL       6

rtems_unsigned32    Ttimer_val;
rtems_boolean       Timer_driver_Find_average_overhead;

rtems_isr timerisr();

void Timer_initialize()
{
  (void) set_vector( timerisr, VBR0 * 0x10 + 0x8, 0 );
  
  Ttimer_val = 0;                     /* clear timer ISR count */
  lcsr->vector_base |= MASK_INT;      /* unmask VMEchip2 interrupts */
  lcsr->intr_clear |= 0x01000000;     /* clear pending interrupt */
  lcsr->to_ctl = 0xE7;                /* prescaler to 1 MHz (see Appendix A1) */
  lcsr->timer_cmp_1 = TICK_INTERVAL;
  lcsr->timer_cnt_1 = 0;              /* clear counter */
  lcsr->board_ctl |= 7;               /* increment, reset-on-compare, */
                                      /*   and clear-overflow-cnt */

  lcsr->intr_level[0] |= TIMER_INT_LEVEL; /* set int level */
  lcsr->intr_ena |= 0x01000000;           /* enable tick timer 1 interrupt */
}

#define AVG_OVERHEAD      3U    /* It typically takes 3.0 microseconds */
                                /* (3 countdowns) to start/stop the timer. */
#define LEAST_VALID       10U   /* Don't trust a value lower than this */

int Read_timer() 
{
  rtems_unsigned32    total;

  total = (Ttimer_val * TICK_INTERVAL) + lcsr->timer_cnt_1;

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in one-half microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return (total-AVG_OVERHEAD) >> 1;
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
