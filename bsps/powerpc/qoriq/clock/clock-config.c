/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief QorIQ clock configuration.
 */

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
#include <bsp/qoriq.h>
#include <bsp/irq.h>

/* This is defined in dev/clock/clockimpl.h */
static rtems_isr Clock_isr(void *arg);

static struct timecounter qoriq_clock_tc;

#ifdef QORIQ_IS_HYPERVISOR_GUEST

#define CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR

void qoriq_decrementer_dispatch(void)
{
  PPC_SET_SPECIAL_PURPOSE_REGISTER(BOOKE_TSR, BOOKE_TSR_DIS);
  Clock_isr(NULL);
}

static uint32_t qoriq_clock_get_timecount(struct timecounter *tc)
{
  return ppc_alternate_time_base();
}

static void qoriq_clock_initialize(void)
{
  uint64_t frequency = bsp_time_base_frequency;
  uint32_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t interval = (uint32_t) ((frequency * us_per_tick) / 1000000);

  PPC_SET_SPECIAL_PURPOSE_REGISTER(BOOKE_DECAR, interval - 1);
  PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS(
    BOOKE_TCR,
    BOOKE_TCR_DIE | BOOKE_TCR_ARE
  );
  ppc_set_decrementer_register(interval - 1);

  qoriq_clock_tc.tc_get_timecount = qoriq_clock_get_timecount;
  qoriq_clock_tc.tc_counter_mask = 0xffffffff;
  qoriq_clock_tc.tc_frequency = qoriq_clock_frequency;
  qoriq_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&qoriq_clock_tc);
}

#else /* !QORIQ_IS_HYPERVISOR_GUEST */

static volatile qoriq_pic_global_timer *const qoriq_clock =
  #if QORIQ_CLOCK_TIMER < 4
    &qoriq.pic.gta [QORIQ_CLOCK_TIMER];
  #else
    &qoriq.pic.gtb [QORIQ_CLOCK_TIMER - 4];
  #endif

static volatile qoriq_pic_global_timer *const qoriq_timecounter =
  #if QORIQ_CLOCK_TIMECOUNTER < 4
    &qoriq.pic.gta [QORIQ_CLOCK_TIMECOUNTER];
  #else
    &qoriq.pic.gtb [QORIQ_CLOCK_TIMECOUNTER - 4];
  #endif

#define CLOCK_INTERRUPT (QORIQ_IRQ_GT_BASE + QORIQ_CLOCK_TIMER)

static void qoriq_clock_handler_install(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING) && !defined(RTEMS_SMP)
  {
    Processor_mask affinity;

    _Processor_mask_From_index(&affinity, ppc_processor_id());
    bsp_interrupt_set_affinity(CLOCK_INTERRUPT, &affinity);
  }
#endif

  sc = qoriq_pic_set_priority(
    CLOCK_INTERRUPT,
    QORIQ_PIC_PRIORITY_LOWEST,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  sc = rtems_interrupt_handler_install(
    CLOCK_INTERRUPT,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static uint32_t qoriq_clock_get_timecount(struct timecounter *tc)
{
  uint32_t ccr = qoriq_timecounter->ccr;

  return GTCCR_COUNT_GET(-ccr);
}

static void qoriq_clock_initialize(void)
{
  uint32_t timer_frequency = BSP_bus_frequency / 8;
  uint32_t interval = (uint32_t) (((uint64_t) timer_frequency
    * (uint64_t) rtems_configuration_get_microseconds_per_tick()) / 1000000);

  qoriq_clock->bcr = GTBCR_COUNT(interval - 1);

  qoriq_timecounter->bcr = GTBCR_COUNT(0xffffffff);

  qoriq_clock_tc.tc_get_timecount = qoriq_clock_get_timecount;
  qoriq_clock_tc.tc_counter_mask = GTCCR_COUNT_GET(0xffffffff);
  qoriq_clock_tc.tc_frequency = timer_frequency;
  qoriq_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&qoriq_clock_tc);
}

#define Clock_driver_support_install_isr(clock_isr) \
  qoriq_clock_handler_install()

#define Clock_driver_support_set_interrupt_affinity(online_processors) \
  bsp_interrupt_set_affinity(CLOCK_INTERRUPT, online_processors)

#endif /* QORIQ_IS_HYPERVISOR_GUEST */

#define Clock_driver_support_initialize_hardware() \
  qoriq_clock_initialize()

/* Include shared source clock driver code */
#include "../../../shared/dev/clock/clockimpl.h"
