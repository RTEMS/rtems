/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  _ARMV7M_NVIC_Set_enable((int) vector);
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  _ARMV7M_NVIC_Clear_enable((int) vector);
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  ARMV7M_Exception_handler *vector_table;
  int                       i;

  vector_table = (ARMV7M_Exception_handler *) bsp_vector_table_begin;

  if (bsp_vector_table_begin != bsp_start_vector_table_begin) {
    memcpy(
      vector_table,
      bsp_start_vector_table_begin,
      (size_t) bsp_vector_table_size
    );
  }

  _ARMV7M_SCB->icsr = ARMV7M_SCB_ICSR_PENDSVCLR | ARMV7M_SCB_ICSR_PENDSTCLR;

  for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
    _ARMV7M_NVIC_Clear_enable(i);
    _ARMV7M_NVIC_Clear_pending(i);
    _ARMV7M_NVIC_Set_priority(i, BSP_ARMV7M_IRQ_PRIORITY_DEFAULT);
  }

  _ARMV7M_SCB->vtor = vector_table;

  return RTEMS_SUCCESSFUL;
}

#endif /* ARM_MULTILIB_ARCH_V7M */
