/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2011, 2017 embedded brains GmbH & Co. KG
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

#include <rtems/timecounter.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/irq.h>

/* This is defined in dev/clock/clockimpl.h */
static rtems_isr Clock_isr(void *arg);

static struct timecounter t32mppc_clock_tc;

#define CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR

void t32mppc_decrementer_dispatch(void)
{
  PPC_SET_SPECIAL_PURPOSE_REGISTER(BOOKE_TSR, BOOKE_TSR_DIS);
  Clock_isr(NULL);
}

static uint32_t t32mppc_clock_get_timecount(struct timecounter *tc)
{
  return ppc_time_base();
}

static void t32mppc_clock_initialize(void)
{
  uint64_t frequency = bsp_time_base_frequency / 10;
  uint32_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t interval = (uint32_t) ((frequency * us_per_tick) / 1000000);

  PPC_SET_SPECIAL_PURPOSE_REGISTER(BOOKE_DECAR, interval - 1);
  PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS(
    BOOKE_TCR,
    BOOKE_TCR_DIE | BOOKE_TCR_ARE
  );
  ppc_set_decrementer_register(interval - 1);

  t32mppc_clock_tc.tc_get_timecount = t32mppc_clock_get_timecount;
  t32mppc_clock_tc.tc_counter_mask = 0xffffffff;
  t32mppc_clock_tc.tc_frequency = bsp_time_base_frequency;
  t32mppc_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&t32mppc_clock_tc);
}

#define Clock_driver_support_initialize_hardware() \
  t32mppc_clock_initialize()

/* Include shared source clock driver code */
#include "../../../shared/dev/clock/clockimpl.h"
