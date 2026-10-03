/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCPSIM
 *
 * @brief This source file contains the interrupt controller support of the
 *   psim BSP.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/openpic.h>
#include <bsp/vectors.h>
#include <libcpu/io.h>
#include <psim.h>

/* The priority of all sources.  The task priority of the processor is 0. */
#define PSIM_OPENPIC_PRIORITY 8

static bool is_source(rtems_vector_number vector)
{
  return vector < BSP_OPENPIC_SOURCE_LOWEST_OFFSET + BSP_OPENPIC_SOURCE_NUMBER;
}

static bool is_ipi(rtems_vector_number vector)
{
  return vector >= BSP_OPENPIC_IPI_LOWEST_OFFSET &&
    vector < BSP_OPENPIC_IPI_LOWEST_OFFSET + BSP_OPENPIC_IPI_NUMBER;
}

static volatile unsigned int *vector_priority(rtems_vector_number vector)
{
  if (is_source(vector)) {
    return &OpenPIC->Source[vector - BSP_OPENPIC_SOURCE_LOWEST_OFFSET]
      .Vector_Priority;
  }

  return &OpenPIC->Global.IPI_Vector_Priority(
    vector - BSP_OPENPIC_IPI_LOWEST_OFFSET
  );
}

static uint32_t read_vector_priority(rtems_vector_number vector)
{
  return in_le32((volatile uint32_t *) vector_priority(vector));
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  attributes->is_maskable = true;

  if (is_source(vector)) {
    attributes->can_enable = true;
    attributes->maybe_enable = true;
    attributes->can_disable = true;
    attributes->maybe_disable = true;
  } else if (is_ipi(vector)) {
    attributes->can_raise = true;
    attributes->cleared_by_acknowledge = true;
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

  if (vector == BSP_DECREMENTER) {
    /* The processor has no register which shows a pending exception */
    *pending = false;
  } else {
    /* The activity bit covers the pending and the in-service state */
    *pending = (read_vector_priority(vector) & OPENPIC_ACTIVITY) != 0;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (!is_ipi(vector)) {
    return RTEMS_UNSATISFIED;
  }

  openpic_cause_IPI(0, vector - BSP_OPENPIC_IPI_LOWEST_OFFSET, 1U << 0);
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

  if (vector == BSP_DECREMENTER) {
    *enabled = true;
  } else {
    *enabled = (read_vector_priority(vector) & OPENPIC_MASK) == 0;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (!is_source(vector)) {
    return RTEMS_UNSATISFIED;
  }

  openpic_enable_irq(vector - BSP_OPENPIC_SOURCE_LOWEST_OFFSET);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (!is_source(vector)) {
    return RTEMS_UNSATISFIED;
  }

  (void) openpic_disable_irq(vector - BSP_OPENPIC_SOURCE_LOWEST_OFFSET);
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

void bsp_interrupt_dispatch(uintptr_t exception_number)
{
  unsigned int openpic_vector;

  if (exception_number == ASM_DEC_VECTOR) {
    bsp_interrupt_handler_dispatch(BSP_DECREMENTER);
    return;
  }

  openpic_vector = openpic_irq(0);

  if (openpic_vector == OPENPIC_VEC_SPURIOUS) {
    return;
  }

  if (
    openpic_vector >= OPENPIC_VEC_IPI &&
    openpic_vector < OPENPIC_VEC_IPI + BSP_OPENPIC_IPI_NUMBER
  ) {
    /*
     * The end of interrupt comes before the handlers run, so that a handler
     * which enables the interrupts can take a nested interrupt of the same
     * vector.
     */
    openpic_eoi(0);
    bsp_interrupt_handler_dispatch(
      openpic_vector - OPENPIC_VEC_IPI + BSP_OPENPIC_IPI_LOWEST_OFFSET
    );
  } else {
    bsp_interrupt_handler_dispatch(
      openpic_vector - OPENPIC_VEC_SOURCE + BSP_OPENPIC_SOURCE_LOWEST_OFFSET
    );
    openpic_eoi(0);
  }
}

void bsp_interrupt_facility_initialize(void)
{
  int i;

  OpenPIC = (void *) PSIM.OpenPIC;
  openpic_init(1, NULL, NULL, BSP_OPENPIC_SOURCE_NUMBER, 0, 0);

  /*
   * The OpenPIC of the simulator drops an interprocessor interrupt which is
   * masked when it is raised.  The interprocessor interrupts thus stay
   * unmasked.
   */
  for (i = 0; i < BSP_OPENPIC_IPI_NUMBER; ++i) {
    volatile uint32_t *reg;

    openpic_initipi(i, PSIM_OPENPIC_PRIORITY, OPENPIC_VEC_IPI + i);
    reg = (volatile uint32_t *) vector_priority(
      BSP_OPENPIC_IPI_LOWEST_OFFSET + i
    );
    out_le32(reg, in_le32(reg) & ~OPENPIC_MASK);
  }
}
