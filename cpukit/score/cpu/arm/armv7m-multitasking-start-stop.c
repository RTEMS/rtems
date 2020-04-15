/**
 * @file
 *
 * @brief ARMV7M Start Multitasking
 */

/*
 * Copyright (c) 2011-2014 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
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

#ifdef ARM_MULTILIB_ARCH_V7M

void __attribute__((naked)) _ARMV7M_Start_multitasking(
  Context_Control *heir
)
{
  __asm__ volatile (
    /* Restore heir context */
    "ldr r2, [r0, %[spctxoff]]\n"
    "msr psp, r2\n"
    "ldm r0, {r4-r11, lr}\n"
    /* Enable process stack pointer (PSP) */
    "mrs r2, control\n"
    "orr r2, #0x2\n"
    "msr control, r2\n"
    /* Return to heir */
    "bx lr\n"
    :
    : [spctxoff] "J" (offsetof(Context_Control, register_sp))
  );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
