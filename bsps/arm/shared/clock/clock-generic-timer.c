/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#include <rtems/counter.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>
#include <rtems/score/smpimpl.h>

#include <libcpu/arm-cp15.h>

/*
 * Clock driver using the ARMv7-AR Generic Timer.  The BSP must provide the
 * following function via <bsp.h>:
 *
 * void arm_generic_timer_get_config(uint32_t *frequency, uint32_t *irq);
 *
 * The BSP may optionally define ARM_GENERIC_TIMER_USE_VIRTUAL in <bsp.h> to
 * use the virtual timer instead of the physical timer.
 */

typedef struct {
  struct timecounter tc;
  uint32_t interval;
  rtems_vector_number irq;
} arm_gt_clock_context;

static arm_gt_clock_context arm_gt_clock_instance;

/* This is defined in dev/clock/clockimpl.h */
void Clock_isr(rtems_irq_hdl_param arg);

static inline uint64_t arm_gt_clock_get_compare_value(void)
{
#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  return arm_cp15_get_counter_pl1_virtual_compare_value();
#else
  return arm_cp15_get_counter_pl1_physical_compare_value();
#endif
}

static inline void arm_gt_clock_set_compare_value(uint64_t cval)
{
#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  arm_cp15_set_counter_pl1_virtual_compare_value(cval);
#else
  arm_cp15_set_counter_pl1_physical_compare_value(cval);
#endif
}

static inline uint64_t arm_gt_clock_get_count(void)
{
#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  return arm_cp15_get_counter_virtual_count();
#else
  return arm_cp15_get_counter_physical_count();
#endif
}

static inline void arm_gt_clock_set_control(uint32_t ctl)
{
#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  arm_cp15_set_counter_pl1_virtual_timer_control(ctl);
#else
  arm_cp15_set_counter_pl1_physical_timer_control(ctl);
#endif
}

static void arm_gt_clock_at_tick(void)
{
  uint64_t cval;
  uint32_t interval;

  interval = arm_gt_clock_instance.interval;
  cval = arm_gt_clock_get_compare_value();
  cval += interval;
  arm_gt_clock_set_compare_value(cval);
}

static void arm_gt_clock_handler_install(void)
{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    arm_gt_clock_instance.irq,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(BSP_ARM_FATAL_GENERIC_TIMER_CLOCK_IRQ_INSTALL);
  }
}

static uint32_t arm_gt_clock_get_timecount(struct timecounter *tc)
{
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
#endif
}

static void arm_gt_clock_initialize(void)
{
  uint64_t frequency;
  uint64_t us_per_tick;
  uint32_t interval;
  uint64_t cval;
  struct timecounter *tc;

  frequency = arm_gt_clock_instance.interval;
  us_per_tick = rtems_configuration_get_microseconds_per_tick();
  interval = (uint32_t) ((frequency * us_per_tick) / 1000000);
  cval = arm_gt_clock_get_count();
  cval += interval;
  arm_gt_clock_instance.interval = interval;

  arm_gt_clock_gt_init(cval);
  arm_gt_clock_secondary_initialization(cval);

  tc = &arm_gt_clock_instance.tc;
  tc->tc_get_timecount = arm_gt_clock_get_timecount;
  tc->tc_counter_mask = 0xffffffff;
  tc->tc_frequency = frequency;
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

static void arm_gt_clock_early_init(void)
{
  arm_gt_clock_set_control(0x3);

  arm_generic_timer_get_config(
    &arm_gt_clock_instance.interval,
    &arm_gt_clock_instance.irq
  );
}

RTEMS_SYSINIT_ITEM(
  arm_gt_clock_early_init,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

#define Clock_driver_support_at_tick() \
  arm_gt_clock_at_tick()

#define Clock_driver_support_initialize_hardware() \
  arm_gt_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  arm_gt_clock_handler_install()

/* Include shared source clock driver code */
#include "../../shared/dev/clock/clockimpl.h"
