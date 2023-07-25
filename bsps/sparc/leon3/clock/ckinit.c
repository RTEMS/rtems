/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This source file contains the Clock Driver implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  Copyright (C) 2014, 2018 embedded brains GmbH & Co. KG
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
#include <bsp/leon3.h>
#include <rtems/rtems/intr.h>
#include <grlib/irqamp.h>
#include <rtems/score/profiling.h>
#include <rtems/score/sparcimpl.h>
#include <rtems/timecounter.h>

#if !defined(LEON3_PLB_FREQUENCY_DEFINED_BY_GPTIMER)
#include <grlib/ambapp.h>
#endif

/* The LEON3 BSP Timer driver can rely on the Driver Manager if the
 * DrvMgr is initialized during startup. Otherwise the classic driver
 * must be used.
 *
 * The DrvMgr Clock driver is located in the shared/timer directory
 */
#ifndef RTEMS_DRVMGR_STARTUP

/* LEON3 Timer system interrupt number */
static int clkirq;

static struct timecounter leon3_tc;

static void leon3_tc_tick_default(void)
{
#if !defined(RTEMS_SMP)
  SPARC_Counter *counter;
  rtems_interrupt_level level;

  counter = &_SPARC_Counter_mutable;
  rtems_interrupt_local_disable(level);

  grlib_store_32(&LEON3_IrqCtrl_Regs->iclear, counter->pending_mask);
  counter->accumulated += counter->interval;

  rtems_interrupt_local_enable(level);
#endif

  rtems_timecounter_tick();
}

#if defined(RTEMS_PROFILING)
static void (*leon3_tc_tick)(void) = leon3_tc_tick_default;

#define IRQMP_TIMESTAMP_S1_S2 ((1U << 25) | (1U << 26))

static void leon3_tc_tick_irqmp_timestamp(void)
{
  irqamp_timestamp *irqmp_ts =
    irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs);
  uint32_t first = grlib_load_32(&irqmp_ts->itstmpas);
  uint32_t second = grlib_load_32(&irqmp_ts->itcnt);
  uint32_t control = grlib_load_32(&irqmp_ts->itstmpc);

  control |= IRQMP_TIMESTAMP_S1_S2;
  grlib_store_32(&irqmp_ts->itstmpc, control);

  _Profiling_Update_max_interrupt_delay(_Per_CPU_Get(), second - first);

  rtems_timecounter_tick();
}

static void leon3_tc_tick_irqmp_timestamp_init(void)
{
  /*
   * Ignore the first clock interrupt, since it contains the sequential system
   * initialization time.  Do the timestamp initialization on the fly.
   */

#ifdef RTEMS_SMP
  static Atomic_Uint counter = ATOMIC_INITIALIZER_UINT(0);

  bool done =
    _Atomic_Fetch_add_uint(&counter, 1, ATOMIC_ORDER_RELAXED)
      == rtems_scheduler_get_processor_maximum() - 1;
#else
  bool done = true;
#endif

  irqamp_timestamp *irqmp_ts =
    irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs);
  uint32_t ks = 1U << 5;
  uint32_t control = grlib_load_32(&irqmp_ts->itstmpc);

  control = ks | IRQMP_TIMESTAMP_S1_S2 | (unsigned int) clkirq;
  grlib_store_32(&irqmp_ts->itstmpc, control);

  if (done) {
    leon3_tc_tick = leon3_tc_tick_irqmp_timestamp;
  }

  rtems_timecounter_tick();
}
#endif /* RTEMS_PROFILING */

static void leon3_tc_do_tick(void)
{
#if defined(RTEMS_PROFILING)
  (*leon3_tc_tick)();
#else
  leon3_tc_tick_default();
#endif
}

#define Adjust_clkirq_for_node() do { clkirq += LEON3_CLOCK_INDEX; } while(0)

#define Clock_driver_support_find_timer() \
  do { \
    /* Assume timer found during BSP initialization */ \
    if (LEON3_Timer_Regs) { \
      clkirq = (grlib_load_32(&LEON3_Timer_Regs->config) & 0xf8) >> 3; \
      \
      Adjust_clkirq_for_node(); \
    } \
  } while (0)

#define Clock_driver_support_install_isr(isr) \
  bsp_clock_handler_install(isr)

static rtems_interrupt_entry leon3_clock_interrupt_entry;

static void bsp_clock_handler_install(rtems_interrupt_handler isr)
{
  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    &leon3_clock_interrupt_entry,
    isr,
    NULL,
    "Clock"
  );
  sc = rtems_interrupt_entry_install(
    clkirq,
    RTEMS_INTERRUPT_UNIQUE,
    &leon3_clock_interrupt_entry
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal(RTEMS_FATAL_SOURCE_BSP, LEON3_FATAL_CLOCK_INITIALIZATION);
  }
}

#define Clock_driver_support_set_interrupt_affinity(online_processors) \
  bsp_interrupt_set_affinity(clkirq, online_processors)

