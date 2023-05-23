/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This source file contains the implementation of
 *   _ARMV7M_Thread_dispatch().
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
#include <rtems/score/percpu.h>

#ifdef ARM_MULTILIB_ARCH_V7M

static void __attribute__((naked)) _ARMV7M_Thread_dispatch( void )
{
  __asm__ volatile (
    "bl _Thread_Dispatch\n"
    /* FIXME: SVC, binutils bug */
    ".short 0xdf00\n"
    "nop\n"
  );
}

static void _ARMV7M_Trigger_lazy_floating_point_context_save( void )
{
#ifdef ARM_MULTILIB_VFP
  __asm__ volatile (
    "vmov.f32 s0, s0\n"
  );
#endif
}

void _ARMV7M_Pendable_service_call( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  /*
   * We must check here if a thread dispatch is allowed.  Right after a
   * "msr basepri_max, %[basepri]" instruction an interrupt service may still
   * take place.  However, pendable service calls that are activated during
   * this interrupt service may be delayed until interrupts are enable again.
   */
  if (
    ( cpu_self->isr_nest_level | cpu_self->thread_dispatch_disable_level ) == 0
  ) {
    ARMV7M_Exception_frame *ef;

    cpu_self->isr_nest_level = 1;

    _ARMV7M_SCB->icsr = ARMV7M_SCB_ICSR_PENDSVCLR;
    _ARMV7M_Trigger_lazy_floating_point_context_save();

    ef = (ARMV7M_Exception_frame *) _ARMV7M_Get_PSP();
    --ef;
    _ARMV7M_Set_PSP( (uint32_t) ef );

    /*
     * According to "ARMv7-M Architecture Reference Manual" section B1.5.6
     * "Exception entry behavior" the return address is half-word aligned.
     */
    ef->register_pc = (void *)
      ((uintptr_t) _ARMV7M_Thread_dispatch & ~((uintptr_t) 1));

    ef->register_xpsr = 0x01000000U;
  }
}

void _ARMV7M_Supervisor_call( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();
  ARMV7M_Exception_frame *ef;

  _ARMV7M_Trigger_lazy_floating_point_context_save();

  ef = (ARMV7M_Exception_frame *) _ARMV7M_Get_PSP();
  ++ef;
  _ARMV7M_Set_PSP( (uint32_t) ef );

  cpu_self->isr_nest_level = 0;

  if ( cpu_self->dispatch_necessary ) {
    _ARMV7M_Pendable_service_call();
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
