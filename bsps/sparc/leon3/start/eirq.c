/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This source file contains the implementation of the interrupt
 *   controller support.
 */

/*
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

#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/irqimpl.h>

#if !defined(LEON3_IRQAMP_EXTENDED_INTERRUPT)
/* GRLIB extended IRQ controller IRQ number */
uint32_t LEON3_IrqCtrl_EIrq;
#endif

rtems_interrupt_lock LEON3_IrqCtrl_Lock =
  RTEMS_INTERRUPT_LOCK_INITIALIZER("LEON3 IrqCtrl");

/* Initialize Extended Interrupt controller */
void leon3_ext_irq_init(irqamp *regs)
{
  grlib_store_32(&regs->pimask[LEON3_Cpu_Index], 0);
  grlib_store_32(&regs->piforce[LEON3_Cpu_Index], 0);
  grlib_store_32(&regs->iclear, 0xffffffff);
#if !defined(LEON3_IRQAMP_EXTENDED_INTERRUPT)
  LEON3_IrqCtrl_EIrq = IRQAMP_MPSTAT_EIRQ_GET(grlib_load_32(&regs->mpstat));
#endif
}

bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  if (vector == 0) {
    return false;
  }

#if defined(LEON3_IRQAMP_EXTENDED_INTERRUPT)
  return vector <= BSP_INTERRUPT_VECTOR_MAX_EXT;
#else
  if (LEON3_IrqCtrl_EIrq > 0) {
    return vector <= BSP_INTERRUPT_VECTOR_MAX_EXT;
  }

  return vector <= BSP_INTERRUPT_VECTOR_MAX_STD;
#endif
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

  leon3_ext_irq_init(LEON3_IrqCtrl_Regs);
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
  irqamp *regs;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  bit = 1U << vector;
  regs = LEON3_IrqCtrl_Regs;

  rtems_interrupt_local_disable(level);
  *pending = (grlib_load_32(&regs->ipend) & bit) != 0 ||
    (grlib_load_32(&regs->piforce[rtems_scheduler_get_processor()]) & bit) != 0;
  rtems_interrupt_local_enable(level);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  uint32_t bit;
  irqamp *regs;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bit = 1U << vector;
  regs = LEON3_IrqCtrl_Regs;

  if ( vector <= BSP_INTERRUPT_VECTOR_MAX_STD ) {
    uint32_t cpu_count;
    uint32_t cpu_index;

    cpu_count = rtems_scheduler_get_processor_maximum();

    for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
      grlib_store_32(&regs->piforce[cpu_index], bit);
    }
  } else {
    rtems_interrupt_lock_context lock_context;
    uint32_t ipend;

    /*
     * This is a very dangerous operation and should only be used for test
     * software.  We may accidentally clear the pending state set by
     * peripherals with this read-modify-write operation.
     */
    LEON3_IRQCTRL_ACQUIRE(&lock_context);
    ipend = grlib_load_32(&regs->ipend);
    ipend |= bit;
    grlib_store_32(&regs->ipend, ipend);
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
  irqamp *regs;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(cpu_index < rtems_scheduler_get_processor_maximum());

  if ( vector > BSP_INTERRUPT_VECTOR_MAX_STD ) {
    return RTEMS_UNSATISFIED;
  }

  regs = LEON3_IrqCtrl_Regs;
  grlib_store_32(&regs->piforce[cpu_index], 1U << vector);
  return RTEMS_SUCCESSFUL;
}
#endif

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  uint32_t bit;
  irqamp *regs;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bit = 1U << vector;
  regs = LEON3_IrqCtrl_Regs;

  grlib_store_32(&regs->iclear, bit);

  if (vector <= BSP_INTERRUPT_VECTOR_MAX_STD) {
    grlib_store_32(&regs->piforce[rtems_scheduler_get_processor()], bit << 16);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  uint32_t bit;
  irqamp *regs;
  uint32_t pimask;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  bit = 1U << vector;
  regs = LEON3_IrqCtrl_Regs;
  pimask = grlib_load_32(&regs->pimask[_LEON3_Get_current_processor()]);
  *enabled = (pimask & bit) != 0;
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
  uint32_t brdcst;
  irqamp *regs;

  if (vector <= BSP_INTERRUPT_VECTOR_MAX_STD) {
    affinity = leon3_interrupt_affinities[vector];
  } else {
    affinity = leon3_interrupt_affinities[LEON3_IrqCtrl_EIrq];
  }

  cpu_count = rtems_scheduler_get_processor_maximum();
  bit = 1U << vector;
  regs = LEON3_IrqCtrl_Regs;
  unmasked = 0;

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    uint32_t pimask;

    pimask = grlib_load_32(&regs->pimask[cpu_index]);

    if (_Processor_mask_Is_set(&affinity, cpu_index)) {
      ++unmasked;
      pimask |= bit;
    } else {
      pimask &= ~bit;
    }

    grlib_store_32(&regs->pimask[cpu_index], pimask);
  }

  brdcst = grlib_load_32(&regs->brdcst);

  if (unmasked > 1) {
    brdcst |= bit;
  } else {
    brdcst &= ~bit;
  }

  grlib_store_32(&regs->brdcst, brdcst);
}
#endif

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  rtems_interrupt_lock_context lock_context;
#if !defined(RTEMS_SMP)
  uint32_t bit;
  irqamp *regs;
  uint32_t pimask;
  uint32_t cpu_index;
