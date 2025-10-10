/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup A9MPCoreSupport
 *
 * @brief This source file contains the Clock Driver for Cortex-A9 MPCore
 * compatible devices.
 */

/*
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

__attribute__ ((weak)) uint32_t a9mpcore_clock_periphclk(void)
{
  /* default to the BSP option. */
  return BSP_ARM_A9MPCORE_PERIPHCLK;
}

static void a9mpcore_clock_at_tick(volatile a9mpcore_gt *gt)
{
  gt->irqst = A9MPCORE_GT_IRQST_EFLG;
}

static rtems_interrupt_entry a9mpcore_clock_interrupt_entry;

static void a9mpcore_clock_handler_install(rtems_interrupt_handler handler)
{
  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    &a9mpcore_clock_interrupt_entry,
    handler,
    RTEMS_DEVOLATILE(a9mpcore_gt *, A9MPCORE_GT),
    "Clock"
  );
  sc = rtems_interrupt_entry_install(
    A9MPCORE_IRQ_GT,
    RTEMS_INTERRUPT_UNIQUE,
    &a9mpcore_clock_interrupt_entry
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
  (void) tc;

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
#else
  (void) gt;
  (void) cmpval;
  (void) interval;
#endif
}

static void a9mpcore_clock_initialize(void)
{
  volatile a9mpcore_gt *gt = A9MPCORE_GT;
  uint64_t periphclk = a9mpcore_clock_periphclk();
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t interval = (uint32_t) ((periphclk * us_per_tick) / 1000000) - 1;
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

#define Clock_driver_support_at_tick(arg) \
  a9mpcore_clock_at_tick(arg)

#define Clock_driver_support_initialize_hardware() \
  a9mpcore_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  a9mpcore_clock_handler_install(isr)

/* Include shared source clock driver code */
#include "../../shared/dev/clock/clockimpl.h"
