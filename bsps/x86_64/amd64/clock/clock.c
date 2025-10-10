/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief APIC timer clock implementation
 */

/*
 * Copyright (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <assert.h>
#include <bsp.h>
#include <rtems.h>
#include <apic.h>
#include <clock.h>
#include <rtems/score/idt.h>
#include <rtems/timecounter.h>
#include <rtems/score/cpu.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/x86_64.h>
#include <bsp/irq-generic.h>
#include <rtems/score/smpimpl.h>

static struct timecounter amd64_clock_tc;

extern volatile uint32_t Clock_driver_ticks;
extern void Clock_isr(void *param);

static uint32_t amd64_clock_get_timecount(struct timecounter *tc)
{
  (void) tc;

  return Clock_driver_ticks;
}

static void lapic_timer_isr(void *param)
{
  Clock_isr(param);
  lapic_eoi();
}

#ifdef RTEMS_SMP
static void smp_lapic_timer_enable(void* arg)
{
  uint32_t* lapic_reload_value = (uint32_t*) arg;
  lapic_timer_enable(*lapic_reload_value);
}
#endif

void lapic_timer_install_handler(void)
{
  rtems_status_code sc = rtems_interrupt_handler_install(
    BSP_VECTOR_APIC_TIMER,
    "LAPIC timer",
    RTEMS_INTERRUPT_UNIQUE,
    lapic_timer_isr,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

void amd64_clock_driver_initialize(void)
{
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint64_t irq_ticks_per_sec = 1000000 / us_per_tick;
  DBG_PRINTF(
    "us_per_tick = %" PRIu64 "\nDesired frequency = %" PRIu64 "irqs/sec\n",
    us_per_tick,
    irq_ticks_per_sec
  );

  /* Setup and initialize the Local APIC timer */
  uint32_t lapic_reload_value = lapic_timer_calc_ticks(irq_ticks_per_sec);
#ifdef RTEMS_SMP
  _SMP_Broadcast_action(smp_lapic_timer_enable, &lapic_reload_value);
#else
  lapic_timer_enable(lapic_reload_value);
#endif

  amd64_clock_tc.tc_get_timecount = amd64_clock_get_timecount;
  amd64_clock_tc.tc_counter_mask = 0xffffffff;
  amd64_clock_tc.tc_frequency = irq_ticks_per_sec;
  amd64_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&amd64_clock_tc);
}

#define Clock_driver_support_install_isr(_new) \
  lapic_timer_install_handler()

#define Clock_driver_support_initialize_hardware() \
  amd64_clock_driver_initialize()

#include "../../../shared/dev/clock/clockimpl.h"
