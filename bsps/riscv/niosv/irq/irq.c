/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Kevin Kirspel
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

#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/niosv.h>

#include <rtems/score/percpu.h>
#include <rtems/score/riscv-utility.h>

#include <string.h>

void _RISCV_Interrupt_dispatch(uintptr_t mcause, Per_CPU_Control *cpu_self)
{
  (void) cpu_self;

  uint32_t exception_code = (mcause & ~NIOSV_MCAUSE_INTERRUPT_MASK);

  if (exception_code == RISCV_INTERRUPT_TIMER_MACHINE) {
    bsp_interrupt_handler_dispatch_unchecked(NIOSV_INTERRUPT_VECTOR_TIMER);
  } else if (exception_code >= 16) {
    uint32_t interrupt_index, active;

    active = alt_irq_pending();

    while (active != 0) {
      interrupt_index = alt_irq_index(active);

      bsp_interrupt_handler_dispatch(
        NIOSV_INTERRUPT_VECTOR_EXTERNAL(interrupt_index)
      );

      active = alt_irq_pending();
    }
  } else if (exception_code == RISCV_INTERRUPT_SOFTWARE_MACHINE) {
    bsp_interrupt_handler_dispatch_unchecked(NIOSV_INTERRUPT_VECTOR_SOFTWARE);
  } else {
    bsp_fatal(RISCV_FATAL_UNEXPECTED_INTERRUPT_EXCEPTION);
  }
}

void bsp_interrupt_facility_initialize(void)
{
}

bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  return vector < (rtems_vector_number) BSP_INTERRUPT_VECTOR_COUNT;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  attributes->is_maskable = true;
  attributes->can_enable = true;
  attributes->maybe_enable = true;
  attributes->can_disable = true;
  attributes->maybe_disable = true;
  attributes->can_raise = (vector == NIOSV_INTERRUPT_VECTOR_SOFTWARE);
  attributes->can_raise_on = attributes->can_raise;
  attributes->cleared_by_acknowledge = true;
  attributes->can_get_affinity = false;
  attributes->can_set_affinity = false;

  if (vector == NIOSV_INTERRUPT_VECTOR_SOFTWARE) {
    attributes->trigger_signal = RTEMS_INTERRUPT_NO_SIGNAL;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);

  if (NIOSV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;

    interrupt_index = NIOSV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    *pending = alt_irq_is_pending(interrupt_index);
    return RTEMS_SUCCESSFUL;
  }

  if (vector == NIOSV_INTERRUPT_VECTOR_TIMER) {
    *pending = (read_csr(mip) & MIP_MTIP) != 0;
    return RTEMS_SUCCESSFUL;
  }

  _Assert(vector == NIOSV_INTERRUPT_VECTOR_SOFTWARE);
  *pending = (read_csr(mip) & MIP_MSIP) != 0;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector != NIOSV_INTERRUPT_VECTOR_SOFTWARE) {
    return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);

  if (NIOSV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;

    interrupt_index = NIOSV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    *enabled = alt_irq_is_enabled(interrupt_index);

    return RTEMS_SUCCESSFUL;
  }

  if (vector == NIOSV_INTERRUPT_VECTOR_TIMER) {
    *enabled = (read_csr(mie) & MIP_MTIP) != 0;
    return RTEMS_SUCCESSFUL;
  }

  _Assert(vector == NIOSV_INTERRUPT_VECTOR_SOFTWARE);
  *enabled = (read_csr(mie) & MIP_MSIP) != 0;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (NIOSV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;

    interrupt_index = NIOSV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    alt_irq_enable(interrupt_index);
    return RTEMS_SUCCESSFUL;
  }

  if (vector == NIOSV_INTERRUPT_VECTOR_TIMER) {
    set_csr(mie, MIP_MTIP);
    return RTEMS_SUCCESSFUL;
  }

  _Assert(vector == NIOSV_INTERRUPT_VECTOR_SOFTWARE);
  set_csr(mie, MIP_MSIP);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (NIOSV_INTERRUPT_VECTOR_IS_EXTERNAL(vector)) {
    uint32_t interrupt_index;

    interrupt_index = NIOSV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(vector);
    alt_irq_disable(interrupt_index);
    return RTEMS_SUCCESSFUL;
  }

  if (vector == NIOSV_INTERRUPT_VECTOR_TIMER) {
    clear_csr(mie, MIP_MTIP);
    return RTEMS_SUCCESSFUL;
  }

  _Assert(vector == NIOSV_INTERRUPT_VECTOR_SOFTWARE);
  clear_csr(mie, MIP_MSIP);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t *priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(priority != NULL);
  return RTEMS_UNSATISFIED;
}
