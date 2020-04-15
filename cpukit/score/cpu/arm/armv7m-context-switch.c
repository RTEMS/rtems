/**
 * @file
 *
 * @brief ARM7M CPU Context Switch
 */

/*
 * Copyright (c) 2011-2014 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
