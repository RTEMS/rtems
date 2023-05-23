/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This source file contains static assertions to ensure the consistency
 *   of interfaces used in C and assembler and it contains the ARM-specific
 *   implementation of _CPU_Initialize(), _CPU_ISR_Get_level(),
 *   _CPU_ISR_Set_level(), _CPU_ISR_install_vector(),
 *   _CPU_Context_Initialize(), and _CPU_Fatal_halt().
 */

/*
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2002 Advent Networks, Inc
 *      Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  Copyright (c) 2007 Ray xu <rayx.cn@gmail.com>
 *
 *  Copyright (C) 2009, 2017 embedded brains GmbH & Co. KG
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

#include <rtems/score/cpuimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/tls.h>

#ifdef ARM_MULTILIB_VFP
  RTEMS_STATIC_ASSERT(
    offsetof( Context_Control, register_d8 ) == ARM_CONTEXT_CONTROL_D8_OFFSET,
    ARM_CONTEXT_CONTROL_D8_OFFSET
  );
#endif

RTEMS_STATIC_ASSERT(
  offsetof( Context_Control, thread_id )
    == ARM_CONTEXT_CONTROL_THREAD_ID_OFFSET,
  ARM_CONTEXT_CONTROL_THREAD_ID_OFFSET
);

#ifdef ARM_MULTILIB_ARCH_V4
  RTEMS_STATIC_ASSERT(
    offsetof( Context_Control, isr_dispatch_disable )
      == ARM_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE,
    ARM_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE
  );
#endif

#ifdef RTEMS_SMP
  RTEMS_STATIC_ASSERT(
    offsetof( Context_Control, is_executing )
      == ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET,
    ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET
  );
#endif

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Exception_frame ) == ARM_EXCEPTION_FRAME_SIZE,
  ARM_EXCEPTION_FRAME_SIZE
);

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Exception_frame ) % CPU_STACK_ALIGNMENT == 0,
  CPU_Exception_frame_alignment
);

RTEMS_STATIC_ASSERT(
  offsetof( CPU_Exception_frame, register_sp )
    == ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET,
  ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET
);

RTEMS_STATIC_ASSERT(
  sizeof( ARM_VFP_context ) == ARM_VFP_CONTEXT_SIZE,
  ARM_VFP_CONTEXT_SIZE
);

#ifdef ARM_MULTILIB_ARCH_V4

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  (void) new_level;

  the_context->register_sp = (uint32_t) stack_area_begin + stack_area_size;
  the_context->register_lr = (uint32_t) entry_point;
  the_context->isr_dispatch_disable = 0;
  the_context->thread_id = (uint32_t) tls_area;

  if ( tls_area != NULL ) {
    the_context->thread_id = (uint32_t) _TLS_Initialize_area( tls_area );
  }
}

void _CPU_ISR_Set_level( uint32_t level )
{
  uint32_t arm_switch_reg;

  /* Ignore the level parameter and just enable interrupts */
  (void) level;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[arm_switch_reg], cpsr\n"
    "bic %[arm_switch_reg], #" RTEMS_XSTRING( ARM_PSR_I ) "\n"
    "msr cpsr, %0\n"
    ARM_SWITCH_BACK
    : [arm_switch_reg] "=&r" (arm_switch_reg)
  );
}

uint32_t _CPU_ISR_Get_level( void )
{
  ARM_SWITCH_REGISTERS;
  uint32_t level;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[level], cpsr\n"
    "and %[level], #" RTEMS_XSTRING( ARM_PSR_I ) "\n"
    ARM_SWITCH_BACK
    : [level] "=&r" (level) ARM_SWITCH_ADDITIONAL_OUTPUT
  );

  return ( level & ARM_PSR_I ) != 0;
}

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
  /* Redirection table starts at the end of the vector table */
  CPU_ISR_handler volatile  *table = (CPU_ISR_handler *) (MAX_EXCEPTIONS * 4);

  CPU_ISR_handler current_handler = table [vector];

  /* The current handler is now the old one */
  if (old_handler != NULL) {
    *old_handler = current_handler;
  }

  /* Write only if necessary to avoid writes to a maybe read-only memory */
  if (current_handler != new_handler) {
    table [vector] = new_handler;
  }
#pragma GCC diagnostic pop
}

void _CPU_Initialize( void )
{
  /* Do nothing */
}

#endif /* ARM_MULTILIB_ARCH_V4 */

void _CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr error )
{
  ISR_Level level;

  _CPU_ISR_Disable( level );
  (void) level;

  __asm__ volatile ("mov r0, %0\n"
                : "=r" (error)
                : "0" (error)
                : "r0" );

  while ( true ) {
    /* Do nothing */
  }
}
