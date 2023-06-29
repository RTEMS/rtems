/**
 * @file
 *
 * @ingroup RTEMSBSPsARMZynqMP
 *
 * @brief Triple Timer Counter clock functions definitions.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
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

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <peripheral_maps/xilinx_zynqmp.h>
#include <dev/clock/xttcps_hw.h>
#include <rtems/timecounter.h>

typedef struct {
  struct timecounter ttc_tc;
  uint32_t irq_match_interval;
  uint32_t tick_miss;
} ttc_clock_context;

static ttc_clock_context ttc_clock_instance = {0, };

#define TTC_REFERENCE_CLOCK 100000000

uint32_t _CPU_Counter_frequency( void )
{
  return ttc_clock_instance.ttc_tc.tc_frequency;
}

static uint32_t zynqmp_ttc_get_timecount(struct timecounter *tc)
{
  uint32_t time;
  time = XTtcPs_ReadReg(BSP_SELECTED_TTC_ADDR, XTTCPS_COUNT_VALUE_OFFSET);
  return time;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return zynqmp_ttc_get_timecount(&ttc_clock_instance.ttc_tc);
}

/**
 *  @brief Initialize the HW peripheral for clock driver
 *
 *  Clock driver is implemented by RTI module
 *
 * @retval Void
 */
static void zynqmp_ttc_clock_driver_support_initialize_hardware(void)
{

  uint32_t microsec_per_tick;
  uint16_t clock_ratio;
  uint8_t  index;
  uint32_t frequency;
  uint32_t prescaler;
  uint32_t tmp_reg_val;

  microsec_per_tick = rtems_configuration_get_microseconds_per_tick();

  /* Check the TTC is OFF before reconfiguring */
  XTtcPs_WriteReg(BSP_SELECTED_TTC_ADDR, XTTCPS_CNT_CNTRL_OFFSET,
                  /* Don't enable waveform output */
                  XTTCPS_CNT_CNTRL_DIS_MASK | XTTCPS_CNT_CNTRL_EN_WAVE_MASK);

  /* Prescaler value is 2^(N + 1)
   * Divide down the clock as much as possible while still retaining a
   * frequency that is an integer multiple of 1MHz.  This maximizes time to
   * overflow while minimizing rounding errors in 1us periods
   */
  clock_ratio = TTC_REFERENCE_CLOCK / 1000000;
  /* Search for the highest set bit.  This is effectively min(log2(ratio)) */
  for(index = sizeof(clock_ratio) * 8 - 1; index > 0; index--) {
    if((clock_ratio >> (index)) & 0x01) {
        break;
    }
  }
  if(index == 0 && (clock_ratio & 0x01) == 0) {
    /* No prescaler */
    frequency = TTC_REFERENCE_CLOCK;
    XTtcPs_WriteReg(BSP_SELECTED_TTC_ADDR, XTTCPS_CLK_CNTRL_OFFSET, 0);
  } else {
    prescaler = index - 1;
    frequency = TTC_REFERENCE_CLOCK / (1 << (prescaler + 1));
    XTtcPs_WriteReg(BSP_SELECTED_TTC_ADDR, XTTCPS_CLK_CNTRL_OFFSET,
        prescaler << XTTCPS_CLK_CNTRL_PS_VAL_SHIFT |
        XTTCPS_CLK_CNTRL_PS_EN_MASK);
  }

  /* Max out the counter interval */
  tmp_reg_val = XTTCPS_INTERVAL_VAL_MASK;
  XTtcPs_WriteReg(BSP_SELECTED_TTC_ADDR, XTTCPS_INTERVAL_VAL_OFFSET,
                  tmp_reg_val);

  /* Setup match register to generate tick IRQ */
  ttc_clock_instance.irq_match_interval =
    (uint32_t) ((frequency * microsec_per_tick) / 1000000);
  XTtcPs_WriteReg(BSP_SELECTED_TTC_ADDR, XTTCPS_MATCH_0_OFFSET,
                  ttc_clock_instance.irq_match_interval);
  /* Clear interupts (clear on read) */
  XTtcPs_ReadReg(BSP_SELECTED_TTC_ADDR, XTTCPS_ISR_OFFSET);
  /* Enable interupt for match register */
  XTtcPs_WriteReg(BSP_SELECTED_TTC_ADDR, XTTCPS_IER_OFFSET,
                  XTTCPS_IXR_MATCH_0_MASK);
  /* Configure, reset, and enable counter */
  XTtcPs_WriteReg(BSP_SELECTED_TTC_ADDR, XTTCPS_CNT_CNTRL_OFFSET,
        XTTCPS_CNT_CNTRL_EN_WAVE_MASK |  /* Don't enable waveform output */
        XTTCPS_CNT_CNTRL_RST_MASK |      /* Reset count and start counter */
        XTTCPS_CNT_CNTRL_MATCH_MASK      /* Enable match mode */
        /* Increment mode */
        /* Overflow mode */
        /* Not disabled */
        );

  /* set timecounter */
  ttc_clock_instance.ttc_tc.tc_get_timecount = zynqmp_ttc_get_timecount;
  ttc_clock_instance.ttc_tc.tc_counter_mask = XTTCPS_COUNT_VALUE_MASK;
  ttc_clock_instance.ttc_tc.tc_frequency = frequency;
  ttc_clock_instance.ttc_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&ttc_clock_instance.ttc_tc);
}

