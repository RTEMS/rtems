/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 2011, 2018 Sebastian Huber
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

void _ARMV7M_Clock_handler(void)
{
  _ARMV7M_Interrupt_service_enter();
  Clock_isr(NULL);
  _ARMV7M_Interrupt_service_leave();
}

static void _ARMV7M_Clock_handler_install(void)
{
  _ARMV7M_Set_exception_priority(
    ARMV7M_VECTOR_SYSTICK,
    BSP_ARMV7M_SYSTICK_PRIORITY
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
