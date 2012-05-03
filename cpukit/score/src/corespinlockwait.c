/*
 *  SuperCore Spinlock Handler -- Wait for Spinlock
 *
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/corespinlock.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/*
 *  _CORE_spinlock_Wait
 *
 *  This function waits for the spinlock to become available.  Optionally,
 *  a limit may be placed on the duration of the spin.
 *
 *  Input parameters:
 *    the_spinlock - the spinlock control block to initialize
 *    wait         - true if willing to wait
 *    timeout      - the maximum number of ticks to spin (0 is forever)
 *
 *  Output parameters:  NONE
 */

CORE_spinlock_Status _CORE_spinlock_Wait(
  CORE_spinlock_Control  *the_spinlock,
  bool                    wait,
  Watchdog_Interval       timeout
)
{
  ISR_Level level;
  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    Watchdog_Interval       limit = _Watchdog_Ticks_since_boot + timeout;
  #endif

  _ISR_Disable( level );
    if ( (the_spinlock->lock == CORE_SPINLOCK_LOCKED) &&
         (the_spinlock->holder == _Thread_Executing->Object.id) ) {
      _ISR_Enable( level );
      return CORE_SPINLOCK_HOLDER_RELOCKING;
    }
    the_spinlock->users += 1;
    for ( ;; ) {
      if ( the_spinlock->lock == CORE_SPINLOCK_UNLOCKED ) {
        the_spinlock->lock = CORE_SPINLOCK_LOCKED;
        the_spinlock->holder = _Thread_Executing->Object.id;
        _ISR_Enable( level );
        return CORE_SPINLOCK_SUCCESSFUL;
      }

      /*
       *  Spinlock is unavailable.  If not willing to wait, return.
       */
      if ( !wait ) {
        the_spinlock->users -= 1;
        _ISR_Enable( level );
        return CORE_SPINLOCK_UNAVAILABLE;
      }

      #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
        /*
         *  They are willing to wait but there could be a timeout.
         */
        if ( timeout && (limit <= _Watchdog_Ticks_since_boot) ) {
          the_spinlock->users -= 1;
          _ISR_Enable( level );
          return CORE_SPINLOCK_TIMEOUT;
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

       _ISR_Enable( level );
       /* An ISR could occur here */

       _Thread_Enable_dispatch();
       /* Another thread could get dispatched here */

       /* Reenter the critical sections so we can attempt the lock again. */
       _Thread_Disable_dispatch();

       _ISR_Disable( level );
    }

}
