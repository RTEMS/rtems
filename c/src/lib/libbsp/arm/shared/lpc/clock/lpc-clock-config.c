/**
 * @file
 *
 * @ingroup lpc_clock
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <bsp/lpc-clock-config.h>
#include <bsp/lpc-timer.h>

#ifdef ARM_MULTILIB_ARCH_V4

/* This is defined in ../../../shared/clockdrv_shell.h */
void Clock_isr(rtems_irq_hdl_param arg);

static volatile lpc_timer *const lpc_clock =
  (volatile lpc_timer *) LPC_CLOCK_TIMER_BASE;

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
}

static void lpc_clock_cleanup(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* Disable timer */
  lpc_clock->tcr = 0x0;

  /* Remove interrupt handler */
  sc = rtems_interrupt_handler_remove(
    LPC_CLOCK_INTERRUPT,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static uint32_t lpc_clock_nanoseconds_since_last_tick(void)
{
  uint64_t k = (1000000000ULL << 32) / LPC_CLOCK_REFERENCE;
  uint64_t c = lpc_clock->tc;

  if ((lpc_clock->ir & LPC_TIMER_IR_MR0) != 0) {
    c = lpc_clock->tc + lpc_clock->mr0;
  }

  return (uint32_t) ((c * k) >> 32);
}

#define Clock_driver_support_at_tick() lpc_clock_at_tick()
#define Clock_driver_support_initialize_hardware() lpc_clock_initialize()
#define Clock_driver_support_install_isr(isr, old_isr) \
  do {						       \
    lpc_clock_handler_install();		       \
    old_isr = NULL;				       \
  } while (0)

#define Clock_driver_support_shutdown_hardware() lpc_clock_cleanup()
#define Clock_driver_nanoseconds_since_last_tick \
  lpc_clock_nanoseconds_since_last_tick

/* Include shared source clock driver code */
#include "../../../../shared/clockdrv_shell.h"

#endif /* ARM_MULTILIB_ARCH_V4 */