#if defined(LEON3_HAS_ASR_22_23_UP_COUNTER) || \
   defined(LEON3_PROBE_ASR_22_23_UP_COUNTER)
static void leon3_clock_use_up_counter(struct timecounter *tc)
{
  tc->tc_get_timecount = _SPARC_Get_timecount_asr23;
  tc->tc_frequency = leon3_up_counter_frequency();

#if defined(RTEMS_PROFILING)
  if (irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs) == NULL) {
    bsp_fatal(LEON3_FATAL_CLOCK_NO_IRQMP_TIMESTAMP_SUPPORT);
  }

  leon3_tc_tick = leon3_tc_tick_irqmp_timestamp_init;
#endif

  rtems_timecounter_install(tc);
}
#endif

#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)
static void leon3_clock_use_irqamp_timestamp(
  struct timecounter *tc,
  irqamp_timestamp *irqmp_ts
)
{
  tc->tc_get_timecount = _SPARC_Get_timecount_up;
#if defined(LEON3_PLB_FREQUENCY_DEFINED_BY_GPTIMER)
  tc->tc_frequency = leon3_processor_local_bus_frequency();
#else
  tc->tc_frequency = ambapp_freq_get(ambapp_plb(), LEON3_Timer_Adev);
#endif

#if defined(RTEMS_PROFILING)
  leon3_tc_tick = leon3_tc_tick_irqmp_timestamp_init;
#endif

  /*
   * At least one TSISEL field must be non-zero to enable the timestamp
   * counter.  Use an arbitrary interrupt source.
   */
  grlib_store_32(&irqmp_ts->itstmpc, IRQAMP_ITSTMPC_TSISEL(1));

  rtems_timecounter_install(tc);
}
#endif

#if !defined(LEON3_HAS_ASR_22_23_UP_COUNTER)
static void leon3_clock_use_gptimer(
  struct timecounter *tc,
  gptimer_timer *timer
)
{
#ifdef RTEMS_SMP
  /*
   * The GR712RC for example has no timestamp unit in the interrupt
   * controller.  At least on SMP configurations we must use a second timer
   * in free running mode for the timecounter.  The timer is initialized by
   * leon3_counter_initialize().
   */
  tc->tc_get_timecount = _SPARC_Get_timecount_down;
#else
  SPARC_Counter *counter;

  counter = &_SPARC_Counter_mutable;
  counter->read_isr_disabled = _SPARC_Counter_read_clock_isr_disabled;
  counter->read = _SPARC_Counter_read_clock;
  counter->counter_register = &timer->tcntval;
  counter->pending_register = &LEON3_IrqCtrl_Regs->ipend;
  counter->pending_mask = UINT32_C(1) << clkirq;
  counter->accumulated = rtems_configuration_get_microseconds_per_tick();
  counter->interval = rtems_configuration_get_microseconds_per_tick();

  tc->tc_get_timecount = _SPARC_Get_timecount_clock;
#endif

  tc->tc_frequency = LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER,

  rtems_timecounter_install(tc);
}
#endif

static void leon3_clock_initialize(void)
{
#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)
  irqamp_timestamp *irqmp_ts;
#endif
  gptimer_timer *timer;
  struct timecounter *tc;

  timer = &LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX];

  grlib_store_32(
    &timer->trldval,
    rtems_configuration_get_microseconds_per_tick() - 1
  );
  grlib_store_32(
    &timer->tctrl,
    GPTIMER_TCTRL_EN | GPTIMER_TCTRL_RS | GPTIMER_TCTRL_LD | GPTIMER_TCTRL_IE
  );

  tc = &leon3_tc;
  tc->tc_counter_mask = 0xffffffff;
  tc->tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;

#if defined(LEON3_HAS_ASR_22_23_UP_COUNTER)
  leon3_up_counter_enable();
  leon3_clock_use_up_counter(tc);
#else /* LEON3_HAS_ASR_22_23_UP_COUNTER */
#if defined(LEON3_PROBE_ASR_22_23_UP_COUNTER)
  leon3_up_counter_enable();

  if (leon3_up_counter_is_available()) {
    /* Use the LEON4 up-counter if available */
    leon3_clock_use_up_counter(tc);
    return;
  }
#endif

#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)
  irqmp_ts = irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs);

  if (irqmp_ts != NULL) {
    /* Use the interrupt controller timestamp counter if available */
    leon3_clock_use_irqamp_timestamp(tc, irqmp_ts);
    return;
  }
#endif

  leon3_clock_use_gptimer(tc, timer);
#endif /* LEON3_HAS_ASR_22_23_UP_COUNTER */
}

#define Clock_driver_support_initialize_hardware() \
  leon3_clock_initialize()

#define Clock_driver_timecounter_tick() leon3_tc_do_tick()

#define BSP_FEATURE_IRQ_EXTENSION

#include "../../../shared/dev/clock/clockimpl.h"

#endif
