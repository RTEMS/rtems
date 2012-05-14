/*
 *  Event Manager
 *
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/event.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>

/*
 *  _Event_Timeout
 *
 *  This routine processes a thread which timeouts while waiting to
 *  receive an event_set. It is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters: NONE
 */

void _Event_Timeout(
  Objects_Id  id,
  void       *ignored
)
{
  Thread_Control    *the_thread;
  Objects_Locations  location;
  ISR_Level          level;

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
        #if defined(RTEMS_DEBUG)
          if ( !the_thread->Wait.count ) {  /* verify thread is waiting */
            _Thread_Unnest_dispatch();
            _ISR_Enable( level );
            return;
          }
        #endif

        the_thread->Wait.count = 0;
        if ( _Thread_Is_executing( the_thread ) ) {
          if ( _Event_Sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED )
            _Event_Sync_state = THREAD_BLOCKING_OPERATION_TIMEOUT;
        }

        the_thread->Wait.return_code = RTEMS_TIMEOUT;
      _ISR_Enable( level );
      _Thread_Unblock( the_thread );
      _Thread_Unnest_dispatch();
      break;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:  /* impossible */
#endif
    case OBJECTS_ERROR:
      break;
  }
}
