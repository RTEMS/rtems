/**
 * @file
 *
 * @ingroup beagle_clock
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <bsp/beagle-clock-config.h>
#include <bsp/beagle-timer.h>

#ifdef ARM_MULTILIB_ARCH_V4

/* This is defined in ../../../shared/clockdrv_shell.h */
void Clock_isr(rtems_irq_hdl_param arg);

static volatile beagle_timer *const beagle_clock =
  (volatile beagle_timer *) BEAGLE_CLOCK_TIMER_BASE;

static void beagle_clock_at_tick(void)
{
  beagle_clock->ir = BEAGLE_TIMER_IR_MR0;
}

static void beagle_clock_handler_install(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    BEAGLE_CLOCK_INTERRUPT,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void beagle_clock_initialize(void)
{
  uint64_t interval = ((uint64_t) BEAGLE_CLOCK_REFERENCE
    * (uint64_t) rtems_configuration_get_microseconds_per_tick()) / 1000000;

  /* Enable module */
  BEAGLE_CLOCK_MODULE_ENABLE();

  /* Reset timer */
  beagle_clock->tcr = BEAGLE_TIMER_TCR_RST;

  /* Clear interrupt flags */
  beagle_clock->ir = BEAGLE_TIMER_IR_ALL;

  /* Set timer mode */
  beagle_clock->ccr = 0;

  /* Timer is incremented every PERIPH_CLK tick */
  beagle_clock->pr = 0;

  /* Set match registers */
  beagle_clock->mr0 = (uint32_t) interval;

  /* Generate interrupt and reset counter on match with MR0 */
  beagle_clock->mcr = BEAGLE_TIMER_MCR_MR0_INTR | BEAGLE_TIMER_MCR_MR0_RST;

  /* No external match */
  beagle_clock->emr = 0x0;

  /* Enable timer */
  beagle_clock->tcr = BEAGLE_TIMER_TCR_EN;
}

static void beagle_clock_cleanup(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* Disable timer */
  beagle_clock->tcr = 0x0;

  /* Remove interrupt handler */
  sc = rtems_interrupt_handler_remove(
    BEAGLE_CLOCK_INTERRUPT,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static uint32_t beagle_clock_nanoseconds_since_last_tick(void)
{
  uint64_t k = (1000000000ULL << 32) / BEAGLE_CLOCK_REFERENCE;
  uint64_t c = beagle_clock->tc;

  if ((beagle_clock->ir & BEAGLE_TIMER_IR_MR0) != 0) {
    c = beagle_clock->tc + beagle_clock->mr0;
  }

  return (uint32_t) ((c * k) >> 32);
}

#define Clock_driver_support_at_tick() beagle_clock_at_tick()
#define Clock_driver_support_initialize_hardware() beagle_clock_initialize()
#define Clock_driver_support_install_isr(isr, old_isr) \
  do {                   \
    beagle_clock_handler_install();          \
    old_isr = NULL;              \
  } while (0)

#define Clock_driver_support_shutdown_hardware() beagle_clock_cleanup()
#define Clock_driver_nanoseconds_since_last_tick \
  beagle_clock_nanoseconds_since_last_tick

/* Include shared source clock driver code */
#include "../../../shared/clockdrv_shell.h"

#endif /* ARM_MULTILIB_ARCH_V4 */
