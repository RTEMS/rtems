/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This source file contains the implementation of
 *   _ARMV7M_Interrupt_service_enter() and _ARMV7M_Interrupt_service_leave().
 */

/*
 * Copyright (c) 2011, 2017 Sebastian Huber.  All rights reserved.
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

#include <rtems/score/armv7m.h>
#include <rtems/score/isr.h>
#include <rtems/score/threaddispatch.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _ARMV7M_Interrupt_service_enter( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  ++cpu_self->thread_dispatch_disable_level;
  ++cpu_self->isr_nest_level;
}

void _ARMV7M_Interrupt_service_leave( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  --cpu_self->thread_dispatch_disable_level;
  --cpu_self->isr_nest_level;

  /*
   * Optimistically activate a pendable service call if a thread dispatch is
   * necessary.  The _ARMV7M_Pendable_service_call() will check that a thread
   * dispatch is allowed.
   */
  if ( cpu_self->dispatch_necessary ) {
    _ARMV7M_SCB->icsr = ARMV7M_SCB_ICSR_PENDSVSET;
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
