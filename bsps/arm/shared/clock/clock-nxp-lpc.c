/**
 * @file
 *
 * @ingroup lpc_clock
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2009-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/timecounter.h>

#include <bsp/lpc-clock-config.h>
#include <bsp/lpc-timer.h>

#ifdef ARM_MULTILIB_ARCH_V4

/* This is defined in ../../../shared/dev/clock/clockimpl.h */
void Clock_isr(rtems_irq_hdl_param arg);

static volatile lpc_timer *const lpc_clock =
  (volatile lpc_timer *) LPC_CLOCK_TIMER_BASE;

static volatile lpc_timer *const lpc_timecounter =
  (volatile lpc_timer *) LPC_CLOCK_TIMECOUNTER_BASE;

static struct timecounter lpc_clock_tc;

static uint32_t lpc_clock_tc_get_timecount(struct timecounter *tc)
{
  return lpc_timecounter->tc;
}

static void lpc_clock_at_tick(void)
{
  lpc_clock->ir = LPC_TIMER_IR_MR0;
}

static void lpc_clock_handler_install(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    LPC_CLOCK_INTERRUPT,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void lpc_clock_initialize(void)
{
  uint64_t interval = ((uint64_t) LPC_CLOCK_REFERENCE
    * (uint64_t) rtems_configuration_get_microseconds_per_tick()) / 1000000;

  /* Enable module */
  LPC_CLOCK_MODULE_ENABLE();

  /* Reset timer */
  lpc_clock->tcr = LPC_TIMER_TCR_RST;

  /* Clear interrupt flags */
  lpc_clock->ir = LPC_TIMER_IR_ALL;

  /* Set timer mode */
  lpc_clock->ccr = 0;

  /* Timer is incremented every PERIPH_CLK tick */
  lpc_clock->pr = 0;

  /* Set match registers */
  lpc_clock->mr0 = (uint32_t) interval;

  /* Generate interrupt and reset counter on match with MR0 */
  lpc_clock->mcr = LPC_TIMER_MCR_MR0_INTR | LPC_TIMER_MCR_MR0_RST;

  /* No external match */
  lpc_clock->emr = 0x0;

  /* Enable timer */
  lpc_clock->tcr = LPC_TIMER_TCR_EN;

  /* Install timecounter */
  lpc_clock_tc.tc_get_timecount = lpc_clock_tc_get_timecount;
  lpc_clock_tc.tc_counter_mask = 0xffffffff;
  lpc_clock_tc.tc_frequency = LPC_CLOCK_REFERENCE;
  lpc_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&lpc_clock_tc);
}

#define Clock_driver_support_at_tick() lpc_clock_at_tick()
#define Clock_driver_support_initialize_hardware() lpc_clock_initialize()
#define Clock_driver_support_install_isr(isr) \
  lpc_clock_handler_install()

/* Include shared source clock driver code */
#include "../../../shared/dev/clock/clockimpl.h"

#endif /* ARM_MULTILIB_ARCH_V4 */
