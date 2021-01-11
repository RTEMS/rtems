/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief AArch64 architecture support implementation.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <rtems/score/assert.h>
#include <rtems/score/cpu.h>
#include <rtems/score/thread.h>
#include <rtems/score/tls.h>

#ifdef AARCH64_MULTILIB_VFP
  RTEMS_STATIC_ASSERT(
    offsetof( Context_Control, register_d8 )
      == AARCH64_CONTEXT_CONTROL_D8_OFFSET,
    AARCH64_CONTEXT_CONTROL_D8_OFFSET
  );
#endif

  RTEMS_STATIC_ASSERT(
    offsetof( Context_Control, thread_id )
      == AARCH64_CONTEXT_CONTROL_THREAD_ID_OFFSET,
    AARCH64_CONTEXT_CONTROL_THREAD_ID_OFFSET
  );

  RTEMS_STATIC_ASSERT(
    offsetof( Context_Control, isr_dispatch_disable )
      == AARCH64_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE,
    AARCH64_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE
  );

#ifdef RTEMS_SMP
  RTEMS_STATIC_ASSERT(
    offsetof( Context_Control, is_executing )
      == AARCH64_CONTEXT_CONTROL_IS_EXECUTING_OFFSET,
    AARCH64_CONTEXT_CONTROL_IS_EXECUTING_OFFSET
  );
#endif

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Exception_frame ) == AARCH64_EXCEPTION_FRAME_SIZE,
  AARCH64_EXCEPTION_FRAME_SIZE
);

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Exception_frame ) % CPU_STACK_ALIGNMENT == 0,
  CPU_Exception_frame_alignment
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, register_sp )
    == AARCH64_EXCEPTION_FRAME_REGISTER_SP_OFFSET,
  AARCH64_EXCEPTION_FRAME_REGISTER_SP_OFFSET
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, register_lr )
    == AARCH64_EXCEPTION_FRAME_REGISTER_LR_OFFSET,
  AARCH64_EXCEPTION_FRAME_REGISTER_LR_OFFSET
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, register_daif )
    == AARCH64_EXCEPTION_FRAME_REGISTER_DAIF_OFFSET,
  AARCH64_EXCEPTION_FRAME_REGISTER_DAIF_OFFSET
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, register_syndrome )
    == AARCH64_EXCEPTION_FRAME_REGISTER_SYNDROME_OFFSET,
  AARCH64_EXCEPTION_FRAME_REGISTER_SYNDROME_OFFSET
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, vector )
    == AARCH64_EXCEPTION_FRAME_REGISTER_VECTOR_OFFSET,
  AARCH64_EXCEPTION_FRAME_REGISTER_VECTOR_OFFSET
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, register_fpsr )
    == AARCH64_EXCEPTION_FRAME_REGISTER_FPSR_OFFSET,
  AARCH64_EXCEPTION_FRAME_REGISTER_FPSR_OFFSET
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, register_q0 )
    == AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET,
  AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET
);


void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint64_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  (void) new_level;

  the_context->register_sp = (uintptr_t) stack_area_begin + stack_area_size;
  the_context->register_lr = (uintptr_t) entry_point;
  the_context->isr_dispatch_disable = 0;

  the_context->thread_id = (uintptr_t) tls_area;

  if ( tls_area != NULL ) {
    _TLS_TCB_at_area_begin_initialize( tls_area );
  }
}

void _CPU_ISR_Set_level( uint64_t level )
{
  /* Set the mask bit if interrupts are disabled */
  level = level ? AARCH64_PSTATE_I : 0;
  __asm__ volatile (
    "msr DAIF, %[level]\n"
    : : [level] "r" (level)
  );
}

uint64_t _CPU_ISR_Get_level( void )
{
  uint64_t level;

  __asm__ volatile (
    "mrs %[level], DAIF\n"
    : [level] "=&r" (level)
  );

  return ( level & AARCH64_PSTATE_I ) != 0;
}

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
  /* Redirection table starts at the end of the vector table */
  CPU_ISR_handler *table = (CPU_ISR_handler *) (MAX_EXCEPTIONS * 4);

  CPU_ISR_handler current_handler = table [vector];

  /* The current handler is now the old one */
  if (old_handler != NULL) {
    *old_handler = current_handler;
  }

  /* Write only if necessary to avoid writes to a maybe read-only memory */
  if (current_handler != new_handler) {
    table [vector] = new_handler;
  }
}

void _CPU_Initialize( void )
{
  /* Do nothing */
}
