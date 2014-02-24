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
 * http://www.rtems.com/license/LICENSE.
 */

#include <leon.h>

#include <rtems/counter.h>

static volatile struct gptimer_regs *adev_to_gpt(struct ambapp_dev *adev)
{
  return (volatile struct gptimer_regs *) DEV_TO_APB(adev)->start;
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

void leon3_cpu_counter_initialize(void)
{
  struct ambapp_dev *adev;

  adev = (void *) ambapp_for_each(
    &ambapp_plb,
    OPTIONS_ALL | OPTIONS_APB_SLVS,
    VENDOR_GAISLER,
    GAISLER_GPTIMER,
    ambapp_find_by_idx,
    NULL
  );
  if (adev != NULL) {
    volatile struct gptimer_regs *gpt = adev_to_gpt(adev);
    unsigned prescaler = gpt->scaler_reload + 1;

    /* Assume that GRMON initialized the first GPTIMER to 1MHz */
    uint32_t frequency = 1000000;

    uint32_t max_frequency = frequency * prescaler;
    int idx = 1;

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
      unsigned min_prescaler = 8;

      gpt = adev_to_gpt(adev);

      gpt->scaler_reload = min_prescaler - 1;
      gpt->timer[0].reload = 0xffffffff;
      gpt->timer[0].ctrl = LEON3_GPTIMER_EN | LEON3_GPTIMER_RL
        | LEON3_GPTIMER_LD;

      frequency = max_frequency / min_prescaler;
      gpt_counter_initialize(gpt, 0, frequency, free_counter_difference);
    } else {
      /* Fall back to the first GPTIMER */
      gpt_counter_initialize(
        gpt,
        LEON3_CLOCK_INDEX,
        frequency,
        clock_counter_difference
      );
    }
  }
}
