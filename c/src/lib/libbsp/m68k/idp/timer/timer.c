/*  Timer_init()
 *
 *  This routine initializes the MC68230 timer on the Motorola IDP board.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: This routine will not work if the optimizer is enabled
 *        for some compilers.  The multiple writes to the MC68230
 *        may be optimized away.
 *
 *        It is important that the timer start/stop overhead be 
 *        determined when porting or modifying this code.
 *
 *  Code Modified for the MC68230 by Doug McBride, Colorado Space Grant College
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
#include <motorola/mc68230.h>

#define TIMER_VECTOR 0x4D

int Ttimer_val;
rtems_boolean Timer_driver_Find_average_overhead;

rtems_isr timerisr();

void Timer_initialize()
{
  (void) set_vector( timerisr, TIMER_VECTOR, 0 );  /* install ISR */
  
  Ttimer_val = 0;                          /* clear timer ISR count */

  /* some PI/T initialization stuff here */
  /* Set up the interrupt vector on the MC68230 chip:
     TIVR = TIMER_VECTOR; */
  MC68230_WRITE (TIVR, TIMER_VECTOR);

  /* Set CPRH through CPRL to maximum count to reduce interrupt overhead
      CPRH = 0xFF;
      CPRM = 0xFF;
      CPRL = 0xFF; */
  MC68230_WRITE (CPRH, 0xFF);
  MC68230_WRITE (CPRM, 0xFF);
  MC68230_WRITE (CPRL, 0xFF);

  /* Enable timer and use it as an external periodic interrupt generator
      TCR = 0xA1; */
  MC68230_WRITE (TCR, 0xA1);

}

#define AVG_OVERHEAD      9  /* may not be right -- do this later */
#define LEAST_VALID       10 /* Don't trust a value lower than this */

int Read_timer() 
{
  rtems_unsigned8 data;
  rtems_unsigned8  msb, osb, lsb;
  rtems_unsigned32 remaining, total;

  /* Disable timer so that timer can be read
        data = TCR;
        TCR = (data & 0xFE); */
  MC68230_READ (TCR, data);
  MC68230_WRITE (TCR, (data & 0xFE));

  /* Read the counter value
        msb = CNTRH;
        osb = CNTRM;
        lsb = CNTRL; */
  MC68230_READ (CNTRH, msb);
  MC68230_READ (CNTRM, osb);
  MC68230_READ (CNTRL, lsb);

  /* Calculate the time so far */
  remaining = 0x1000000 - ((msb << 16) + (osb << 8) + lsb);
  total = (Ttimer_val * 0x1000000) + remaining;

  /* Enable timer so that timer can continue
	 	TCR = 0xA1; */
  MC68230_WRITE (TCR, 0xA1);

  /* do not restore old vector */
  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in countdown units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */
   
  /* Clocked at 6.5 Mhz */
  /* Avoid floating point problems, be lazy, and return the total minus
     the average overhead */
  return (total - AVG_OVERHEAD);
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
