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
 *  Copyright (C) 2014, 2023 embedded brains GmbH & Co. KG
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
#include <rtems/timecounter.h>

/* The LEON3 BSP Timer driver can rely on the Driver Manager if the
 * DrvMgr is initialized during startup. Otherwise the classic driver
 * must be used.
 *
 * The DrvMgr Clock driver is located in the shared/timer directory
 */
#ifndef RTEMS_DRVMGR_STARTUP

/* LEON3 Timer system interrupt number */
static int clkirq;

#if defined(RTEMS_PROFILING) && \
  (defined(LEON3_HAS_ASR_22_23_UP_COUNTER) || \
     defined(LEON3_PROBE_ASR_22_23_UP_COUNTER) || \
     defined(LEON3_IRQAMP_PROBE_TIMESTAMP))

#define LEON3_CLOCK_PROBE_IRQAMP_TIMESTAMP

#define IRQMP_TIMESTAMP_S1_S2 ((1U << 25) | (1U << 26))

static void leon3_tc_tick_default(void)
{
  rtems_timecounter_tick();
}

static void (*leon3_tc_tick)(void) = leon3_tc_tick_default;

static void leon3_tc_do_tick(void)
{
  (*leon3_tc_tick)();
}

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
#else
static void leon3_tc_do_tick(void)
{
  rtems_timecounter_tick();
}
#endif

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

static void leon3_clock_initialize(void)
{
  gptimer_timer *timer;

  timer = &LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX];

  grlib_store_32(
    &timer->trldval,
    rtems_configuration_get_microseconds_per_tick() - 1
  );
  grlib_store_32(
    &timer->tctrl,
    GPTIMER_TCTRL_EN | GPTIMER_TCTRL_RS | GPTIMER_TCTRL_LD | GPTIMER_TCTRL_IE
  );

#if defined(LEON3_CLOCK_PROBE_IRQAMP_TIMESTAMP)
  if (irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs) != NULL) {
    leon3_tc_tick = leon3_tc_tick_irqmp_timestamp_init;
  }
#endif

  rtems_timecounter_install(&leon3_timecounter_instance.base);
}

#define Clock_driver_support_initialize_hardware() \
  leon3_clock_initialize()

#define Clock_driver_timecounter_tick() leon3_tc_do_tick()

#define BSP_FEATURE_IRQ_EXTENSION

#include "../../../shared/dev/clock/clockimpl.h"

#endif /* RTEMS_DRVMGR_STARTUP */