/**
 * @brief Clears interrupt source
 *
 * @retval Void
 */
static void zynqmp_ttc_clock_driver_support_at_tick( void )
{
  uint32_t irq_flags;
  uint32_t cval;
  uint32_t now;
  uint32_t delta;

  /* Get and clear interupts (clear on read) */
  irq_flags = XTtcPs_ReadReg(BSP_SELECTED_TTC_ADDR, XTTCPS_ISR_OFFSET);

  if(irq_flags & XTTCPS_IXR_MATCH_0_MASK) {
    /* Update match */
    cval = XTtcPs_ReadReg(BSP_SELECTED_TTC_ADDR, XTTCPS_MATCH_0_OFFSET);
    /* Check that the match for the next tick is in the future
     * If no, then set the match for one irq interval from now
     *   This will have the effect that your timebase will slip but
     *   won't hang waiting for the counter to wrap around.
     * If this happens during normal operation, there is a problem
     *   causing this interrupt to not be serviced quickly enough
     * If this happens during debugging, that is normal and expected
     *   because the TTC does NOT pause when the CPU is halted on a breakpoint
     */
    now = XTtcPs_ReadReg(BSP_SELECTED_TTC_ADDR, XTTCPS_COUNT_VALUE_OFFSET);
    delta = now - cval;
    if(delta > ttc_clock_instance.irq_match_interval) {
     cval = now;
     ttc_clock_instance.tick_miss++;
    }
    cval += ttc_clock_instance.irq_match_interval;
    XTtcPs_WriteReg(BSP_SELECTED_TTC_ADDR, XTTCPS_MATCH_0_OFFSET, cval);
  }
  /* Else, something is set up wrong, only match should be enabled */
}

/**
 * @brief registers RTI interrupt handler
 *
 * @param[in] Clock_isr new ISR handler
 * @param[in] Old_ticker old ISR handler (unused and type broken)
 *
 * @retval Void
 */
static void zynqmp_ttc_clock_driver_support_install_isr(
  rtems_isr_entry Clock_isr
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    BSP_SELECTED_TTC_IRQ,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

#define Clock_driver_support_at_tick \
  zynqmp_ttc_clock_driver_support_at_tick

#define Clock_driver_support_initialize_hardware \
  zynqmp_ttc_clock_driver_support_initialize_hardware

#define Clock_driver_support_install_isr(Clock_isr) \
  zynqmp_ttc_clock_driver_support_install_isr( Clock_isr )

#include "../../../shared/dev/clock/clockimpl.h"
