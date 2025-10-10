/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This source file contains the implementation of
 *   _CPU_Context_switch().
 */

/*
 * Copyright (c) 2011-2014 Sebastian Huber.  All rights reserved.
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

void __attribute__((naked)) _CPU_Context_switch(
  Context_Control *executing,
  Context_Control *heir
)
{
  (void) executing;
  (void) heir;

  __asm__ volatile (
    "movw r2, #:lower16:_Per_CPU_Information\n"
    "movt r2, #:upper16:_Per_CPU_Information\n"
    "ldr r3, [r2, %[isrpcpuoff]]\n"
    "stm r0, {r4-r11, lr}\n"
#ifdef ARM_MULTILIB_VFP
    "add r4, r0, %[d8off]\n"
    "vstm r4, {d8-d15}\n"
#endif
    "str sp, [r0, %[spctxoff]]\n"
    "str r3, [r0, %[isrctxoff]]\n"
    "ldr r3, [r1, %[isrctxoff]]\n"
    "ldr sp, [r1, %[spctxoff]]\n"
#ifdef ARM_MULTILIB_VFP
    "add r4, r1, %[d8off]\n"
    "vldm r4, {d8-d15}\n"
#endif
    "ldm r1, {r4-r11, lr}\n"
    "str r3, [r2, %[isrpcpuoff]]\n"
    "bx lr\n"
    :
    : [spctxoff] "J" (offsetof(Context_Control, register_sp)),
#ifdef ARM_MULTILIB_VFP
      [d8off] "J" (ARM_CONTEXT_CONTROL_D8_OFFSET),
#endif
      [isrctxoff] "J" (offsetof(Context_Control, isr_nest_level)),
      [isrpcpuoff] "J" (offsetof(Per_CPU_Control, isr_nest_level))
  );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
