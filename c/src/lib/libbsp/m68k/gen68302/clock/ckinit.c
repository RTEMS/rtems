/*  Clock_init()
 *
 *  This routine initializes Timer 1 for an MC68302.
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

#include <stdlib.h>			/* for atexit() */

#include <rtems.h>
#include <bsp.h>
#include <clockdrv.h>
#include "m68302.h"


#define TMR1_VAL (  RBIT_TMR_RST	/* software reset the timer */\
		  | RBIT_TMR_ICLK_MASTER16 /* master clock divided by 16 */\
		  | RBIT_TMR_FRR	/* restart timer after ref reached */\
		  | RBIT_TMR_ORI)	/* enable interrupt when ref reached */
#define TRR1_VAL 1000			/* 1000 ticks @ 16MHz/16
					 *    = 1 millisecond tick.
					 */

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */
volatile rtems_unsigned32 Clock_driver_ticks;

/*
 *  Clock_isrs is the number of clock ISRs until the next invocation of
 *  the RTEMS clock tick routine.  The clock tick device driver
 *  gets an interrupt once a millisecond and counts down until the
 *  length of time between the user configured microseconds per tick
 *  has passed.
 */
rtems_unsigned32 Clock_isrs;


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


void Install_clock(
  rtems_isr_entry clock_isr
)
{

  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  if ( BSP_Configuration.ticks_per_timeslice ) {
/*  set_vector( clock_isr, 137, 1 );*/

    m302.reg.trr1 = TRR1_VAL;		/* set timer reference register */
    m302.reg.tmr1 = TMR1_VAL;		/* set timer mode register & enable */
    /*
     * Enable TIMER1 interrupts only.
     */
    m302.reg.imr  = RBIT_IMR_TIMER1;	/* set 68302 int-mask to allow ints */

    atexit( Clock_exit );
  }
}


void Clock_exit( void )
{
  if ( BSP_Configuration.ticks_per_timeslice ) {
    /* TODO: figure out what to do here */
    /* do not restore old vector */
  }
}
