/*
 *  LPC22XX/LPC21xx clock specific using the System Timer
 *
 *  Set the Time0 to generate click for RTEMS
 */

/*
 *  Copyright (c) 2006 by Ray <rayx.cn@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <lpc22xx.h>
#include <rtems/bspIo.h>  /* for printk */
#include <rtems/timecounter.h>

void Clock_isr(rtems_irq_hdl_param arg);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

static rtems_timecounter_simple lpc22xx_tc;

static uint32_t lpc22xx_tc_get(rtems_timecounter_simple *tc)
{
  return T0TC;
}

static bool lpc22xx_tc_is_pending(rtems_timecounter_simple *tc)
{
  return (T0IR & 0x1) != 0;
}

static uint32_t lpc22xx_tc_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_upcounter_get(
    tc,
    lpc22xx_tc_get,
    lpc22xx_tc_is_pending
  );
}

/**
 * When we get the clock interrupt
 *    - clear the interrupt bit?
 *    - restart the timer?
 */
static void lpc22xx_tc_at_tick(rtems_timecounter_simple *tc)
{
  if (!(T0IR & 0x01))
    return;
  T0IR = 0x01;
  VICVectAddr = 0x00;
}

static void lpc22xx_tc_tick(void)
{
  rtems_timecounter_simple_upcounter_tick(
    &lpc22xx_tc,
    lpc22xx_tc_get,
    lpc22xx_tc_at_tick
  );
}

/* Replace the first value with the clock's interrupt name. */
rtems_irq_connect_data clock_isr_data = {
  .name   = LPC22xx_INTERRUPT_TIMER0,
  .hdl    = Clock_isr,
  .handle = NULL,
  .on     = clock_isr_on,
  .off    = clock_isr_off,
  .isOn   = clock_isr_is_on,
};

/* use the /shared/dev/clock/clockimpl.h code template */

/**
 * Installs the clock ISR. You shouldn't need to change this.
 */
#define Clock_driver_support_install_isr( _new ) \
  BSP_install_rtems_irq_handler(&clock_isr_data)

/**
 * Initialize the hardware for the clock
 *   - Set the frequency
 *   - enable it
 *   - clear any pending interrupts
 *
 * Since you may want the clock always running, you can
 * enable interrupts here. If you do so, the clock_isr_on(),
 * clock_isr_off(), and clock_isr_is_on() functions can be
 * NOPs.
 * 
 * set timer to generate interrupt every
 * rtems_configuration_get_microseconds_per_tick()
 *     MR0/(LPC22xx_Fpclk/(PR0+1)) = 10/1000 = 0.01s
 */
#define Clock_driver_support_initialize_hardware() \
  do { \
    /* disable and clear timer 0, set to  */ \
    T0TCR &= 0;                              \
    /* TC is incremented on every pclk.*/    \
    T0PC   = 0;                              \
    /* initialize the timer period and prescaler */  \
    T0MR0  = ((LPC22xx_Fpclk/1000 *          \
             rtems_configuration_get_microseconds_per_tick()) / 1000); \
    /* generate interrupt when T0MR0 match T0TC and Reset Timer Count*/ \
    T0MCR |= 0x03;          \
    /* No external match */ \
    T0EMR = 0;              \
    /* enable timer0 */     \
    T0TCR = 1;              \
    /* enable interrupt, skyeye will check this*/ \
    T0IR |= 0x01; \
    /* install timecounter */ \
    rtems_timecounter_simple_install( \
      &lpc22xx_tc, \
      LPC22xx_Fpclk, \
      T0MR0, \
      lpc22xx_tc_get_timecount \
    ); \
  } while (0)

/**
 * Enables clock interrupt.
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_on(const rtems_irq_connect_data *unused)
{
  T0IR&=0x01;
}

/**
 * Disables clock interrupts
 *
 * If the interrupt is always on, this can be a NOP.
 */
static void clock_isr_off(const rtems_irq_connect_data *unused)
{
  T0IR=0x00;
}

/**
 * Tests to see if clock interrupt is enabled, and returns 1 if so.
 * If interrupt is not enabled, returns 0.
 *
 * If the interrupt is always on, this always returns 1.
 */
static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  return T0IR & 0x01;  /* MR0 mask */
}

#define Clock_driver_timecounter_tick() lpc22xx_tc_tick()

/* Make sure to include this, and only at the end of the file */
#include "../../../shared/dev/clock/clockimpl.h"

