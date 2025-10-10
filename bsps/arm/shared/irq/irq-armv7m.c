/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
 * Copyright (C) 2011, 2012 Sebastian Huber
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

#include <bsp/irq-generic.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/armv7m-irq.h>

#include <rtems/score/armv7m.h>

#include <string.h>

#ifdef ARM_MULTILIB_ARCH_V7M

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  (void) vector;

  attributes->maximum_priority = 255;
  attributes->can_get_priority = true;
  attributes->can_set_priority = true;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
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
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  _ARMV7M_NVIC_Set_enable((int) vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  _ARMV7M_NVIC_Clear_enable((int) vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t priority
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (priority > 255) {
    return RTEMS_INVALID_PRIORITY;
  }

  _ARMV7M_NVIC_Set_priority((int) vector, (int) priority);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t *priority
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(priority != NULL);

  *priority = (uint32_t) _ARMV7M_NVIC_Get_priority((int) vector);
  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_facility_initialize(void)
{
  ARMV7M_Exception_handler *vector_table;
  int                       i;

  vector_table = (ARMV7M_Exception_handler *) bsp_vector_table_begin;

  if (&bsp_vector_table_begin[0] != &bsp_start_vector_table_begin[0]) {
    memcpy(
      vector_table,
      bsp_start_vector_table_begin,
      (size_t) bsp_vector_table_size
    );
  }

  _ARMV7M_SCB->icsr = ARMV7M_SCB_ICSR_PENDSVCLR | ARMV7M_SCB_ICSR_PENDSTCLR;

  for (i = 0; i < BSP_INTERRUPT_VECTOR_COUNT; ++i) {
    _ARMV7M_NVIC_Clear_enable(i);
    _ARMV7M_NVIC_Clear_pending(i);
    _ARMV7M_NVIC_Set_priority(i, BSP_ARMV7M_IRQ_PRIORITY_DEFAULT);
  }

  _ARMV7M_SCB->vtor = vector_table;
}

#endif /* ARM_MULTILIB_ARCH_V7M */
