/*
 *  @file
 *
 *  @brief Wait for Spinlock
 *  @ingroup ScoreSpinlock
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

Status_Control _CORE_spinlock_Seize(
  CORE_spinlock_Control *the_spinlock,
  bool                   wait,
  Watchdog_Interval      timeout,
  ISR_lock_Context      *lock_context
)
{
  Thread_Control *executing;

  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    Watchdog_Interval       limit = _Watchdog_Ticks_since_boot + timeout;
  #endif

  executing = _Thread_Executing;

  _CORE_spinlock_Acquire_critical( the_spinlock, lock_context );
    if ( the_spinlock->lock == CORE_SPINLOCK_LOCKED &&
         the_spinlock->holder == executing ) {
      _CORE_spinlock_Release( the_spinlock, lock_context );
      return STATUS_NESTING_NOT_ALLOWED;
    }
    the_spinlock->users += 1;
    for ( ;; ) {
      if ( the_spinlock->lock == CORE_SPINLOCK_UNLOCKED ) {
        the_spinlock->lock = CORE_SPINLOCK_LOCKED;
        the_spinlock->holder = executing;
        _CORE_spinlock_Release( the_spinlock, lock_context );
        return STATUS_SUCCESSFUL;
      }

      /*
       *  Spinlock is unavailable.  If not willing to wait, return.
       */
      if ( !wait ) {
        the_spinlock->users -= 1;
        _CORE_spinlock_Release( the_spinlock, lock_context );
        return STATUS_UNAVAILABLE;
      }

      #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
        /*
         *  They are willing to wait but there could be a timeout.
         */
        if ( timeout && (limit <= _Watchdog_Ticks_since_boot) ) {
          the_spinlock->users -= 1;
          _CORE_spinlock_Release( the_spinlock, lock_context );
          return STATUS_TIMEOUT;
        }
      #endif

      /*
       *  The thread is willing to spin so let's set things up so
       *  another thread has a chance of running.  This spinlock has
       *  to be released by either another thread or an ISR.  Since
       *  POSIX does not say anything about ISRs, that implies that
       *  another thread must be able to run while spinning.  We are
       *  not blocking so that implies we are at least preemptible
       *  and possibly time-sliced.
       *
       *  So first, we will enable interrpts to allow for them to happen.
       *  Then we will "flash" the thread dispatching critical section
       *  so other threads have a chance to run.
       *
       *  A spinlock cannot be deleted while it is being used so we are
       *  safe from deletion.
       */

       _CORE_spinlock_Release( the_spinlock, lock_context );

       /*
        * An ISR could occur here.  Another thread could get dispatched here.
        * Reenter the critical sections so we can attempt the lock again.
        */

       _ISR_lock_ISR_disable( lock_context );
       _CORE_spinlock_Acquire_critical( the_spinlock, lock_context );
    }

}
