/*  Clock_init()
 *
 *
 *  This is modified by Doug McBride to get it to work for the MC68EC040
 *  IDP board.  The below comments are kept to show that some prior work
 *  was done in the area and the modifications performed was application
 *  specific for the IDP board to port it to.
 *
 *  This routine initializes the mc68230 on the MC68EC040 board.
 *  The tick frequency is 40 milliseconds.
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

#include <stdlib.h>

#include "rtems.h"
#include "clockdrv.h"
#include "bsp.h"
#include "cpu.h"

rtems_unsigned32 Clock_isrs;        /* ISRs until next tick */
volatile rtems_unsigned32 Clock_driver_ticks;
                                    /* ticks since initialization */
rtems_isr_entry  Old_ticker;

extern rtems_configuration_table Configuration;
extern void led_putnum();
void Disable_clock();

#define TIMER_VECTOR 0x4D

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

void ReInstall_clock( clock_isr )
rtems_isr_entry clock_isr;
{
  rtems_unsigned32 isrlevel = 0 ;

  rtems_interrupt_disable( isrlevel );
   (void) set_vector( clock_isr, TIMER_VECTOR, 1 );
  rtems_interrupt_enable( isrlevel );
}

/* The following was added for debugging purposes */
void Disable_clock()
{
	/* Disable timer */
	MC68230_WRITE (TCR, 0x00);
}

void Install_clock( clock_isr )
rtems_isr_entry clock_isr;
{
  Clock_driver_ticks = 0;
  Clock_isrs = (int)(Configuration.microseconds_per_tick / 1000);

  if ( Configuration.ticks_per_timeslice ) {
/*    led_putnum('c'); * for debugging purposes */
    Old_ticker = (rtems_isr_entry) set_vector( clock_isr, TIMER_VECTOR, 1 );

	/* Disable timer for initialization */
	MC68230_WRITE (TCR, 0x00);

	/* some PI/T initialization stuff here -- see comment in the ckisr.c
	   file in this directory to understand why I use the values that I do */
	/* Set up the interrupt vector on the MC68230 chip:
		TIVR = TIMER_VECTOR; */
	MC68230_WRITE (TIVR, TIMER_VECTOR);

	/* Set CPRH through CPRL to 193 (not 203) decimal for countdown--see ckisr.c
		CPRH = 0x00;
		CPRM = 0x00;
		CPRL = 0xC1; */
	MC68230_WRITE (CPRH, 0x00);
	MC68230_WRITE (CPRM, 0x00);
	MC68230_WRITE (CPRL, 0xC1);

	/* Enable timer and use it as an external periodic interrupt generator
		TCR = 0xA1; */
/*    led_putnum('a'); * for debugging purposes */
	MC68230_WRITE (TCR, 0xA1);

	/*
	 *  Schedule the clock cleanup routine to execute if the application exits.
	 */
    atexit( Clock_exit );
  } 
}

void Clock_exit( void )
{
  rtems_unsigned8 data;

  if ( Configuration.ticks_per_timeslice ) {

	/* disable timer
		data = TCR;
		TCR = (data & 0xFE); */
	MC68230_READ (TCR, data);
	MC68230_WRITE (TCR, (data & 0xFE));

    /* do not restore old vector */
  }
}
