/*
 *  Clock Tick Device Driver
 *
 *  This routine utilizes the Decrementer Register common to the PPC family.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <stdlib.h>                     /* for atexit() */
#include <assert.h>
#include <libcpu/cpu.h>

/*
 *  Clock ticks since initialization
 */

volatile rtems_unsigned32 Clock_driver_ticks;

/*
 *  This is the value programmed into the count down timer.
 */

rtems_unsigned32 Clock_Decrementer_value;

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

void clockOff(const rtems_irq_connect_data* unused)
{
  if (BSP_Configuration.ticks_per_timeslice)    {
    /*
     * Nothing to do as we cannot disable all interrupts and
     * the decrementer interrupt enable is MSR_EE
     */
  }
}
static void clockOn(const rtems_irq_connect_data* unused)
{
  PPC_Set_decrementer( Clock_Decrementer_value );
}

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
void clockIsr()
{
  /*
   *  The driver has seen another tick.
   */

  PPC_Set_decrementer( Clock_Decrementer_value );

  Clock_driver_ticks += 1;

  /*
   *  Real Time Clock counter/timer is set to automatically reload.
   */

  rtems_clock_tick();
}

int clockIsOn(const rtems_irq_connect_data* unused)
{
  unsigned32 msr_value;

  _CPU_MSR_GET( msr_value );
  if (msr_value & MSR_EE) return 1;
  return 0;
}

static rtems_irq_connect_data clockIrqData = {BSP_DECREMENTER,
					      clockIsr,
					      clockOn,
					      clockOff,
					      clockIsOn};
					      

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
  if ( BSP_Configuration.ticks_per_timeslice ) {
   BSP_remove_rtems_irq_handler (&clockIrqData);
  }
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
  Clock_Decrementer_value = (BSP_bus_frequency/4000)*
                            (BSP_Configuration.microseconds_per_tick/1000);

  if (!BSP_install_rtems_irq_handler (&clockIrqData)) {
    printk("Unable to initialize system clock\n");
    rtems_fatal_error_occurred(1);
  }
  /* make major/minor avail to others such as shared memory driver */
 
  rtems_clock_major = major;
  rtems_clock_minor = minor;

  return RTEMS_SUCCESSFUL;
} /* Clock_initialize */
 
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
 
    if (args == 0)
        goto done;
 
    Clock_Decrementer_value = (BSP_bus_frequency/4000)*
      (BSP_Configuration.microseconds_per_tick/1000);

    if      (args->command == rtems_build_name('I', 'S', 'R', ' '))
      clockIsr();
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
      if (!BSP_install_rtems_irq_handler (&clockIrqData)) {
	printk("Error installing clock interrupt handler!\n");
	rtems_fatal_error_occurred(1);
      }
    }
done:
    return RTEMS_SUCCESSFUL;
}






