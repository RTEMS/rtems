/*
 *  PXA255 clock specific using the System Timer
 *
 *  RTEMS uses IRQ 26 as Clock Source
 */

/*
 *  By Yang Xi <hiyangxi@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>

#include <stdlib.h>
#include <bsp.h>
#include <bspopts.h>
#include <bsp/irq.h>
#include <pxa255.h>

#if ON_SKYEYE==1
  #define CLOCK_DRIVER_USE_FAST_IDLE 1
#endif

static unsigned long period_num;

/**
 * Enables clock interrupt.
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
  /*Clear the interrupt bit */
  XSCALE_OS_TIMER_TSR = 0x1;

  /* enable timer interrupt */
  XSCALE_OS_TIMER_IER |= 0x1;

#if ON_SKYEYE==1
  period_num = (TIMER_RATE* rtems_configuration_get_microseconds_per_tick())/100000;
#else
  period_num = (TIMER_RATE* rtems_configuration_get_microseconds_per_tick())/10000;
#endif

  XSCALE_OS_TIMER_MR0 = XSCALE_OS_TIMER_TCR + period_num;
}

/**
 * Disables clock interrupts
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
  /*Clear the interrupt bit */
  XSCALE_OS_TIMER_TSR = 0x1;
  /* disable timer interrupt*/
  XSCALE_OS_TIMER_IER &= ~0x1;
}

/**
 * Tests to see if clock interrupt is enabled, and returns 1 if so.
 * If interrupt is not enabled, returns 0.
 *
 * If the interrupt is always on, this always returns 1.
 */
static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  /* check timer interrupt */
  return XSCALE_OS_TIMER_IER & 0x1;
}

void Clock_isr(rtems_irq_hdl_param arg);

rtems_irq_connect_data clock_isr_data = {
  .name   = XSCALE_IRQ_OS_TIMER,
  .hdl    = Clock_isr,
  .handle = NULL,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};

#define Clock_driver_support_install_isr( _new ) \
  BSP_install_rtems_irq_handler(&clock_isr_data)

static void Clock_driver_support_initialize_hardware(void)
{
  period_num = TIMER_RATE* rtems_configuration_get_microseconds_per_tick();
#if ON_SKYEYE==1
  period_num /= 100000;
#else
  period_num /= 10000;
#endif
}

#define Clock_driver_support_at_tick() \
  do { \
    /* read the status to clear the int */ \
    XSCALE_OS_TIMER_TSR = 0x1; \
    \
    /*Add the match register*/ \
    XSCALE_OS_TIMER_MR0 = XSCALE_OS_TIMER_TCR + period_num; \
  } while (0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"
