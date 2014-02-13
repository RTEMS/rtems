/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/arm-a9mpcore-regs.h>
#include <bsp/arm-a9mpcore-clock.h>

#define A9MPCORE_GT ((volatile a9mpcore_gt *) BSP_ARM_A9MPCORE_GT_BASE)

static uint64_t a9mpcore_clock_last_tick_k;

/* This is defined in clockdrv_shell.h */
void Clock_isr(rtems_irq_hdl_param arg);

__attribute__ ((weak)) uint32_t a9mpcore_clock_periphclk(void)
{
  /* default to the BSP option. */
  return BSP_ARM_A9MPCORE_PERIPHCLK;
}

static void a9mpcore_clock_at_tick(void)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;

  gt->irqst = A9MPCORE_GT_IRQST_EFLG;
}

static void a9mpcore_clock_handler_install(void)
{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    A9MPCORE_IRQ_GT,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal(
      RTEMS_FATAL_SOURCE_BSP_SPECIFIC,
      BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_INSTALL
    );
  }
}

static uint64_t a9mpcore_clock_get_counter(volatile a9mpcore_gt *gt)
{
  uint32_t cl;
  uint32_t cu1;
  uint32_t cu2;

  do {
    cu1 = gt->cntrupper;
    cl = gt->cntrlower;
    cu2 = gt->cntrupper;
  } while (cu1 != cu2);

  return ((uint64_t) cu2 << 32) | cl;
}

static void a9mpcore_clock_initialize(void)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;
  uint64_t periphclk = a9mpcore_clock_periphclk();
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t interval = (uint32_t) ((periphclk * us_per_tick) / 1000000);
  uint64_t cmpval;

  a9mpcore_clock_last_tick_k = (UINT64_C(1000000000) << 32) / periphclk;

  gt->ctrl &= A9MPCORE_GT_CTRL_TMR_EN;
  gt->irqst = A9MPCORE_GT_IRQST_EFLG;

  cmpval = a9mpcore_clock_get_counter(gt);
  cmpval += interval;

  gt->cmpvallower = (uint32_t) cmpval;
  gt->cmpvalupper = (uint32_t) (cmpval >> 32);
  gt->autoinc = interval;
  gt->ctrl = A9MPCORE_GT_CTRL_AUTOINC_EN
    | A9MPCORE_GT_CTRL_IRQ_EN
    | A9MPCORE_GT_CTRL_COMP_EN
    | A9MPCORE_GT_CTRL_TMR_EN;
}

static void a9mpcore_clock_cleanup(void)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;
  rtems_status_code sc;

  gt->ctrl &= A9MPCORE_GT_CTRL_TMR_EN;
  gt->irqst = A9MPCORE_GT_IRQST_EFLG;

  sc = rtems_interrupt_handler_remove(
    A9MPCORE_IRQ_GT,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal(
      RTEMS_FATAL_SOURCE_BSP_SPECIFIC,
      BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_REMOVE
    );
  }
}

static uint32_t a9mpcore_clock_nanoseconds_since_last_tick(void)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;
  uint64_t k = a9mpcore_clock_last_tick_k;
  uint32_t c = gt->cntrlower;
  uint32_t n = gt->cmpvallower;
  uint32_t i = gt->autoinc;

  if ((gt->irqst & A9MPCORE_GT_IRQST_EFLG) != 0) {
    n = gt->cmpvallower - i;
  }

  return (uint32_t) (((c - n + i) * k) >> 32);
}

#define Clock_driver_support_at_tick() \
  a9mpcore_clock_at_tick()

#define Clock_driver_support_initialize_hardware() \
  a9mpcore_clock_initialize()

#define Clock_driver_support_install_isr(isr, old_isr) \
  do { \
    a9mpcore_clock_handler_install();	\
    old_isr = NULL; \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  a9mpcore_clock_cleanup()

#define Clock_driver_nanoseconds_since_last_tick \
  a9mpcore_clock_nanoseconds_since_last_tick

/* Include shared source clock driver code */
#include "../../shared/clockdrv_shell.h"
