/**
 * @file
 *
 * @brief Release a Spinlock
 * @ingroup ScoreSpinlock
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/corespinlockimpl.h>
#include <rtems/score/percpu.h>

Status_Control _CORE_spinlock_Surrender(
  CORE_spinlock_Control *the_spinlock,
  ISR_lock_Context      *lock_context
)
{
  _CORE_spinlock_Acquire_critical( the_spinlock, lock_context );

    /*
     *  It must locked by the current thread before it can be unlocked.
     */
    if (
      the_spinlock->lock != CORE_SPINLOCK_LOCKED
        || the_spinlock->holder != _Thread_Executing
    ) {
      _CORE_spinlock_Release( the_spinlock, lock_context );
      return STATUS_NOT_OWNER;
    }

    /*
     *  Let it be unlocked.
     */
    the_spinlock->users -= 1;
    the_spinlock->lock   = CORE_SPINLOCK_UNLOCKED;
    the_spinlock->holder = 0;

  _CORE_spinlock_Release( the_spinlock, lock_context );
  return STATUS_SUCCESSFUL;
}
