/*
 * Copyright (c) 2011, 2018 Sebastian Huber.  All rights reserved.
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

#include <bsp/clock-armv7m.h>

#include <rtems.h>
#include <rtems/sysinit.h>

#ifdef ARM_MULTILIB_ARCH_V7M

/* This is defined in dev/clock/clockimpl.h */
static void Clock_isr(void *arg);

ARMV7M_Timecounter _ARMV7M_TC;

static uint32_t _ARMV7M_TC_get_timecount(struct timecounter *base)
{
  return _ARMV7M_Clock_counter((ARMV7M_Timecounter *) base);
}

static void _ARMV7M_Clock_handler(void)
{
  _ARMV7M_Interrupt_service_enter();
  Clock_isr(NULL);
  _ARMV7M_Interrupt_service_leave();
}

static void _ARMV7M_Clock_handler_install(void)
{
  _ARMV7M_Set_exception_priority_and_handler(
    ARMV7M_VECTOR_SYSTICK,
    BSP_ARMV7M_SYSTICK_PRIORITY,
    _ARMV7M_Clock_handler
  );
}

static void _ARMV7M_Clock_initialize(void)
{
  volatile ARMV7M_Systick *systick;
  ARMV7M_Timecounter *tc;

  systick = _ARMV7M_Systick;
  tc = &_ARMV7M_TC;

  systick->csr = ARMV7M_SYSTICK_CSR_ENABLE
    | ARMV7M_SYSTICK_CSR_TICKINT
    | ARMV7M_SYSTICK_CSR_CLKSOURCE;

  tc->base.tc_get_timecount = _ARMV7M_TC_get_timecount;
  tc->base.tc_counter_mask = 0xffffffff;
  tc->base.tc_frequency = _ARMV7M_Clock_frequency();
  tc->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&tc->base);
}

static void _ARMV7M_Clock_initialize_early(void)
{
  volatile ARMV7M_Systick *systick;
  uint32_t us_per_tick;
  uint64_t freq;
  uint32_t interval;

  systick = _ARMV7M_Systick;
  us_per_tick = rtems_configuration_get_microseconds_per_tick();
  freq = _ARMV7M_Clock_frequency();

  interval = (uint32_t) ((freq * us_per_tick) / 1000000);

  systick->rvr = interval;
  systick->cvr = 0;
  systick->csr = ARMV7M_SYSTICK_CSR_ENABLE | ARMV7M_SYSTICK_CSR_CLKSOURCE;
}

RTEMS_SYSINIT_ITEM(
  _ARMV7M_Clock_initialize_early,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

#define Clock_driver_support_initialize_hardware() \
  _ARMV7M_Clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  _ARMV7M_Clock_handler_install()

/* Include shared source clock driver code */
#include "../../../shared/dev/clock/clockimpl.h"

#endif /* ARM_MULTILIB_ARCH_V7M */
