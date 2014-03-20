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

static volatile struct gptimer_regs *adev_to_gpt(struct ambapp_dev *adev)
{
  return (volatile struct gptimer_regs *) DEV_TO_APB(adev)->start;
}

static CPU_Counter_ticks timestamp_counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

static CPU_Counter_ticks free_counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return first - second;
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

static unsigned int get_tstamp(volatile struct irqmp_timestamp_regs *irqmp_ts)
{
  return irqmp_ts->control >> 27;
}

void leon3_cpu_counter_initialize(void)
{
  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &LEON3_IrqCtrl_Regs->timestamp[0];
  struct ambapp_dev *adev;
  int idx = 1;

  if (leon3_irqmp_has_timestamp(irqmp_ts)) {
    /* Use the interrupt controller timestamp counter if available */

    /* Enable interrupt timestamping for an arbitrary interrupt line */
    irqmp_ts->control = 0x1;

    _SPARC_Counter_initialize(
      (volatile const uint32_t *) &irqmp_ts->counter,
      timestamp_counter_difference
    );

    /* Get and set the frequency */
    adev = (void *) ambapp_for_each(
      &ambapp_plb,
      OPTIONS_ALL | OPTIONS_APB_SLVS,
      VENDOR_GAISLER,
      GAISLER_IRQMP,
      ambapp_find_by_idx,
      NULL
    );
    rtems_counter_initialize_converter(ambapp_freq_get(&ambapp_plb, adev));
  } else {
    adev = (void *) ambapp_for_each(
      &ambapp_plb,
      OPTIONS_ALL | OPTIONS_APB_SLVS,
      VENDOR_GAISLER,
      GAISLER_GPTIMER,
      ambapp_find_by_idx,
      &idx
    );
    if (adev != NULL) {
      /* Use the second GPTIMER if available */
      volatile struct gptimer_regs *gpt = adev_to_gpt(adev);
      uint32_t max_frequency = ambapp_freq_get(&ambapp_plb, adev);
      unsigned min_prescaler = 8;
      uint32_t frequency = max_frequency / min_prescaler;

      gpt->scaler_reload = min_prescaler - 1;
      gpt->timer[0].reload = 0xffffffff;
      gpt->timer[0].ctrl = LEON3_GPTIMER_EN | LEON3_GPTIMER_RL
        | LEON3_GPTIMER_LD;

      gpt_counter_initialize(gpt, 0, frequency, free_counter_difference);
    } else if (LEON3_Timer_Regs != NULL) {
      /* Fall back to the first GPTIMER if available */
      gpt_counter_initialize(
        LEON3_Timer_Regs,
        LEON3_CLOCK_INDEX,
        LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER,
        clock_counter_difference
      );
    }
  }
}
