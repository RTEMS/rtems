/*  ckinit.c
 *
 *  This file provides a template for the clock device driver initialization.
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

#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <bsp.h>
#include <c4xio.h>

void Clock_exit( void );
rtems_isr Clock_isr( rtems_vector_number vector );

rtems_unsigned32 Clock_counter_register_value;

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR    9

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */

volatile rtems_unsigned32 Clock_driver_ticks;

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

#define FAST_IDLE 1
#if FAST_IDLE
int Clock_in_fast_idle_mode = 0;
#endif

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
 /*
  *  The counter register gets reset automatically as well as the 
  *  interrupt occurred flag so we should not have to do anything
  *  with the hardware.
  */

 /*
  *  Bump the number of clock driver ticks since initialization
  */

 Clock_driver_ticks += 1;

 rtems_clock_tick();

#if FAST_IDLE
  if ( Clock_in_fast_idle_mode ) {
    if ( _Thread_Executing == _Thread_Idle && _Thread_Heir != _Thread_Idle ) {
      c4x_timer_stop( C4X_TIMER_0 );
      c4x_timer_set_counter( C4X_TIMER_0, 0 );
      c4x_timer_set_period( C4X_TIMER_0, Clock_counter_register_value );
      c4x_timer_start( C4X_TIMER_0 );
      Clock_in_fast_idle_mode = 0;
    }
  } else {
    if ( _Thread_Executing == _Thread_Idle && _Thread_Heir == _Thread_Idle ) {
      c4x_timer_stop( C4X_TIMER_0 );
      c4x_timer_set_counter( C4X_TIMER_0, 0 );
      c4x_timer_set_period( C4X_TIMER_0, Clock_counter_register_value >> 5 );
      c4x_timer_start( C4X_TIMER_0 );
    }
  }
#endif
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
  extern int _ClockFrequency;
  float tmp;
  int tmpi;
  /*
   *  Initialize the clock tick device driver variables
   */

  Clock_driver_ticks = 0;

  tmpi = ((int) &_ClockFrequency) * 1000000;  /* ClockFrequency is in Mhz */
  tmp = (float) tmpi / 2.0;
  tmp = ((float) BSP_Configuration.microseconds_per_tick / 1000000.0) * (tmp);

  Clock_counter_register_value = (unsigned int) tmp;
#if 0
  Clock_counter_register_value = 
      (unsigned32) ((float) BSP_Configuration.microseconds_per_tick /
       ((float)_ClockFrequency / 2.0)));
#endif
  c4x_timer_stop( C4X_TIMER_0 );
  c4x_timer_set_counter( C4X_TIMER_0, 0 );
  c4x_timer_set_period( C4X_TIMER_0, Clock_counter_register_value );
  c4x_timer_start( C4X_TIMER_0 );

#if defined(_C4x)
  c4x_set_iee( c4x_get_iie() | 0x1 );  /* should be ETINT0 */
#else
  c3x_set_ie( c3x_get_ie() | 0x100 );
#endif

  /*
   *  If ticks_per_timeslice is configured as non-zero, then the user
   *  wants a clock tick.
   */

  Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );
  /*
   *  Hardware specific initialize goes here
   */

  /* XXX */

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
  /* XXX: turn off the timer interrupts */

  /* XXX: If necessary, restore the old vector */
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
       (void) set_vector( args->buffer, CLOCK_VECTOR, 1 );
      rtems_interrupt_enable( isrlevel );
    }
 
done:
    return RTEMS_SUCCESSFUL;
}
