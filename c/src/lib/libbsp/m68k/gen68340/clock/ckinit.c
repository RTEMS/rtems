/* 
 * This routine initializes the MC68340/349 Periodic Interval Timer
 *
 * Based on the `gen68360' board support package, and covered by the
 * original distribution terms.
 *
 * Geoffroy Montel
 * France Telecom - CNET/DSM/TAM/CAT
 * 4, rue du Clos Courtel
 * 35512 CESSON-SEVIGNE
 * FRANCE
 * 
 * e-mail: g_montel@yahoo.com
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
 * http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>			/* for atexit() */
#include <bsp.h>
#include <rtems/libio.h>
#include <m68340.h>

#define CLOCK_VECTOR	120		/* clock isr routine vector in the vbr */
#define CLOCK_IRQ_LEVEL	6		/* clock isr level */

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

/*
 * Periodic interval timer interrupt handler
 */

/******************************************************
  Name: Clock_isr
  Input parameters: irq vector 
  Output parameters: none
  Description: update # of clock ticks
 *****************************************************/
rtems_isr
Clock_isr (rtems_vector_number vector)
{
	/*
	 * Announce the clock tick
	 */
	Clock_driver_ticks++;
	rtems_clock_tick();
}

/******************************************************
  Name: clock_exit
  Input parameters: -
  Output parameters: -
  Description: turn off periodic time at shutdown
 *****************************************************/
void
Clock_exit (void)
{
	/*
	 * Turn off periodic interval timer
	 */		
	SIMPITR = 0;
}

/******************************************************
  Name: Install_clock
  Input parameters: the Clock Interrupt Subroutine
  Output parameters: -
  Description: initialize the periodic interval ticker
	       called by Clock_Initialize
 *****************************************************/
static void
Install_clock (rtems_isr_entry clock_isr)
{
	unsigned32 pitr_tmp;
	unsigned32 usecs_per_tick;

	Clock_driver_ticks = 0;

	set_vector (clock_isr, CLOCK_VECTOR, 1);
		
	/* sets the Periodic Interrupt Control Register PICR */
	/* voir a quoi correspond exactement le Clock Vector */

	SIMPICR = ( CLOCK_IRQ_LEVEL << 8 ) | ( CLOCK_VECTOR );		
			  
	/* sets the PITR count value */
	/* this assumes a 32.765 kHz crystal */
		
        usecs_per_tick = BSP_Configuration.microseconds_per_tick;
	/* find out whether prescaler should be enabled or not */
	if ( usecs_per_tick <= 31128 ) {
	   pitr_tmp = ( usecs_per_tick * 8192 ) / 1000000 ;
	} else {
	   pitr_tmp = ( usecs_per_tick / 1000000 ) * 16;
	   /* enable it */		   
	   pitr_tmp |= 0x100; 		   
	}

	SIMPITR = (unsigned char) pitr_tmp;

	atexit (Clock_exit);
}

/******************************************************
  Name: Clock_initialize
  Input parameters: major & minor numbers
  Output parameters: - 
  Description: main entry for clock initialization
	       calls the bsp dependant routine
 *****************************************************/
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
 
/******************************************************
  Name: Clock_control
  Input parameters: major & minor number
  Output parameters:
  Description:
 *****************************************************/
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
