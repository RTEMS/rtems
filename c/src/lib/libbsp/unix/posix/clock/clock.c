/*  Clock
 *
 *  This routine generates clock ticks using standard POSIX services.
 *  The tick frequency is specified by the bsp.
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

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>

void Clock_exit(void);

volatile rtems_unsigned32 Clock_driver_ticks;

rtems_unsigned32 Clock_driver_vector;

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

void Install_clock(rtems_isr_entry clock_isr)
{
    Clock_driver_ticks = 0;

    (void) set_vector( clock_isr, Clock_driver_vector, 1 );

    _CPU_Start_clock( BSP_Configuration.microseconds_per_tick );

    atexit(Clock_exit);
}

void Clock_isr(int vector)
{
    Clock_driver_ticks++;
    rtems_clock_tick();
}

/*
 * Called via atexit()
 * Remove the clock signal
 */

void Clock_exit(void)
{
  _CPU_Stop_clock();

  (void) set_vector( 0, Clock_driver_vector, 1 );
}

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
    Clock_driver_vector = _CPU_Get_clock_vector();

    Install_clock((rtems_isr_entry) Clock_isr);

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
        Clock_isr(Clock_driver_vector);
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
      rtems_interrupt_disable( isrlevel );
       (void) set_vector( args->buffer, Clock_driver_vector, 1 );
      rtems_interrupt_enable( isrlevel );
    }
    
done:
    return RTEMS_SUCCESSFUL;
}
