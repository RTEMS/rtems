/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief Implementation of _CPU_Exception_frame_print.
 *
 * This file implements _CPU_Exception_frame_print for use in fatal output.
 * It dumps all standard integer and floating point registers as well as some
 * of the more important system registers.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <inttypes.h>

#include <rtems/score/cpu.h>
#include <rtems/bspIo.h>

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame )
{
  printk(
    "\n"
    "X0   = 0x%016" PRIx64  " X17  = 0x%016" PRIx64 "\n"
    "X1   = 0x%016" PRIx64  " X18  = 0x%016" PRIx64 "\n"
    "X2   = 0x%016" PRIx64  " X19  = 0x%016" PRIx64 "\n"
    "X3   = 0x%016" PRIx64  " X20  = 0x%016" PRIx64 "\n"
    "X4   = 0x%016" PRIx64  " X21  = 0x%016" PRIx64 "\n"
    "X5   = 0x%016" PRIx64  " X22  = 0x%016" PRIx64 "\n"
    "X6   = 0x%016" PRIx64  " X23  = 0x%016" PRIx64 "\n"
    "X7   = 0x%016" PRIx64  " X24  = 0x%016" PRIx64 "\n"
    "X8   = 0x%016" PRIx64  " X25  = 0x%016" PRIx64 "\n"
    "X9   = 0x%016" PRIx64  " X26  = 0x%016" PRIx64 "\n"
    "X10  = 0x%016" PRIx64  " X27  = 0x%016" PRIx64 "\n"
    "X11  = 0x%016" PRIx64  " X28  = 0x%016" PRIx64 "\n"
    "X12  = 0x%016" PRIx64  " FP   = 0x%016" PRIx64 "\n"
    "X13  = 0x%016" PRIx64  " LR   = 0x%016" PRIxPTR "\n"
    "X14  = 0x%016" PRIx64  " SP   = 0x%016" PRIxPTR "\n"
    "X15  = 0x%016" PRIx64  " PC   = 0x%016" PRIxPTR "\n"
    "X16  = 0x%016" PRIx64  " DAIF = 0x%016" PRIx64 "\n"
    "VEC  = 0x%016" PRIxPTR " CPSR = 0x%016" PRIx64 "\n"
    "ESR  = 0x%016" PRIx64  " FAR  = 0x%016" PRIx64 "\n",
    frame->register_x0, frame->register_x17,
    frame->register_x1, frame->register_x18,
    frame->register_x2, frame->register_x19,
    frame->register_x3, frame->register_x20,
    frame->register_x4, frame->register_x21,
    frame->register_x5, frame->register_x22,
    frame->register_x6, frame->register_x23,
    frame->register_x7, frame->register_x24,
    frame->register_x8, frame->register_x25,
    frame->register_x9, frame->register_x26,
    frame->register_x10, frame->register_x27,
    frame->register_x11, frame->register_x28,
    frame->register_x12, frame->register_fp,
    frame->register_x13, (intptr_t)frame->register_lr,
    frame->register_x14, (intptr_t)frame->register_sp,
    frame->register_x15, (intptr_t)frame->register_pc,
    frame->register_x16, frame->register_daif,
    (intptr_t) frame->vector, frame->register_cpsr,
    frame->register_syndrome, frame->register_fault_address
  );

  const uint128_t *qx = &frame->register_q0;
  int i;

  printk(
    "FPCR = 0x%016" PRIx64 " FPSR = 0x%016" PRIx64 "\n",
    frame->register_fpcr, frame->register_fpsr
  );

  for ( i = 0; i < 32; ++i ) {
    uint64_t low = (uint64_t) qx[i];
    uint64_t high = (uint64_t) (qx[i] >> 32);

    printk( "Q%02i  = 0x%016" PRIx64 "%016" PRIx64 "\n", i, high, low );
  }
}
