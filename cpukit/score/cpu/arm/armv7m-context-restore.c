/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/percpu.h>

#ifdef ARM_MULTILIB_ARCH_V7M

#include <rtems/score/armv7m.h>

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
