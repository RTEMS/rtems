/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2011, 2021 embedded brains GmbH & Co. KG
 *
 * Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 * COPYRIGHT (c) 1989-2006
 * On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/score/cpu.h>
#include <rtems/score/nios2-utility.h>
#include <rtems/score/interr.h>
#include <rtems/score/tls.h>

void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  const Nios2_MPU_Configuration *mpu_config = _Nios2_MPU_Get_configuration();
  uint32_t stack = (uint32_t) stack_area_begin + stack_area_size - 4;

  memset(context, 0, sizeof(*context));

  context->fp = stack;
  context->status = _Nios2_ISR_Set_level( new_level, NIOS2_STATUS_PIE );
  context->sp = stack;
  context->ra = (uint32_t) entry_point;

  if ( mpu_config != NULL ) {
    Nios2_MPU_Region_descriptor desc = {
      .index = mpu_config->data_index_for_stack_protection,
      .base = stack_area_begin,
      .end = (const void *) RTEMS_ALIGN_UP(
        (uintptr_t) stack_area_begin + stack_area_size +
          _TLS_Get_allocation_size(),
        1U << mpu_config->data_region_size_log2
      ),
      .perm = NIOS2_MPU_DATA_PERM_SVR_READWRITE_USER_NONE,
      .data = true,
      .cacheable = mpu_config->enable_data_cache_for_stack,
      .read = false,
      .write = true
    };
    bool ok = _Nios2_MPU_Setup_region_registers(
      mpu_config,
      &desc,
      &context->stack_mpubase,
      &context->stack_mpuacc
    );

    if ( !ok ) {
      /* The task stack allocator must ensure that the stack area is valid */
      _Terminate( INTERNAL_ERROR_CORE, 0xdeadbeef );
    }
  }

  if ( tls_area != NULL ) {
    context->r23 = (uintptr_t) _TLS_Initialize_area( tls_area ) + 0x7000;
  }
}
