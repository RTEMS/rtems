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
 * Copyright (c) 2015, 2016 embedded brains GmbH.  All rights reserved.
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
