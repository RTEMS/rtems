/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief NIOS II Specific Thread Dispatch Disabled Indicator.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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

#include <rtems/score/cpu.h>
#include <rtems/score/nios2-utility.h>

#define NIOS2_ASSERT_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(Context_Control, field) == NIOS2_CONTEXT_OFFSET_ ## off, \
    nios2_context_offset_ ## field \
  )

NIOS2_ASSERT_OFFSET(r16, R16);
NIOS2_ASSERT_OFFSET(r17, R17);
NIOS2_ASSERT_OFFSET(r18, R18);
NIOS2_ASSERT_OFFSET(r19, R19);
NIOS2_ASSERT_OFFSET(r20, R20);
NIOS2_ASSERT_OFFSET(r21, R21);
NIOS2_ASSERT_OFFSET(r22, R22);
NIOS2_ASSERT_OFFSET(r23, R23);
NIOS2_ASSERT_OFFSET(fp, FP);
NIOS2_ASSERT_OFFSET(status, STATUS);
NIOS2_ASSERT_OFFSET(sp, SP);
NIOS2_ASSERT_OFFSET(ra, RA);
NIOS2_ASSERT_OFFSET(isr_dispatch_disable, ISR_DISPATCH_DISABLE);
NIOS2_ASSERT_OFFSET(stack_mpubase, STACK_MPUBASE);
NIOS2_ASSERT_OFFSET(stack_mpuacc, STACK_MPUACC);
