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

#include <bsp.h>
#include <bsp/fatal.h>
#include <leon.h>

#include <rtems/counter.h>

static volatile struct gptimer_regs *leon3_cpu_counter_gpt;

void leon3_cpu_counter_initialize(void)
{
  struct ambapp_dev *adev;
  int idx = 1;
  volatile struct gptimer_regs *gpt;
  unsigned new_prescaler = 8;
  unsigned prescaler;
  uint32_t frequency;

  adev = (void *) ambapp_for_each(
    &ambapp_plb,
    OPTIONS_ALL | OPTIONS_APB_SLVS,
    VENDOR_GAISLER,
    GAISLER_GPTIMER,
    ambapp_find_by_idx,
    &idx
  );
  if (adev == NULL) {
    bsp_fatal(LEON3_FATAL_CPU_COUNTER_INIT);
  }

  gpt = (volatile struct gptimer_regs *) DEV_TO_APB(adev)->start;

  prescaler = gpt->scaler_reload + 1;
  gpt->scaler_reload = new_prescaler - 1;
  gpt->timer[0].reload = 0xffffffff;
  gpt->timer[0].ctrl = LEON3_GPTIMER_EN | LEON3_GPTIMER_RL
    | LEON3_GPTIMER_LD;

  leon3_cpu_counter_gpt = gpt;

  /* Assume that GRMON initialized the timer to 1MHz */
  frequency = UINT32_C(1000000) * (prescaler / new_prescaler);
  rtems_counter_initialize_converter(frequency);
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  volatile struct gptimer_regs *gpt = leon3_cpu_counter_gpt;

  return 0U - gpt->timer[0].value;
}
