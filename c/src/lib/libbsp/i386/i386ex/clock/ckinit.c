/*  Clock_initialize
 *
 *  This routine initializes the Timer/Counter on the Intel
 *  386ex evaluation board.
 *
 *  The tick frequency is 1 millisecond.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <bsp.h>
#include <clockdrv.h>
#include <stdlib.h>

volatile rtems_unsigned32 Clock_driver_ticks;
rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */
rtems_isr_entry  Old_ticker;

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp,
  rtems_id tid,
  rtems_unsigned32 *rval
)
{
  Install_clock( Clock_isr );
}

void ReInstall_clock(
  rtems_isr_entry clock_isr
)
{
  rtems_unsigned32 isrlevel = 0;

  rtems_interrupt_disable( isrlevel );
   (void) set_vector( clock_isr, 0x20, 1 ); /* was 0x38 */
  rtems_interrupt_enable( isrlevel );
}

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    Old_ticker = ( rtems_isr_entry ) set_vector( clock_isr, 0x20, 1 ); 
                                                     /* was 0x38 */

/*  The following is already set up in interns.s -> 
    ( This is test code only... production code will move the 
      TMRCFG stuff here )
*/
#define TMR0	  0xF040
#define TMR1	  0xF041
#define TMR2	  0xF042
#define TMRCON	  0xF043
#define TMRCFG    0xF834
	
	outport_byte	( TMRCFG , 0x80 );

	outport_byte    ( TMRCON , 0x34 ); 
	outport_byte	( TMR0   , 0xA8 ); 
	outport_byte    ( TMR0   , 0x04 ); 

	outport_byte    ( TMRCFG , 0x00 ); 
  }
  atexit( Clock_exit );
}

void Clock_exit( void )
{
  if ( BSP_Configuration.ticks_per_timeslice ) {
/*     outport_byte( TBCR, 0x00 ); */ /* initial value */
/*    outport_byte( IERA, 0x40 ); */ /* disable interrupt */
/* ??? Is "do not restore old vector" causing problems? */
  }
}

