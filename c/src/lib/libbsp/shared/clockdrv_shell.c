/*
 *  Clock Tick Device Driver Shell
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

#include <stdlib.h>

#include <bsp.h>
#include <rtems/libio.h>

#if defined(CLOCK_DRIVER_USE_FAST_IDLE) && defined(CLOCK_DRIVER_ISRS_PER_TICK)
#error "clockdrv_shell.c: fast idle and N ISRs per tick is not supported"
#endif

/*
 *  ISRs until next clock tick
 */

#ifdef CLOCK_DRIVER_ISRS_PER_TICK
volatile rtems_unsigned32 Clock_driver_isrs;
#endif

/*
 *  Clock ticks since initialization
 */

volatile rtems_unsigned32 Clock_driver_ticks;

/*
 *  ISR formerly installed.
 */

rtems_isr_entry  Old_ticker;

void Clock_exit( void );
 
/*
 *  Major and minor number.
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  Clock_isr
 *
 *  This is the clock tick interrupt handler.
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  /*
   *  Accurate count of ISRs
   */

  Clock_driver_ticks += 1;

#ifdef CLOCK_DRIVER_USE_FAST_IDLE
  do { 
    rtems_clock_tick();
  } while ( _Thread_Executing == _Thread_Idle &&
          _Thread_Heir == _Thread_Executing);

  Clock_driver_support_at_tick();
  return;

#else

  /*
   *  Do the hardware specific per-tick action.
   *
   *  The counter/timer may or may not be set to automatically reload.
   */

  Clock_driver_support_at_tick();

#ifdef CLOCK_DRIVER_ISRS_PER_TICK
  /*
   *  The driver is multiple ISRs per clock tick.
   */

  if ( !Clock_driver_isrs ) {

    rtems_clock_tick();

    Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;
  }
  Clock_driver_isrs--;
#else

  /*
   *  The driver is one ISR per clock tick.
   */
  rtems_clock_tick();
#endif
#endif
}

/*
 *  Install_clock
 *
 *  This routine actually performs the hardware initialization for the clock.
 *
 *  Input parameters:
 *    clock_isr - clock interrupt service routine entry point
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  Clock_driver_ticks = 0;

  /*
   *  Install vector
   */

  Clock_driver_support_install_isr( clock_isr, Old_ticker );

  /*
   *  Now initialize the hardware that is the source of the tick ISR.
   */

  Clock_driver_support_initialize_hardware();

  atexit( Clock_exit );
}

/*
 *  Clock_exit
 *
 *  This routine allows the clock driver to exit by masking the interrupt and
 *  disabling the clock's counter.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */

void Clock_exit( void )
{
  Clock_driver_support_shutdown_hardware();

  /* do not restore old vector */
}
 
/*
 *  Clock_initialize
 *
 *  This routine initializes the clock driver.
 *
 *  Input parameters:
 *    major - clock device major number
 *    minor - clock device minor number
 *    parg  - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
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
 
  /*
   *  If we are counting ISRs per tick, then initialize the counter.
   */

#ifdef CLOCK_DRIVER_ISRS_PER_TICK
  Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;
#endif

  return RTEMS_SUCCESSFUL;
}
 
/*
 *  Clock_control
 *
 *  This routine is the clock device driver control entry point.
 *
 *  Input parameters:
 *    major - clock device major number
 *    minor - clock device minor number
 *    parg  - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */

rtems_device_driver Clock_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
    rtems_unsigned32 isrlevel;
    rtems_libio_ioctl_args_t *args = pargp;
    rtems_isr_entry  ignored_ticker;
 
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
       Clock_driver_support_install_isr( args->buffer, ignored_ticker );
      rtems_interrupt_enable( isrlevel );
    }
 
done:
    return RTEMS_SUCCESSFUL;
}
