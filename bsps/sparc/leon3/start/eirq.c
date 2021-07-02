/*
 *  GRLIB/LEON3 extended interrupt controller
 *
 *  Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#include <leon.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

/* GRLIB extended IRQ controller IRQ number */
int LEON3_IrqCtrl_EIrq = -1;

/* Initialize Extended Interrupt controller */
void leon3_ext_irq_init(void)
{
  if ( (LEON3_IrqCtrl_Regs->mpstat >> 16) & 0xf ) {
    /* Extended IRQ controller available */
    LEON3_IrqCtrl_EIrq = (LEON3_IrqCtrl_Regs->mpstat >> 16) & 0xf;
  }
}

bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  if (vector == 0) {
    return false;
  }

  if (LEON3_IrqCtrl_EIrq > 0) {
    return vector <= BSP_INTERRUPT_VECTOR_MAX_EXT;
  }

  return vector <= BSP_INTERRUPT_VECTOR_MAX_STD;
}

#if defined(RTEMS_SMP)
Processor_mask leon3_interrupt_affinities[BSP_INTERRUPT_VECTOR_MAX_STD + 1];
#endif

rtems_status_code bsp_interrupt_facility_initialize(void)
{
#if defined(RTEMS_SMP)
  Processor_mask affinity;
  size_t i;

  _Processor_mask_From_index(&affinity, rtems_scheduler_get_processor());

  for (i = 0; i < RTEMS_ARRAY_SIZE(leon3_interrupt_affinities); ++i) {
    leon3_interrupt_affinities[i] = affinity;
  }
#endif

  return RTEMS_SUCCESSFUL;
}

#if defined(RTEMS_SMP)
static void leon3_interrupt_vector_enable(rtems_vector_number vector)
{
  uint32_t cpu_index;
  uint32_t cpu_count;
  Processor_mask affinity;
  uint32_t bit;
  uint32_t unmasked;

  cpu_count = rtems_scheduler_get_processor_maximum();
  affinity = leon3_interrupt_affinities[vector];
  bit = 1U << vector;
  unmasked = 0;

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    uint32_t mask;

    mask = LEON3_IrqCtrl_Regs->mask[cpu_index];

    if (_Processor_mask_Is_set(&affinity, cpu_index)) {
      ++unmasked;
      mask |= bit;
    } else {
      mask &= ~bit;
    }

    LEON3_IrqCtrl_Regs->mask[cpu_index] = mask;
  }

  if (unmasked > 1) {
    LEON3_IrqCtrl_Regs->bcast |= bit;
  } else {
    LEON3_IrqCtrl_Regs->bcast &= ~bit;
  }
}
#endif

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
#if defined(RTEMS_SMP)
  rtems_interrupt_lock_context lock_context;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  LEON3_IRQCTRL_ACQUIRE(&lock_context);
  leon3_interrupt_vector_enable(vector);
  LEON3_IRQCTRL_RELEASE(&lock_context);
#else
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_Cpu_Unmask_interrupt(vector, _LEON3_Get_current_processor());
#endif
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
#if defined(RTEMS_SMP)
  rtems_interrupt_lock_context lock_context;
  uint32_t bit;
  uint32_t cpu_index;
  uint32_t cpu_count;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bit = 1U << vector;
  cpu_count = rtems_scheduler_get_processor_maximum();

  LEON3_IRQCTRL_ACQUIRE(&lock_context);

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    LEON3_IrqCtrl_Regs->mask[cpu_index] &= ~bit;
  }

  LEON3_IrqCtrl_Regs->bcast &= ~bit;

  LEON3_IRQCTRL_RELEASE(&lock_context);
#else
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BSP_Cpu_Mask_interrupt(vector, _LEON3_Get_current_processor());
#endif
}

#if defined(RTEMS_SMP)
void bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
)
{
  rtems_interrupt_lock_context lock_context;
  uint32_t cpu_count;
  uint32_t cpu_index;
  uint32_t bit;

  cpu_count = rtems_scheduler_get_processor_maximum();
  bit = 1U << vector;

  LEON3_IRQCTRL_ACQUIRE(&lock_context);
  leon3_interrupt_affinities[vector] = *affinity;

  /*
   * If the interrupt is enabled on at least one processor, then re-enable it
   * using the new affinity.
   */
  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    if ((LEON3_IrqCtrl_Regs->mask[cpu_index] & bit) != 0) {
      leon3_interrupt_vector_enable(vector);
      break;
    }
  }

  LEON3_IRQCTRL_RELEASE(&lock_context);
}

void bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
)
{
  *affinity = leon3_interrupt_affinities[vector];
}
#endif
