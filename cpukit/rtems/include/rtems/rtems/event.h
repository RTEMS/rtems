/*  event.h
 *
 *  This include file contains the information pertaining to the Event
 *  Manager.  This manager provides a high performance method of communication
 *  and synchronization.
 *
 *  Directives provided are:
 *
 *     + send an event set to a task
 *     + receive event condition
 *
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

#ifndef __RTEMS_EVENT_h
#define __RTEMS_EVENT_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/options.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/eventset.h>

/*
 *  This constant is passed as the event_in to the
 *  rtems_event_receive directive to determine which events are pending.
 */

#define EVENT_CURRENT  0

/*
 *  The following enumerated types indicate what happened while the event
 *  manager was in the synchronization window.
 */

typedef enum {
  EVENT_SYNC_SYNCHRONIZED,
  EVENT_SYNC_NOTHING_HAPPENED,
  EVENT_SYNC_TIMEOUT,
  EVENT_SYNC_SATISFIED
}  Event_Sync_states;

/*
 *  Event_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _Event_Manager_initialization( void );

/*
 *  rtems_event_send
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_event_send directive.  It sends
 *  event_in to the task specified by ID.  If the task is blocked
 *  waiting to receive events and the posting of event_in satisfies
 *  the task's event condition, then it is unblocked.
 */

rtems_status_code rtems_event_send (
  Objects_Id         id,
  rtems_event_set event_in
);

/*
 *  rtems_event_receive
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_event_receive directive.  This
 *  directive is invoked when the calling task wishes to receive
 *  the event_in event condition.  One of the fields in the option_set
 *  parameter determines whether the receive request is satisfied if
 *  any or all of the events are pending.   If the event condition
 *  is not satisfied immediately, then the task may block with an
 *  optional timeout of TICKS clock ticks or return immediately.
 *  This determination is based on another field in the option_set
 *  parameter.  This directive returns the events received in the
 *  event_out parameter.
 */

rtems_status_code rtems_event_receive (
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
);

/*
 *  _Event_Seize
 *
 *  DESCRIPTION:
 *
 *  This routine determines if the event condition event_in is
 *  satisfied.  If so or if the no_wait option is enabled in option_set,
 *  then the procedure returns immediately.  If neither of these
 *  conditions is true, then the calling task is blocked with an
 *  optional timeout of ticks clock ticks.
 */

void _Event_Seize (
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
);

/*
 *  _Event_Surrender
 *
 *  DESCRIPTION:
 *
 *  This routine determines if the event condition of the_thread
 *  has been satisfied.  If so, it unblocks the_thread.
 */

void _Event_Surrender (
  Thread_Control *the_thread
);

/*
 *  _Event_Timeout
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task's event receive request
 *  has not been satisfied after the specified timeout interval.
 *  The task represented by ID will be unblocked and its status
 *  code will be set in it's control block to indicate that a timeout
 *  has occurred.
 */

void _Event_Timeout (
  Objects_Id  id,
  void       *ignored
);

/*
 *  The following defines the synchronization flag used by the
 *  Event Manager to insure that signals sent to the currently
 *  executing thread are received properly.
 */

RTEMS_EXTERN volatile Event_Sync_states _Event_Sync_state;

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/eventmp.h>
#endif
#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/event.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
