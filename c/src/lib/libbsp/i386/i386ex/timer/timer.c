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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


#include <rtems.h>
#include <bsp.h>

int Ttimer_val;
rtems_boolean Timer_driver_Find_average_overhead;

rtems_isr timerisr();

#define TMR0	  0xF040
#define TMR1	  0xF041
#define TMR2	  0xF042
#define TMRCON	  0xF043
#define TMRCFG    0xF834

void Timer_initialize()
{

  (void) set_vector( timerisr, 0x2a, 0 );   /* install ISR ( IR2 ) was 0x38*/

  Ttimer_val = 0;                           /* clear timer ISR count */

  outport_byte  ( TMRCON , 0xb0 ); /* select tmr2, stay in mode 0 */
  outport_byte  ( TMR1   , 0xd2 ); /* set to 250 usec interval */
  outport_byte	( TMR1   , 0x00 );
  outport_byte  ( TMRCON , 0x64 ); /* change to mode 2 ( starts timer ) */ 
                                   /* interrupts ARE enabled */
/*  outport_byte( IERA, 0x41 );             enable interrupt */

}

#define AVG_OVERHEAD      3  /* It typically takes 3.0 microseconds */
                             /* (3 ticks) to start/stop the timer. */
#define LEAST_VALID       4  /* Don't trust a value lower than this */

int Read_timer()
{
  register rtems_unsigned32 clicks;
  register rtems_unsigned32 total;

/*  outport_byte( TBCR, 0x00 );  stop the timer -- not needed on intel */

  outport_byte ( TMRCON, 0x40 );   /* latch the count */
  inport_byte  ( TMR1,   clicks ); /* read the count */ 

  total = Ttimer_val + 250 - clicks;

/*  outport_byte( TBCR, 0x00 );   initial value */
/*  outport_byte( IERA, 0x40 );   disable interrupt */

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
