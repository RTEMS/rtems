/*
 * Copyright (c) 2012-2013 embedded brains GmbH.  All rights reserved.
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
#include <rtems/bspIo.h>

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame )
{
  printk(
    "\n"
    "R0   = 0x%08x R8  = 0x%08x\n"
    "R1   = 0x%08x R9  = 0x%08x\n"
    "R2   = 0x%08x R10 = 0x%08x\n"
    "R3   = 0x%08x R11 = 0x%08x\n"
    "R4   = 0x%08x R12 = 0x%08x\n"
    "R5   = 0x%08x SP  = 0x%08x\n"
    "R6   = 0x%08x LR  = 0x%08x\n"
    "R7   = 0x%08x PC  = 0x%08x\n"
#if defined(ARM_MULTILIB_ARCH_V4)
    "CPSR = 0x%08x VEC = 0x%08x\n",
#elif defined(ARM_MULTILIB_ARCH_V7M)
    "XPSR = 0x%08x VEC = 0x%08x\n",
#endif
    frame->register_r0,
    frame->register_r1,
    frame->register_r2,
    frame->register_r3,
    frame->register_r4,
    frame->register_r5,
    frame->register_r6,
    frame->register_r7,
    frame->register_r8,
    frame->register_r9,
    frame->register_r10,
    frame->register_r11,
    frame->register_r12,
    frame->register_sp,
    frame->register_lr,
    frame->register_pc,
#if defined(ARM_MULTILIB_ARCH_V4)
    frame->register_cpsr,
#elif defined(ARM_MULTILIB_ARCH_V7M)
    frame->register_xpsr,
#endif
    frame->vector
  );
}
