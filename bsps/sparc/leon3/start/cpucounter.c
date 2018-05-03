/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
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

#include <leon.h>

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
  volatile struct irqmp_timestamp_regs *irqmp_ts;
  volatile struct gptimer_regs *gpt;
  SPARC_Counter *counter;

  irqmp_ts = &LEON3_IrqCtrl_Regs->timestamp[0];
  gpt = LEON3_Timer_Regs;
  counter = &_SPARC_Counter_mutable;

  leon3_up_counter_enable();

  if (leon3_up_counter_is_available()) {
    /* Use the LEON4 up-counter if available */
    counter->read_isr_disabled = _SPARC_Counter_read_asr23;
    counter->read = _SPARC_Counter_read_asr23;

    leon3_counter_frequency = leon3_up_counter_frequency();
  } else if (leon3_irqmp_has_timestamp(irqmp_ts)) {
    /* Use the interrupt controller timestamp counter if available */
    counter->read_isr_disabled = _SPARC_Counter_read_up;
    counter->read = _SPARC_Counter_read_up;
    counter->counter_register = &LEON3_IrqCtrl_Regs->timestamp[0].counter;

    /* Enable interrupt timestamping for an arbitrary interrupt line */
    irqmp_ts->control = 0x1;

    leon3_counter_frequency = ambapp_freq_get(&ambapp_plb, LEON3_IrqCtrl_Adev);
  } else if (gpt != NULL) {
    /* Fall back to the first GPTIMER if available */
    counter->read_isr_disabled = _SPARC_Counter_read_down;
    counter->read = _SPARC_Counter_read_down;
    counter->counter_register = &gpt->timer[LEON3_COUNTER_GPTIMER_INDEX].value;

    /* Enable timer just in case no clock driver is configured */
    gpt->timer[LEON3_COUNTER_GPTIMER_INDEX].ctrl |= GPTIMER_TIMER_CTRL_EN;

    leon3_counter_frequency = ambapp_freq_get(&ambapp_plb, LEON3_Timer_Adev) /
      (gpt->scaler_reload + 1);
  }
}

RTEMS_SYSINIT_ITEM(
  leon3_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

SPARC_COUNTER_DEFINITION;
