/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#define A9MPCORE_PT ((volatile a9mpcore_pt *) BSP_ARM_A9MPCORE_PT_BASE)

/* This is defined in clockdrv_shell.h */
void Clock_isr(rtems_irq_hdl_param arg);

static void a9mpcore_clock_at_tick(void)
{
  volatile a9mpcore_pt *pt = A9MPCORE_PT;

  pt->irqst = A9MPCORE_PT_IRQST_EFLG;
}

static void a9mpcore_clock_handler_install(void)
{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    A9MPCORE_IRQ_PT,
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

static void a9mpcore_clock_initialize(void)
{
  volatile a9mpcore_pt *pt = A9MPCORE_PT;
  uint64_t interval = ((uint64_t) BSP_ARM_A9MPCORE_PERIPHCLK
    * (uint64_t) rtems_configuration_get_microseconds_per_tick()) / 1000000;

  pt->load = (uint32_t) interval - 1;
  pt->ctrl = A9MPCORE_PT_CTRL_AUTO_RLD
    | A9MPCORE_PT_CTRL_IRQ_EN
    | A9MPCORE_PT_CTRL_TMR_EN;
}

static void a9mpcore_clock_cleanup(void)
{
  volatile a9mpcore_pt *pt = A9MPCORE_PT;
  rtems_status_code sc;

  pt->ctrl = 0;
  pt->irqst = A9MPCORE_PT_IRQST_EFLG;

  sc = rtems_interrupt_handler_remove(
    A9MPCORE_IRQ_PT,
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
  volatile a9mpcore_pt *pt = A9MPCORE_PT;
  uint64_t k = (1000000000ULL << 32) / BSP_ARM_A9MPCORE_PERIPHCLK;
  uint32_t c = pt->cntr;
  uint32_t p = pt->load + 1;

  if ((pt->irqst & A9MPCORE_PT_IRQST_EFLG) != 0) {
    c = pt->cntr - p;
  }

  return (uint32_t) (((p - c) * k) >> 32);
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
