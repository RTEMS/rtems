/* 
 * This routine initializes the MC68360 Periodic Interval Timer
 *
 * The PIT has rather poor resolution, but it is easy to set up
 * and requires no housekeeping once it is going.
 *
 * Based on the `gen68302' board support package, and covered by the
 * original distribution terms.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

/*
 * Input parameters:	NONE
 *
 * Output parameters:	NONE
 *
 * COPYRIGHT (c) 1989-1999.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.OARcorp.com/rtems/license.html.
 */

#include <stdlib.h>			/* for atexit() */
#include <bsp.h>
#include <rtems/libio.h>
#include "m68360.h"

#define CLOCK_VECTOR	120
#define CLOCK_IRQ_LEVEL	6

/*
 * Clock_driver_ticks is a monotonically increasing counter of the
 * number of clock ticks since the driver was initialized.
 */
volatile rtems_unsigned32 Clock_driver_ticks;

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

char M360DefaultWatchdogFeeder = 1;

/*
 * Periodic interval timer interrupt handler
 */

rtems_isr
Clock_isr (rtems_vector_number vector)
{
	/*
	 * Perform a dummy read of DPRAM.
	 * This works around a bug in Rev. B of the 68360
	 */
	m360.dpram0[0];

	/*
	 * Feed the watchdog
	 * Application code can override this by
	 * setting M360DefaultWatchdogFeeder to zero.
	 */
	if (M360DefaultWatchdogFeeder) {
		m360.swsr = 0x55;
		m360.swsr = 0xAA;
	}

	/*
	 * Announce the clock tick
	 */
	Clock_driver_ticks++;
	rtems_clock_tick();
}

void
Clock_exit (void)
{
	if (BSP_Configuration.ticks_per_timeslice ) {
		/*
		 * Turn off periodic interval timer
		 */
		m360.pitr &= ~0xFF;
	}
}

static void
Install_clock (rtems_isr_entry clock_isr)
{
	Clock_driver_ticks = 0;
	if ( BSP_Configuration.ticks_per_timeslice ) {
		int pitr;

		/*
		 * Choose periodic interval timer register value
		 * For a 25 MHz external clock the basic clock rate is
		 *	40 nsec * 128 * 4 = 20.48 usec/tick
		 */
		pitr = ((BSP_Configuration.microseconds_per_tick * 100) + 1023) / 2048;
		if (pitr >= 256) {
			pitr = (pitr + 255) / 512;
			if (pitr >= 256)
				pitr = 255;
			else if (pitr == 0)
				pitr = 1;
			pitr |= 0x100;
		}
		else if (pitr == 0) {
			pitr = 1;
		}
		m360.pitr &= ~0x1FF;
		m360.picr = (CLOCK_IRQ_LEVEL << 8) | CLOCK_VECTOR;
		set_vector (clock_isr, CLOCK_VECTOR, 1);
		m360.pitr |= pitr;
		atexit (Clock_exit);
	}
}

rtems_device_driver
Clock_initialize(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void *pargp
)
{
	Install_clock (Clock_isr);
 
	/*
	 * make major/minor avail to others such as shared memory driver
	 */
	rtems_clock_major = major;
	rtems_clock_minor = minor;
 
	return RTEMS_SUCCESSFUL;
}
 
rtems_device_driver Clock_control(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void *pargp
)
{
	rtems_unsigned32 isrlevel;
	rtems_libio_ioctl_args_t *args = pargp;

	if (args) {
		/*
		 * This is hokey, but until we get a defined interface
		 * to do this, it will just be this simple...
		 */
		if (args->command == rtems_build_name('I', 'S', 'R', ' ')) {
			Clock_isr( CLOCK_VECTOR);
		}
		else if (args->command == rtems_build_name('N', 'E', 'W', ' ')) {
			rtems_interrupt_disable( isrlevel );
			 (void) set_vector( args->buffer, CLOCK_VECTOR, 1 );
			rtems_interrupt_enable( isrlevel );
		}
	}
	return RTEMS_SUCCESSFUL;
}
