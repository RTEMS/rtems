/*  Timer_init()
 *
 *  This routine initializes the timer on the FORCE CPU-386 board.
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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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

  (void) set_vector( timerisr, 0x38, 0 );   /* install ISR */

  Ttimer_val = 0;                           /* clear timer ISR count */

  outport_byte( IERA, 0x40 );               /* disable interrupt */
  outport_byte( TBCR, 0x40 );               /* stop the timer */
  outport_byte( TBDR, 250 );                /* 250 units */

  outport_byte( TBCR, 0x11 );               /* reset it, delay mode, 4X */
#if 0
  outport_byte( TBCR, 0x13 );               /* reset it, delay mode, 16X */
#endif

  outport_byte( IERA, 0x41 );               /* enable interrupt */

}

#define AVG_OVERHEAD      3  /* It typically takes 3.0 microseconds */
                             /* (3 ticks) to start/stop the timer. */
#define LEAST_VALID       4  /* Don't trust a value lower than this */

int Read_timer()
{
  register rtems_unsigned32 clicks;
  register rtems_unsigned32 total;

  outport_byte( TBCR, 0x00 );        /* stop the timer */

  inport_byte( TBDR, clicks );

  total = Ttimer_val + 250 - clicks;

  outport_byte( TBCR, 0x00 );  /* initial value */
  outport_byte( IERA, 0x40 );  /* disable interrupt */

  /* ??? Is "do not restore old vector" causing problems? */

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in one microsecond units */

  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
    return (total - AVG_OVERHEAD);
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
