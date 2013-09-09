/**
 *  @file
 *
 *  @brief Timeout Event
 *  @ingroup ClassicEvent
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>
#include <rtems/score/threadimpl.h>

void _Event_Timeout(
  Objects_Id  id,
  void       *arg
)
{
  Thread_Control                   *the_thread;
  Objects_Locations                 location;
  ISR_Level                         level;
  Thread_blocking_operation_States *sync_state;

  sync_state = arg;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:

      /*
       *  If the event manager is not synchronized, then it is either
       *  "nothing happened", "timeout", or "satisfied".   If the_thread
       *  is the executing thread, then it is in the process of blocking
       *  and it is the thread which is responsible for the synchronization
       *  process.
       *
       *  If it is not satisfied, then it is "nothing happened" and
       *  this is the "timeout" transition.  After a request is satisfied,
       *  a timeout is not allowed to occur.
       */
      _ISR_Disable( level );
        /*
         * Verify that the thread is still waiting for the event condition.
         * This test is necessary to avoid state corruption if the timeout
         * happens after the event condition is satisfied in
         * _Event_Surrender().  A satisfied event condition is indicated with
         * count set to zero.
         */
        if ( !the_thread->Wait.count ) {
          _ISR_Enable( level );
          _Objects_Put_without_thread_dispatch( &the_thread->Object );
          return;
        }

        the_thread->Wait.count = 0;
        if ( _Thread_Is_executing( the_thread ) ) {
          if ( *sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED )
            *sync_state = THREAD_BLOCKING_OPERATION_TIMEOUT;
        }

        the_thread->Wait.return_code = RTEMS_TIMEOUT;
      _ISR_Enable( level );
      _Thread_Unblock( the_thread );
      _Objects_Put_without_thread_dispatch( &the_thread->Object );
      break;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:  /* impossible */
#endif
    case OBJECTS_ERROR:
      break;
  }
}
