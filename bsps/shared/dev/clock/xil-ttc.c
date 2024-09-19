/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDriverClockXilTTC
 *
 * @brief This source file contains a Clock Driver implementation using the
 *   Xilinx Triple Timer Counter (TTC).
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
 * Copyright (C) 2023 Reflex Aerospace GmbH
 *
 * Written by Philip Kirkpatrick <p.kirkpatrick@reflexaerospace.com>
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
#include <bsp/irq-generic.h>
#include <bsp/fatal.h>
#include <dev/clock/xttcps_hw.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>
#include <rtems/score/processormaskimpl.h>

#if XTTCPS_COUNT_VALUE_MASK != UINT32_MAX
#error "unexpected XTTCPS_COUNT_VALUE_MASK value"
#endif

/**
 * @defgroup RTEMSDriverClockXilTTC \
 *   Xilinx Triple Timer Counter (TTC) Clock Driver
 *
 * @ingroup RTEMSDriverClockImpl
 *
 * @brief This group contains the Xilinx Triple Timer Counter (TTC) Clock
 *   Driver implementation.
 *
 * @{
 */

uint32_t _CPU_Counter_frequency( void )
{
  return XIL_CLOCK_TTC_REFERENCE_CLOCK;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_COUNT_VALUE_OFFSET);
}

static void xil_ttc_initialize(void)
{
  /* Do not use a prescaler to get a high resolution time source */
  XTtcPs_WriteReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_CLK_CNTRL_OFFSET, 0);

  /* Disable interupts */
  XTtcPs_WriteReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_IER_OFFSET, 0);

  /*
   * Enable the timer, do not enable waveform output, increment up, use
   * overflow mode, enable match mode.
   */
  XTtcPs_WriteReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_CNT_CNTRL_OFFSET,
                  XTTCPS_CNT_CNTRL_EN_WAVE_MASK | XTTCPS_CNT_CNTRL_MATCH_MASK);
}

RTEMS_SYSINIT_ITEM(
  xil_ttc_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

typedef struct {
  struct timecounter base;
  uint32_t irq_match_interval;
} xil_ttc_timecounter;

static xil_ttc_timecounter xil_ttc_clock_instance;

static uint32_t xil_ttc_get_timecount(struct timecounter *tc)
{
  (void) tc;
  return XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_COUNT_VALUE_OFFSET);
}

static void xil_ttc_clock_driver_support_initialize_hardware(void)
{
  xil_ttc_timecounter *tc;
  uint64_t frequency;
  uint32_t irq_match_interval;
  uint32_t count;

  tc = &xil_ttc_clock_instance;
  frequency = XIL_CLOCK_TTC_REFERENCE_CLOCK;
  irq_match_interval = (uint32_t)
    ((frequency * rtems_configuration_get_microseconds_per_tick()) / 1000000);

  /* Setup match register to generate clock interrupts */
  count = XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_COUNT_VALUE_OFFSET);
  XTtcPs_WriteReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_MATCH_0_OFFSET,
                  count + irq_match_interval);

  /* Clear interupts (clear on read) */
  (void) XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_ISR_OFFSET);

  /* Enable interupt for match register */
  XTtcPs_WriteReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_IER_OFFSET,
                  XTTCPS_IXR_MATCH_0_MASK);

  /* Install timecounter */
  tc->irq_match_interval = irq_match_interval;
  tc->base.tc_counter_mask = UINT32_MAX;
  tc->base.tc_frequency = XIL_CLOCK_TTC_REFERENCE_CLOCK;
  tc->base.tc_get_timecount = xil_ttc_get_timecount;
  tc->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&tc->base);
}

static void xil_ttc_clock_driver_support_at_tick(xil_ttc_timecounter *tc)
{
  uint32_t irq_match_interval;
  uint32_t count;
  uint32_t match;

  irq_match_interval = tc->irq_match_interval;

  /* Update match register */
  match = XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_MATCH_0_OFFSET);
  match += irq_match_interval;
  XTtcPs_WriteReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_MATCH_0_OFFSET, match);

  /* Clear interupts (clear on read) */
  (void) XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_ISR_OFFSET);

  /* Check that the new match value is in the future */
  count = XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_COUNT_VALUE_OFFSET);

  while (RTEMS_PREDICT_FALSE(match - count > irq_match_interval)) {
    /*
     * Tick misses may happen if interrupts are disabled for an extremly long
     * period or while debugging.
     */
    rtems_timecounter_tick();

    /* Update match register */
    match += irq_match_interval;
    XTtcPs_WriteReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_MATCH_0_OFFSET, match);

    /* Clear interupts (clear on read) */
    (void) XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_ISR_OFFSET);

    /* Maybe the new match value is now in the future */
    count = XTtcPs_ReadReg(XIL_CLOCK_TTC_BASE_ADDR, XTTCPS_COUNT_VALUE_OFFSET);
  }
}

static rtems_interrupt_entry xil_ttc_interrupt_entry;

static void xil_ttc_clock_driver_support_install_isr(
  rtems_interrupt_handler handler
)
{
  rtems_status_code sc;

#if !defined(ZYNQMP_RPU_LOCK_STEP_MODE) && ZYNQMP_RPU_SPLIT_INDEX != 0
  Processor_mask affinity;
  _Processor_mask_From_uint32_t(
    &affinity,
    UINT32_C(1) << ZYNQMP_RPU_SPLIT_INDEX,
    0
  );
  (void) bsp_interrupt_set_affinity(XIL_CLOCK_TTC_IRQ, &affinity);
#endif

  rtems_interrupt_entry_initialize(
    &xil_ttc_interrupt_entry,
    handler,
    &xil_ttc_clock_instance,
    "Clock"
  );
  sc = rtems_interrupt_entry_install(
    XIL_CLOCK_TTC_IRQ,
    RTEMS_INTERRUPT_UNIQUE,
    &xil_ttc_interrupt_entry
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    bsp_fatal(XIL_FATAL_TTC_IRQ_INSTALL);
  }
}

#define Clock_driver_support_at_tick(arg) \
  xil_ttc_clock_driver_support_at_tick(arg)

#define Clock_driver_support_initialize_hardware \
  xil_ttc_clock_driver_support_initialize_hardware

#define Clock_driver_support_install_isr(isr) \
  xil_ttc_clock_driver_support_install_isr(isr)

/** @} */

#include "../../../shared/dev/clock/clockimpl.h"
