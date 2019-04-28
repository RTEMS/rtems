/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/arm-a9mpcore-regs.h>
#include <bsp/arm-a9mpcore-clock.h>
#include <rtems/timecounter.h>
#include <rtems/score/smpimpl.h>

#define A9MPCORE_GT ((volatile a9mpcore_gt *) BSP_ARM_A9MPCORE_GT_BASE)

static struct timecounter a9mpcore_tc;

/* This is defined in dev/clock/clockimpl.h */
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
    bsp_fatal(BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_INSTALL);
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

static uint32_t a9mpcore_clock_get_timecount(struct timecounter *tc)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;

  return gt->cntrlower;
}

static void a9mpcore_clock_gt_init(
  volatile a9mpcore_gt *gt,
  uint64_t cmpval,
  uint32_t interval
)
{
  gt->cmpvallower = (uint32_t) cmpval;
  gt->cmpvalupper = (uint32_t) (cmpval >> 32);
  gt->autoinc = interval;
  gt->ctrl = A9MPCORE_GT_CTRL_AUTOINC_EN
    | A9MPCORE_GT_CTRL_IRQ_EN
    | A9MPCORE_GT_CTRL_COMP_EN
    | A9MPCORE_GT_CTRL_TMR_EN;
}

#if defined(RTEMS_SMP) && !defined(CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR)
typedef struct {
  uint64_t cmpval;
  uint32_t interval;
} a9mpcore_clock_init_data;

static void a9mpcore_clock_secondary_action(void *arg)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;
  a9mpcore_clock_init_data *init_data = arg;

  a9mpcore_clock_gt_init(gt, init_data->cmpval, init_data->interval);
  bsp_interrupt_vector_enable(A9MPCORE_IRQ_GT);
}
#endif

static void a9mpcore_clock_secondary_initialization(
  volatile a9mpcore_gt *gt,
  uint64_t cmpval,
  uint32_t interval
)
{
#if defined(RTEMS_SMP) && !defined(CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR)
  a9mpcore_clock_init_data init_data = {
    .cmpval = cmpval,
    .interval = interval
  };

  _SMP_Othercast_action(a9mpcore_clock_secondary_action, &init_data);

  if (cmpval - a9mpcore_clock_get_counter(gt) >= interval) {
    bsp_fatal(BSP_ARM_A9MPCORE_FATAL_CLOCK_SMP_INIT);
  }
#endif
}

static void a9mpcore_clock_initialize(void)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;
  uint64_t periphclk = a9mpcore_clock_periphclk();
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t interval = (uint32_t) ((periphclk * us_per_tick) / 1000000);
  uint64_t cmpval;

  gt->ctrl &= A9MPCORE_GT_CTRL_TMR_EN;
  gt->irqst = A9MPCORE_GT_IRQST_EFLG;

  cmpval = a9mpcore_clock_get_counter(gt);
  cmpval += interval;

  a9mpcore_clock_gt_init(gt, cmpval, interval);
  a9mpcore_clock_secondary_initialization(gt, cmpval, interval);

  a9mpcore_tc.tc_get_timecount = a9mpcore_clock_get_timecount;
  a9mpcore_tc.tc_counter_mask = 0xffffffff;
  a9mpcore_tc.tc_frequency = periphclk;
  a9mpcore_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&a9mpcore_tc);
}

uint32_t _CPU_Counter_frequency(void)
{
  return a9mpcore_clock_periphclk();
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;

  return gt->cntrlower;
}

#define Clock_driver_support_at_tick() \
  a9mpcore_clock_at_tick()

#define Clock_driver_support_initialize_hardware() \
  a9mpcore_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  a9mpcore_clock_handler_install()

/* Include shared source clock driver code */
#include "../../shared/dev/clock/clockimpl.h"
