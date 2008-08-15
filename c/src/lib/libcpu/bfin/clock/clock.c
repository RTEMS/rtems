/*  RTEMS Clock Tick Driver for Blackfin.  Uses Blackfin Core Timer.
 * 
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 

#include <rtems.h>
#include <stdlib.h>
#include <rtems/libio.h>
#include <bsp.h>

#include <libcpu/cecRegs.h>
#include <libcpu/coreTimerRegs.h>


volatile uint32_t Clock_driver_ticks;

void Clock_exit(void);

/*
 *  Major and minor number.
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

static rtems_isr clockISR(rtems_vector_number vector) {

  Clock_driver_ticks += 1;

#ifdef CLOCK_DRIVER_USE_FAST_IDLE
  do {
    rtems_clock_tick();
  } while (_Thread_Executing == _Thread_Idle &&
           _Thread_Heir == _Thread_Executing);
#else
  rtems_clock_tick();
#endif
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

void Clock_exit(void) {

  *(uint32_t volatile *) TCNTL = 0;
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

rtems_device_driver Clock_initialize(rtems_device_major_number major,
                                     rtems_device_minor_number minor,
                                     void *pargp) {

  Clock_driver_ticks = 0;

  set_vector(clockISR, CEC_CORE_TIMER_VECTOR, 1);

  *(uint32_t volatile *) TCNTL = TCNTL_TMPWR | TCNTL_TAUTORLD;
  *(uint32_t volatile *) TSCALE = 0;
  *(uint32_t volatile *) TPERIOD = CCLK / 1000000 *
      rtems_configuration_get_microseconds_per_tick();
  *(uint32_t volatile *) TCNTL = TCNTL_TMPWR | TCNTL_TAUTORLD | TCNTL_TMREN;

  atexit(Clock_exit);
  /*
   * make major/minor avail to others such as shared memory driver
   */

  rtems_clock_major = major;
  rtems_clock_minor = minor;

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

rtems_device_driver Clock_control(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void *pargp) {
  rtems_interrupt_level isrLevel;
  rtems_libio_ioctl_args_t *args = pargp;

  if (args == 0)
    goto done;

  /*
   * This is hokey, but until we get a defined interface
   * to do this, it will just be this simple...
   */

  if (args->command == rtems_build_name('I', 'S', 'R', ' ')) {
    clockISR(CEC_CORE_TIMER_VECTOR);
  } else if (args->command == rtems_build_name('N', 'E', 'W', ' ')) {
    rtems_interrupt_disable(isrLevel);
    set_vector(args->buffer, CEC_CORE_TIMER_VECTOR, 1);
    rtems_interrupt_enable(isrLevel);
  }

done:
  return RTEMS_SUCCESSFUL;
}

