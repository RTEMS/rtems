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

static void _ARM_VFP_context_print( const ARM_VFP_context *vfp_context )
{
#ifdef ARM_MULTILIB_VFP_D32
  if ( vfp_context != NULL ) {
    const uint64_t *dx = &vfp_context->register_d0;
    int i;

    printk(
      "FPEXC = 0x%08x\nFPSCR = 0x%08x\n",
      vfp_context->register_fpexc,
      vfp_context->register_fpscr
    );

    for ( i = 0; i < 32; ++i ) {
      uint32_t low = (uint32_t) dx[i];
      uint32_t high = (uint32_t) (dx[i] >> 32);

      printk( "D%02i = 0x%08x%08x\n", i, high, low );
    }
  }
#endif
}

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
    "CPSR = 0x%08x "
#elif defined(ARM_MULTILIB_ARCH_V7M)
    "XPSR = 0x%08x "
#endif
    "VEC = 0x%08x\n",
    frame->register_r0,
    frame->register_r8,
    frame->register_r1,
    frame->register_r9,
    frame->register_r2,
    frame->register_r10,
    frame->register_r3,
    frame->register_r11,
    frame->register_r4,
    frame->register_r12,
    frame->register_r5,
    frame->register_sp,
    frame->register_r6,
    frame->register_lr,
    frame->register_r7,
    frame->register_pc,
#if defined(ARM_MULTILIB_ARCH_V4)
    frame->register_cpsr,
#elif defined(ARM_MULTILIB_ARCH_V7M)
    frame->register_xpsr,
#endif
    frame->vector
  );

  _ARM_VFP_context_print( frame->vfp_context );
}
