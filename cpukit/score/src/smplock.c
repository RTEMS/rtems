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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/smplock.h>
#include <rtems/score/assert.h>
#include <rtems/score/smp.h>

#if defined(RTEMS_SMP_LOCK_DO_NOT_INLINE)

void _SMP_lock_Initialize(
  SMP_lock_Control *lock,
  const char *name
)
{
  _SMP_lock_Initialize_body( lock, name );
}

void _SMP_lock_Destroy( SMP_lock_Control *lock )
{
  _SMP_lock_Destroy_body( lock );
}

void _SMP_lock_Acquire(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
#if defined(RTEMS_DEBUG)
  context->lock_used_for_acquire = lock;
#endif
  _SMP_lock_Acquire_body( lock, context );
#if defined(RTEMS_DEBUG)
  lock->owner = _SMP_Get_current_processor();
#endif
}

void _SMP_lock_Release(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
#if defined(RTEMS_DEBUG)
  _Assert( context->lock_used_for_acquire == lock );
  context->lock_used_for_acquire = NULL;
  _Assert( lock->owner == _SMP_Get_current_processor() );
  lock->owner = SMP_LOCK_NO_OWNER;
#endif
  _SMP_lock_Release_body( lock, context );
}

void _SMP_lock_ISR_disable_and_acquire(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
#if defined(RTEMS_DEBUG)
  context->lock_used_for_acquire = lock;
#endif
  _SMP_lock_ISR_disable_and_acquire_body( lock, context );
#if defined(RTEMS_DEBUG)
  lock->owner = _SMP_Get_current_processor();
#endif
}

void _SMP_lock_Release_and_ISR_enable(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
#if defined(RTEMS_DEBUG)
  _Assert( context->lock_used_for_acquire == lock );
  context->lock_used_for_acquire = NULL;
  _Assert( lock->owner == _SMP_Get_current_processor() );
  lock->owner = SMP_LOCK_NO_OWNER;
#endif
  _SMP_lock_Release_body( lock, context );
  _ISR_Local_enable( context->isr_level );
}

#if defined(RTEMS_DEBUG)
bool _SMP_lock_Is_owner( const SMP_lock_Control *lock )
{
  return lock->owner == _SMP_Get_current_processor();
}
#endif

#endif /* defined(RTEMS_SMP_LOCK_DO_NOT_INLINE) */
