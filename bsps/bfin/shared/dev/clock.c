/*  RTEMS Clock Tick Driver for Blackfin.  Uses Blackfin Core Timer.
 */

/*
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


#include <rtems.h>
#include <stdlib.h>
#include <rtems/libio.h>
#include <rtems/score/percpu.h>
#include <rtems/score/thread.h>
#include <bsp.h>
#include <rtems/clockdrv.h>


#include <libcpu/cecRegs.h>
#include <libcpu/coreTimerRegs.h>

#if (BFIN_ON_SKYEYE)
#define CLOCK_DRIVER_USE_FAST_IDLE 1
#endif

volatile uint32_t Clock_driver_ticks;

static rtems_isr clockISR(rtems_vector_number vector) {

  Clock_driver_ticks += 1;

#if CLOCK_DRIVER_USE_FAST_IDLE
  do {
    rtems_clock_tick();
  } while ( _Thread_Heir == _Thread_Executing && _Thread_Executing->is_idle );
#else
  rtems_clock_tick();
#endif
}

/*
 *  Clock_exit
 *
 *  This routine allows the clock driver to exit by masking the interrupt and
 *  disabling the clock's counter.
 */
static void Clock_exit(void)
{
  *(uint32_t volatile *) TCNTL = 0;
}

void _Clock_Initialize( void )
{
  Clock_driver_ticks = 0;

  set_vector(clockISR, CEC_CORE_TIMER_VECTOR, 1);

  *(uint32_t volatile *) TCNTL = TCNTL_TMPWR | TCNTL_TAUTORLD;
  *(uint32_t volatile *) TSCALE = 0;
  *(uint32_t volatile *) TPERIOD = CCLK / 1000000 *
      rtems_configuration_get_microseconds_per_tick();
  *(uint32_t volatile *) TCNTL = TCNTL_TMPWR | TCNTL_TAUTORLD | TCNTL_TMREN;

  atexit(Clock_exit);
}
