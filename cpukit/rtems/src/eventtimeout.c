/*
 *  Event Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/event.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>

/*PAGE
 *
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
  Thread_Control *the_thread;
  Objects_Locations      location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:  /* impossible */
    case OBJECTS_ERROR:
      break;
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

      if ( _Event_Sync_state != EVENT_SYNC_SYNCHRONIZED && 
           _Thread_Is_executing( the_thread ) ) {
        if ( _Event_Sync_state != EVENT_SYNC_SATISFIED )
          _Event_Sync_state = EVENT_SYNC_TIMEOUT;
      } else {
        the_thread->Wait.return_code = RTEMS_TIMEOUT;
        _Thread_Unblock( the_thread );
      }
      _Thread_Unnest_dispatch();
      break;
  }
}
