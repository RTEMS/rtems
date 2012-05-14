/*
 *  SuperCore RWLock Handler -- Release a RWLock
 *
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
#include <rtems/score/corerwlock.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/*
 *  _CORE_RWLock_Release
 *
 *  This function releases the rwlock.
 *
 *  Input parameters:
 *    the_rwlock    - the rwlock control block to initialize
 *
 *  Output parameters:  NONE
 */

CORE_RWLock_Status _CORE_RWLock_Release(
  CORE_RWLock_Control  *the_rwlock
)
{
  ISR_Level       level;
  Thread_Control *executing = _Thread_Executing;
  Thread_Control *next;

  /*
   *  If unlocked, then OK to read.
   *  Otherwise, we have to block.
   *  If locked for reading and no waiters, then OK to read.
   *  If any thread is waiting, then we wait.
   */

  _ISR_Disable( level );
    if ( the_rwlock->current_state == CORE_RWLOCK_UNLOCKED){
      _ISR_Enable( level );
      executing->Wait.return_code = CORE_RWLOCK_UNAVAILABLE;
      return CORE_RWLOCK_SUCCESSFUL;
    }
    if ( the_rwlock->current_state == CORE_RWLOCK_LOCKED_FOR_READING ) {
	the_rwlock->number_of_readers -= 1;
	if ( the_rwlock->number_of_readers != 0 ) {
          /* must be unlocked again */
	  _ISR_Enable( level );
          return CORE_RWLOCK_SUCCESSFUL;
        }
    }

    /* CORE_RWLOCK_LOCKED_FOR_WRITING or READING with readers */
    executing->Wait.return_code = CORE_RWLOCK_SUCCESSFUL;

    /*
     * Implicitly transition to "unlocked" and find another thread interested
     * in obtaining this rwlock.
     */
    the_rwlock->current_state = CORE_RWLOCK_UNLOCKED;
  _ISR_Enable( level );

  next = _Thread_queue_Dequeue( &the_rwlock->Wait_queue );

  if ( next ) {
    if ( next->Wait.option == CORE_RWLOCK_THREAD_WAITING_FOR_WRITE ) {
      the_rwlock->current_state = CORE_RWLOCK_LOCKED_FOR_WRITING;
      return CORE_RWLOCK_SUCCESSFUL;
    }

    /*
     * Must be CORE_RWLOCK_THREAD_WAITING_FOR_READING
     */
    the_rwlock->number_of_readers += 1;
    the_rwlock->current_state = CORE_RWLOCK_LOCKED_FOR_READING;

    /*
     * Now see if more readers can be let go.
     */
    while ( 1 ) {
      next = _Thread_queue_First( &the_rwlock->Wait_queue );
      if ( !next ||
           next->Wait.option == CORE_RWLOCK_THREAD_WAITING_FOR_WRITE )
        return CORE_RWLOCK_SUCCESSFUL;
      the_rwlock->number_of_readers += 1;
      _Thread_queue_Extract( &the_rwlock->Wait_queue, next );
    }
  }

  /* indentation is to match _ISR_Disable at top */

  return CORE_RWLOCK_SUCCESSFUL;
}