#endif

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
#if !defined(RTEMS_SMP)
  bit = 1U << vector;
  regs = LEON3_IrqCtrl_Regs;
#endif

  LEON3_IRQCTRL_ACQUIRE(&lock_context);
#if defined(RTEMS_SMP)
  leon3_interrupt_vector_enable(vector);
#else
  cpu_index = _LEON3_Get_current_processor();
  pimask = grlib_load_32(&regs->pimask[cpu_index]);
  pimask |= bit;
  grlib_store_32(&regs->pimask[cpu_index], pimask);
#endif
  LEON3_IRQCTRL_RELEASE(&lock_context);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  rtems_interrupt_lock_context lock_context;
  uint32_t bit;
  irqamp *regs;
  uint32_t pimask;
  uint32_t cpu_index;
#if defined(RTEMS_SMP)
  uint32_t cpu_count;
  uint32_t brdcst;
#endif

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bit = 1U << vector;
  regs = LEON3_IrqCtrl_Regs;

  LEON3_IRQCTRL_ACQUIRE(&lock_context);

#if defined(RTEMS_SMP)
  cpu_count = rtems_scheduler_get_processor_maximum();

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    pimask = grlib_load_32(&regs->pimask[cpu_index]);
    pimask &= ~bit;
    grlib_store_32(&regs->pimask[cpu_index], pimask);
  }

  brdcst = grlib_load_32(&regs->brdcst);
  brdcst &= ~bit;
  grlib_store_32(&regs->brdcst, brdcst);
#else
  cpu_index = _LEON3_Get_current_processor();
  pimask = grlib_load_32(&regs->pimask[cpu_index]);
  pimask &= ~bit;
  grlib_store_32(&regs->pimask[cpu_index], pimask);
#endif

  LEON3_IRQCTRL_RELEASE(&lock_context);
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
  irqamp *regs;

  if (vector >= RTEMS_ARRAY_SIZE(leon3_interrupt_affinities)) {
    return RTEMS_UNSATISFIED;
  }

  cpu_count = rtems_scheduler_get_processor_maximum();
  bit = 1U << vector;
  regs = LEON3_IrqCtrl_Regs;

  LEON3_IRQCTRL_ACQUIRE(&lock_context);
  leon3_interrupt_affinities[vector] = *affinity;

  /*
   * If the interrupt is enabled on at least one processor, then re-enable it
   * using the new affinity.
   */
  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    if ((grlib_load_32(&regs->pimask[cpu_index]) & bit) != 0) {
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
