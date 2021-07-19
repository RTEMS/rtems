/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2014, 2018 embedded brains GmbH & Co. KG
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

#include <bsp/leon3.h>
#include <grlib/irqamp.h>

#include <rtems/counter.h>
#include <rtems/sysinit.h>
#include <rtems/score/sparcimpl.h>

static uint32_t leon3_counter_frequency = 1000000000;

uint32_t _CPU_Counter_frequency(void)
{
  return leon3_counter_frequency;
}

static void leon3_counter_initialize(void)
{
  irqamp_timestamp *irqmp_ts;
  gptimer *gpt;
  SPARC_Counter *counter;

  irqmp_ts = irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs);
  gpt = LEON3_Timer_Regs;
  counter = &_SPARC_Counter_mutable;

  leon3_up_counter_enable();

  if (leon3_up_counter_is_available()) {
    /* Use the LEON4 up-counter if available */
    counter->read_isr_disabled = _SPARC_Counter_read_asr23;
    counter->read = _SPARC_Counter_read_asr23;

    leon3_counter_frequency = leon3_up_counter_frequency();
  } else if (irqmp_ts != NULL) {
    /* Use the interrupt controller timestamp counter if available */
    counter->read_isr_disabled = _SPARC_Counter_read_up;
    counter->read = _SPARC_Counter_read_up;
    counter->counter_register = &irqmp_ts->itcnt;

    /* Enable interrupt timestamping for an arbitrary interrupt line */
    grlib_store_32(&irqmp_ts->itstmpc, IRQAMP_ITSTMPC_TSISEL(1));

#if defined(LEON3_PLB_FREQUENCY_DEFINED_BY_GPTIMER)
    leon3_counter_frequency = leon3_processor_local_bus_frequency();
#else
    leon3_counter_frequency = ambapp_freq_get(ambapp_plb(), LEON3_IrqCtrl_Adev);
#endif
  } else if (gpt != NULL) {
    gptimer_timer *timer;
    uint32_t       tctrl;

    /* Fall back to the first GPTIMER if available */
    timer = &gpt->timer[LEON3_COUNTER_GPTIMER_INDEX];
    counter->read_isr_disabled = _SPARC_Counter_read_down;
    counter->read = _SPARC_Counter_read_down;
    counter->counter_register = &timer->tcntval;

    /* Enable timer just in case no clock driver is configured */
    grlib_store_32(&timer->trldval, 0xffffffff);
    tctrl = grlib_load_32(&timer->tctrl);
    tctrl |= GPTIMER_TCTRL_EN | GPTIMER_TCTRL_RS | GPTIMER_TCTRL_LD;
    grlib_store_32(&timer->tctrl, tctrl);

#if defined(LEON3_PLB_FREQUENCY_DEFINED_BY_GPTIMER)
    leon3_counter_frequency = LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER;
#else
    leon3_counter_frequency = ambapp_freq_get(ambapp_plb(), LEON3_Timer_Adev) /
      (grlib_load_32(&gpt->sreload) + 1);
#endif
  }
}

RTEMS_SYSINIT_ITEM(
  leon3_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

SPARC_COUNTER_DEFINITION;
