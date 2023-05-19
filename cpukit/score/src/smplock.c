/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSMPLock
 *
 * @brief This source file contains the implementation of
 *   _SMP_lock_Initialize(), _SMP_lock_Destroy(), _SMP_lock_Acquire(),
 *   _SMP_lock_Release(), _SMP_lock_ISR_disable_and_acquire(),
 *   _SMP_lock_Release_and_ISR_enable(), and _SMP_lock_Is_owner().
 */

/*
 * Copyright (C) 2015, 2016 embedded brains GmbH & Co. KG
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

#include <rtems/score/smplock.h>

#if defined(RTEMS_SMP_LOCK_DO_NOT_INLINE)
void _SMP_lock_Initialize(
  SMP_lock_Control *lock,
  const char *name
)
{
  _SMP_lock_Initialize_inline( lock, name );
}

void _SMP_lock_Destroy( SMP_lock_Control *lock )
{
  _SMP_lock_Destroy_inline( lock );
}
#endif

void _SMP_lock_Acquire(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  _SMP_lock_Acquire_inline( lock, context );
}

#if defined(RTEMS_SMP_LOCK_DO_NOT_INLINE)
void _SMP_lock_Release(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  _SMP_lock_Release_inline( lock, context );
}
#endif

void _SMP_lock_ISR_disable_and_acquire(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  _SMP_lock_ISR_disable_and_acquire_inline( lock, context );
}

#if defined(RTEMS_SMP_LOCK_DO_NOT_INLINE)
void _SMP_lock_Release_and_ISR_enable(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  _SMP_lock_Release_and_ISR_enable_inline( lock, context );
}
#endif

#if defined(RTEMS_DEBUG)
bool _SMP_lock_Is_owner( const SMP_lock_Control *lock )
{
  return lock->owner == _SMP_lock_Who_am_I();
}
#endif
