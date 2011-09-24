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

#include <rtems/score/cpu.h>

#ifdef ARM_MULTILIB_ARCH_V7M

#include <rtems/score/armv7m.h>

void __attribute__((naked)) _ARMV7M_Start_multitasking(
  Context_Control *bsp,
  Context_Control *heir
)
{
  __asm__ volatile (
    /* Store BSP context */
    "stm r0, {r4-r11, lr}\n"
    "str sp, [r0, %[spctxoff]]\n"
    /* Restore heir context */
    "ldr r2, [r1, %[spctxoff]]\n"
    "msr psp, r2\n"
    "ldm r1, {r4-r11, lr}\n"
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

void __attribute__((naked)) _ARMV7M_Stop_multitasking( Context_Control *bsp )
{
  __asm__ volatile (
    /* Disable interrupts */
    "mov r2, #0x80\n"
    "msr basepri_max, r2\n"
    /* Restore BSP context */
    "ldr r2, [r0, %[spctxoff]]\n"
    "msr msp, r2\n"
    "ldm r0, {r4-r11, lr}\n"
    /* Disable process stack pointer (PSP) */
    "mrs r2, control\n"
    "bic r2, #0x2\n"
    "msr control, r2\n"
    /* Return to BSP */
    "bx lr\n"
    :
    : [spctxoff] "J" (offsetof(Context_Control, register_sp))
  );
  __builtin_unreachable();
}

#endif /* ARM_MULTILIB_ARCH_V7M */
