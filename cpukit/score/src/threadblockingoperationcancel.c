/*
 *  Cancel Thread Blocking Operation
 *
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/thread.h>
#if defined(RTEMS_DEBUG)
#include <rtems/score/interr.h>
#endif

void _Thread_blocking_operation_Cancel(
#if defined(RTEMS_DEBUG)
  Thread_blocking_operation_States  sync_state,
#else
  Thread_blocking_operation_States  sync_state __attribute__((unused)),
#endif
  Thread_Control                   *the_thread,
  ISR_Level                         level
)
{
  /*
   *  Cases that should not happen and why.
   *
   *  THREAD_BLOCKING_OPERATION_SYNCHRONIZED:
   *
   *  This indicates that someone did not enter a blocking
   *  operation critical section.
   *
   *  THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED:
   *
   *  This indicates that there was nothing to cancel so
   *  we should not have been called.
   */

  #if defined(RTEMS_DEBUG)
    if ( (sync_state == THREAD_BLOCKING_OPERATION_SYNCHRONIZED) ||
         (sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED) ) {
      _Internal_error_Occurred(
        INTERNAL_ERROR_CORE,
        true,
        INTERNAL_ERROR_IMPLEMENTATION_BLOCKING_OPERATION_CANCEL
      );
    }
  #endif

  /*
   * The thread is not waiting on anything after this completes.
   */
  the_thread->Wait.queue = NULL;

  /*
   *  If the sync state is timed out, this is very likely not needed.
   *  But better safe than sorry when it comes to critical sections.
   */
  if ( _Watchdog_Is_active( &the_thread->Timer ) ) {
    _Watchdog_Deactivate( &the_thread->Timer );
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
  } else
    _ISR_Enable( level );

  /*
   *  Global objects with thread queue's should not be operated on from an
   *  ISR.  But the sync code still must allow short timeouts to be processed
   *  correctly.
   */

  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif

}
