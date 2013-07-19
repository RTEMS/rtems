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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/corespinlockimpl.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

CORE_spinlock_Status _CORE_spinlock_Release(
  CORE_spinlock_Control  *the_spinlock
)
{
  ISR_Level level;

  _ISR_Disable( level );

    /*
     *  It must locked before it can be unlocked.
     */
    if ( the_spinlock->lock == CORE_SPINLOCK_UNLOCKED ) {
      _ISR_Enable( level );
      return CORE_SPINLOCK_NOT_LOCKED;
    }

    /*
     *  It must locked by the current thread before it can be unlocked.
     */
    if ( the_spinlock->holder != _Thread_Executing->Object.id ) {
      _ISR_Enable( level );
      return CORE_SPINLOCK_NOT_HOLDER;
    }

    /*
     *  Let it be unlocked.
     */
    the_spinlock->users -= 1;
    the_spinlock->lock   = CORE_SPINLOCK_UNLOCKED;
    the_spinlock->holder = 0;

  _ISR_Enable( level );
  return CORE_SPINLOCK_SUCCESSFUL;
}
