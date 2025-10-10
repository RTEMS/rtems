/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDriverClockArmGenericTimer
 *
 * @brief This source file contains a Clock Driver implementation using
 *   Armv7-AR/AArch64 Generic Timer.
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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
#include <dev/clock/arm-generic-timer.h>

#include <rtems/counter.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>
#include <rtems/score/smpimpl.h>

/**
 * @defgroup RTEMSDriverClockArmGenericTimer \
 *   Armv7-AR/AArch64 Generic Timer Clock Driver
 *
 * @ingroup RTEMSDriverClockImpl
 *
 * @brief This group contains the Armv7-AR/AArch64 Generic Timer Clock Driver
 *   implementation.
 *
 * The BSP must provide the following function:
 *
 * void arm_generic_timer_get_config(uint32_t *frequency, uint32_t *irq);
 *
 * The BSP may optionally define ARM_GENERIC_TIMER_USE_VIRTUAL in <bsp.h> to
 * use the virtual timer instead of the physical timer.
 *
 * @{
 */

typedef struct {
  struct timecounter tc;
  uint32_t interval;
  rtems_vector_number irq;
} arm_gt_clock_context;

static arm_gt_clock_context arm_gt_clock_instance;

static void arm_gt_clock_at_tick(arm_gt_clock_context *ctx)
{
  uint64_t cval;
  uint32_t interval;

  interval = ctx->interval;
  cval = arm_gt_clock_get_compare_value();
  cval += interval;
  arm_gt_clock_set_compare_value(cval);
#ifdef ARM_GENERIC_TIMER_UNMASK_AT_TICK
  arm_gt_clock_set_control(0x1);
#endif /* ARM_GENERIC_TIMER_UNMASK_AT_TICK */
}

static rtems_interrupt_entry arm_gt_interrupt_entry;

static void arm_gt_clock_handler_install(rtems_interrupt_handler handler)
{
  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    &arm_gt_interrupt_entry,
    handler,
    &arm_gt_clock_instance,
    "Clock"
  );
  sc = rtems_interrupt_entry_install(
    arm_gt_clock_instance.irq,
    RTEMS_INTERRUPT_UNIQUE,
    &arm_gt_interrupt_entry
  );
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(BSP_ARM_FATAL_GENERIC_TIMER_CLOCK_IRQ_INSTALL);
  }
}

static uint32_t arm_gt_clock_get_timecount(struct timecounter *tc)
{
  (void) tc;

  return (uint32_t) arm_gt_clock_get_count();
}

static void arm_gt_clock_gt_init(uint64_t cval)
{
  arm_gt_clock_set_compare_value(cval);
  arm_gt_clock_set_control(0x1);
}

#if defined(RTEMS_SMP) && !defined(CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR)
static void arm_gt_clock_secondary_action(void *arg)
{
  uint64_t *cval;

  cval = arg;
  arm_gt_clock_gt_init(*cval);
  bsp_interrupt_vector_enable(arm_gt_clock_instance.irq);
}
#endif

static void arm_gt_clock_secondary_initialization(uint64_t cval)
{
#if defined(RTEMS_SMP) && !defined(CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR)
  _SMP_Broadcast_action(arm_gt_clock_secondary_action, &cval);
#else
   (void) cval;
#endif
}

static void arm_gt_clock_initialize(void)
{
  uint64_t frequency;
  uint64_t us_per_tick;
  uint32_t interval;
  uint64_t cval;
  struct timecounter *tc;

  tc = &arm_gt_clock_instance.tc;
  frequency = tc->tc_frequency;
  us_per_tick = rtems_configuration_get_microseconds_per_tick();
  interval = (uint32_t) ((frequency * us_per_tick) / 1000000);
  cval = arm_gt_clock_get_count();
  cval += interval;
  arm_gt_clock_instance.interval = interval;

  arm_gt_clock_gt_init(cval);
  arm_gt_clock_secondary_initialization(cval);

  tc->tc_get_timecount = arm_gt_clock_get_timecount;
  tc->tc_counter_mask = 0xffffffff;
  tc->tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(tc);
}

uint32_t _CPU_Counter_frequency(void)
{
  return (uint32_t) arm_gt_clock_instance.tc.tc_frequency;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return (uint32_t) arm_gt_clock_get_count();
}

static void arm_gt_system_init(void)
{
#ifdef ARM_GENERIC_TIMER_SYSTEM_BASE
  volatile uint32_t *cntcr;

  cntcr = (volatile uint32_t *) ARM_GENERIC_TIMER_SYSTEM_BASE;
  *cntcr = ARM_GENERIC_TIMER_SYSTEM_CNTCR;
#endif
}

static void arm_gt_clock_early_init(void)
{
  uint32_t frequency;

  arm_gt_system_init();
  arm_gt_clock_set_control(0x3);
  arm_generic_timer_get_config(
    &frequency,
    &arm_gt_clock_instance.irq
  );

  /*
   * Used by _CPU_Counter_frequency() before arm_gt_clock_initialize() is
   * called.
   */
  arm_gt_clock_instance.tc.tc_frequency = frequency;
}

RTEMS_SYSINIT_ITEM(
  arm_gt_clock_early_init,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

#define Clock_driver_support_at_tick(arg) \
  arm_gt_clock_at_tick(arg)

#define Clock_driver_support_initialize_hardware() \
  arm_gt_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  arm_gt_clock_handler_install(isr)

/** @} */

/* Include shared source clock driver code */
#include "../../shared/dev/clock/clockimpl.h"
