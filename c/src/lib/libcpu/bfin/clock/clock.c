/*  RTEMS Clock Tick Driver for Blackfin.  Uses Blackfin Core Timer.
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <rtems.h>
#include <stdlib.h>
#include <rtems/libio.h>
#include <bsp.h>

#include <libcpu/cecRegs.h>
#include <libcpu/coreTimerRegs.h>

#if (BFIN_ON_SKYEYE)
#define CLOCK_DRIVER_USE_FAST_IDLE
#endif

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
