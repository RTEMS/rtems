
/*  ckinit.c
 *
 *  This file contains the clock driver initialization for the IDT 4650.
 *
 *  Author:     Craig Lebakken <craigl@transition.com>
 *
 *  COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Transition Networks not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      Transition Networks makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/clock/ckinit.c:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

/*
 *  Rather than deleting this, it is commented out to (hopefully) help
 *  the submitter send updates.
 *
 *  static char _sccsid[] = "@(#)ckinit.c 08/20/96     1.3\n";
 */


#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>

#define EXT_INT5    0x8000  /* external interrupt 5 */

#include "clock.h"

/* formerly in the BSP */
#if 0
#define CLOCKS_PER_MICROSECOND ( CPU_CLOCK_RATE_MHZ ) /* equivalent to CPU clock speed in MHz */
#endif

#define CLOCKS_PER_MICROSECOND \
  rtems_cpu_configuration_get_clicks_per_microsecond()
/* to avoid including the bsp */
mips_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );

void Clock_exit( void );
rtems_isr Clock_isr( rtems_vector_number vector );


/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR_MASK    EXT_INT5
#define CLOCK_VECTOR         0x7

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

static unsigned32 mips_timer_rate = 0;

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

  /* refresh the internal CPU timer */
  mips_set_timer( mips_timer_rate );

  Clock_driver_ticks += 1;

  rtems_clock_tick();
}

/* User callback shell (set from Clock_Control) */
static void (*user_callback)(void);

rtems_isr User_Clock_isr(
  rtems_vector_number vector
)
{
   /* refresh the internal CPU timer */
   mips_set_timer( mips_timer_rate );

   if (user_callback)
      user_callback();
}

/*
 *  Install_clock
 *
 *  Install a clock tick handleR and reprograms the chip.  This
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
  Clock_isrs = rtems_configuration_get_milliseconds_per_tick();

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );
  /*
   *  Hardware specific initialize goes here
   */

  mips_timer_rate =
     rtems_configuration_get_microseconds_per_tick() * CLOCKS_PER_MICROSECOND;
  mips_set_timer( mips_timer_rate );
  mips_enable_in_interrupt_mask(CLOCK_VECTOR_MASK);

  /*
   *  Schedule the clock cleanup routine to execute if the application exits.
   */

  atexit( Clock_exit );
}

/*
 *  Clean up before the application exits
 */

void Clock_exit( void )
{
  /* mips: turn off the timer interrupts */
  mips_disable_in_interrupt_mask(CLOCK_VECTOR_MASK);
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
    rtems_unsigned32 isrlevel;
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
      rtems_interrupt_disable( isrlevel );
      user_callback = (void (*)(void))args->buffer;
      (void) set_vector( User_Clock_isr, CLOCK_VECTOR, 1 );
      rtems_interrupt_enable( isrlevel );
    }
 
done:
    return RTEMS_SUCCESSFUL;
}
