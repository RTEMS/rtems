/*  ckinit.c
 *
 *  This file provides a template for the clock device driver initialization.
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

#include <rtems.h>
#include <rtems/libio.h>
#include <bsp.h>

void Clock_exit( void );
rtems_isr Clock_isr( rtems_vector_number vector );


/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR    4

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

rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  The previous ISR on this clock tick interrupt vector.
 */

rtems_isr_entry  Old_ticker;

void Clock_exit( void );


/*
 *  Isr Handler
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
/*
 * bump the number of clock driver ticks since initialization
 *
 * determine if it is time to announce the passing of tick as configured
 * to RTEMS through the rtems_clock_tick directive
 *
 * perform any timer dependent tasks
 */
}

/*
 *  Install_clock
 *
 *  Install a clock tick handler and reprograms the chip.  This
 *  is used to initially establish the clock tick.
 */

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  /*
   *  Initialize the clock tick device driver variables
   */

  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  /*
   *  If ticks_per_timeslice is configured as non-zero, then the user
   *  wants a clock tick.
   */

  if ( BSP_Configuration.ticks_per_timeslice ) {
    Old_ticker = ( rtems_isr_entry ) set_vector( clock_isr, CLOCK_VECTOR, 1 );
    /*
     *  Hardware specific initialize goes here
     */

    /* XXX */
  }

  /*
   *  Schedule the clock cleanup routine to execute if the application exits.
   */

  atexit( Clock_exit );
}

/*
 *  Reinstall_clock
 *
 *  Install a clock tick handler without reprogramming the chip.  This
 *  is used by the polling shared memory device driver.
 */

void ReInstall_clock(
  rtems_isr_entry clock_isr
)
{
  rtems_unsigned32 isrlevel = 0;

  /*
   *  Disable interrupts and install the clock ISR vector using the
   *  BSP dependent set_vector routine.  In the below example, the clock
   *  ISR is on vector 4 and is an RTEMS interrupt.
   */

  rtems_interrupt_disable( isrlevel );
   (void) set_vector( clock_isr, CLOCK_VECTOR, 1 );
  rtems_interrupt_enable( isrlevel );
}

/*
 *  Clean up before the application exits
 */

void Clock_exit( void )
{
  if ( BSP_Configuration.ticks_per_timeslice ) {

    /* XXX: turn off the timer interrupts */

    /* XXX: If necessary, restore the old vector */
  }
}

/*
 *  Clock_initialize
 *
 *  Device driver entry point for clock tick driver initialization.
 */

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock( Clock_isr );
 
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
    rtems_libio_ioctl_args_t *args = pargp;
 
    if (args == 0)
        goto done;
 
    /*
     * This is hokey, but until we get a defined interface
     * to do this, it will just be this simple...
     */
 
    if (args->command == rtems_build_name('I', 'S', 'R', ' '))
    {
        Clock_isr(CLOCK_VECTOR);
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
        ReInstall_clock(args->buffer);
    }
 
done:
    return RTEMS_SUCCESSFUL;
}
