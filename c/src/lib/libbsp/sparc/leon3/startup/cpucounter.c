/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

static CPU_Counter_ticks timestamp_counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

static CPU_Counter_ticks clock_counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  CPU_Counter_ticks period = rtems_configuration_get_microseconds_per_tick();

  return (first + period - second) % period;
}

static void gpt_counter_initialize(
  volatile struct gptimer_regs *gpt,
  size_t timer_index,
  uint32_t frequency,
  SPARC_Counter_difference counter_difference
)
{
  _SPARC_Counter_initialize(
    (volatile const uint32_t *) &gpt->timer[timer_index].value,
    counter_difference
  );

  rtems_counter_initialize_converter(frequency);
}

void leon3_cpu_counter_initialize(void)
{
  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &LEON3_IrqCtrl_Regs->timestamp[0];
  unsigned int freq;

  if (leon3_irqmp_has_timestamp(irqmp_ts)) {
    /* Use the interrupt controller timestamp counter if available */

    /* Enable interrupt timestamping for an arbitrary interrupt line */
    irqmp_ts->control = 0x1;

    _SPARC_Counter_initialize(
      (volatile const uint32_t *) &irqmp_ts->counter,
      timestamp_counter_difference
    );

    /* Get and set the frequency */
    rtems_counter_initialize_converter(
      ambapp_freq_get(&ambapp_plb, LEON3_IrqCtrl_Adev));
  } else if (LEON3_Timer_Regs != NULL) {
      /* Fall back to the first GPTIMER if available */
      freq = ambapp_freq_get(&ambapp_plb, LEON3_Timer_Adev);

      gpt_counter_initialize(
        LEON3_Timer_Regs,
        LEON3_CLOCK_INDEX,
        freq / (LEON3_Timer_Regs->scaler_reload - 1),
        clock_counter_difference
      );
  }
}
