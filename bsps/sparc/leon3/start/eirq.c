/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  GRLIB/LEON3 extended interrupt controller
 *
 *  Copyright (C) 2021 embedded brains GmbH & Co. KG
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
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

void bsp_interrupt_facility_initialize(void)
{
#if defined(RTEMS_SMP)
  Processor_mask affinity;
  size_t i;

  _Processor_mask_From_index(&affinity, rtems_scheduler_get_processor());

  for (i = 0; i < RTEMS_ARRAY_SIZE(leon3_interrupt_affinities); ++i) {
    leon3_interrupt_affinities[i] = affinity;
  }
#endif
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  bool is_standard_interrupt;

  is_standard_interrupt = (vector <= BSP_INTERRUPT_VECTOR_MAX_STD);
  attributes->is_maskable = (vector != 15);
  attributes->can_enable = true;
  attributes->maybe_enable = true;
  attributes->can_disable = true;
  attributes->maybe_disable = true;
  attributes->can_raise = true;
  attributes->can_raise_on = is_standard_interrupt;
  attributes->can_clear = true;
  attributes->cleared_by_acknowledge = true;
  attributes->can_get_affinity = is_standard_interrupt;
  attributes->can_set_affinity = is_standard_interrupt;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  rtems_interrupt_level level;
  uint32_t bit;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  bit = 1U << vector;

  rtems_interrupt_local_disable(level);
  *pending = (LEON3_IrqCtrl_Regs->ipend & bit) != 0 ||
    (LEON3_IrqCtrl_Regs->force[rtems_scheduler_get_processor()] & bit) != 0;
  rtems_interrupt_local_enable(level);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  uint32_t bit;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bit = 1U << vector;

  if ( vector <= BSP_INTERRUPT_VECTOR_MAX_STD ) {
    uint32_t cpu_count;
    uint32_t cpu_index;

    cpu_count = rtems_scheduler_get_processor_maximum();

    for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
      LEON3_IrqCtrl_Regs->force[cpu_index] = bit;
    }
  } else {
    rtems_interrupt_lock_context lock_context;

    /*
     * This is a very dangerous operation and should only be used for test
     * software.  We may accidentally clear the pending state set by
     * peripherals with this read-modify-write operation.
     */
    LEON3_IRQCTRL_ACQUIRE(&lock_context);
    LEON3_IrqCtrl_Regs->ipend |= bit;
    LEON3_IRQCTRL_RELEASE(&lock_context);
  }

  return RTEMS_SUCCESSFUL;
}

#if defined(RTEMS_SMP)
rtems_status_code bsp_interrupt_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(cpu_index < rtems_scheduler_get_processor_maximum());

  if ( vector > BSP_INTERRUPT_VECTOR_MAX_STD ) {
    return RTEMS_UNSATISFIED;
  }

  LEON3_IrqCtrl_Regs->force[cpu_index] = 1U << vector;
  return RTEMS_SUCCESSFUL;
}
#endif

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  uint32_t bit;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bit = 1U << vector;

  LEON3_IrqCtrl_Regs->iclear = bit;

  if (vector <= BSP_INTERRUPT_VECTOR_MAX_STD) {
    LEON3_IrqCtrl_Regs->force[rtems_scheduler_get_processor()] = bit << 16;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  *enabled =
    !BSP_Cpu_Is_interrupt_masked(vector, _LEON3_Get_current_processor());
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

  if (vector <= BSP_INTERRUPT_VECTOR_MAX_STD) {
    affinity = leon3_interrupt_affinities[vector];
  } else {
    affinity = leon3_interrupt_affinities[LEON3_IrqCtrl_EIrq];
  }

  cpu_count = rtems_scheduler_get_processor_maximum();
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

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
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
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
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
  return RTEMS_SUCCESSFUL;
}

#if defined(RTEMS_SMP)
rtems_status_code bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
)
{
  rtems_interrupt_lock_context lock_context;
  uint32_t cpu_count;
  uint32_t cpu_index;
  uint32_t bit;

  if (vector >= RTEMS_ARRAY_SIZE(leon3_interrupt_affinities)) {
    return RTEMS_UNSATISFIED;
  }

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
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
)
{
  if (vector >= RTEMS_ARRAY_SIZE(leon3_interrupt_affinities)) {
    return RTEMS_UNSATISFIED;
  }

  *affinity = leon3_interrupt_affinities[vector];
  return RTEMS_SUCCESSFUL;
}
#endif
