/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#include <rtems/system.h>
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
NIOS2_ASSERT_OFFSET(thread_dispatch_disabled, THREAD_DISPATCH_DISABLED);
NIOS2_ASSERT_OFFSET(stack_mpubase, STACK_MPUBASE);
NIOS2_ASSERT_OFFSET(stack_mpuacc, STACK_MPUACC);

uint32_t _Nios2_Thread_dispatch_disabled;
