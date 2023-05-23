/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This source file contains the implementation of
 *   _CPU_Context_restore().
 */

/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
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

void __attribute__((naked)) _CPU_Context_restore(
  Context_Control *heir
)
{
  __asm__ volatile (
    "movw r2, #:lower16:_Per_CPU_Information\n"
    "movt r2, #:upper16:_Per_CPU_Information\n"
    "ldr r3, [r0, %[isrctxoff]]\n"
    "ldr sp, [r0, %[spctxoff]]\n"
    "ldm r0, {r4-r11, lr}\n"
    "str r3, [r2, %[isrpcpuoff]]\n"
    "bx lr\n"
    :
    : [spctxoff] "J" (offsetof(Context_Control, register_sp)),
      [isrctxoff] "J" (offsetof(Context_Control, isr_nest_level)),
      [isrpcpuoff] "J" (offsetof(Per_CPU_Control, isr_nest_level))
  );
  __builtin_unreachable();
}

#endif /* ARM_MULTILIB_ARCH_V7M */
