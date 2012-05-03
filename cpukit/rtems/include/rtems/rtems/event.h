/**
 * @file rtems/rtems/event.h
 *
 *  This include file contains the information pertaining to the Event
 *  Manager.  This manager provides a high performance method of communication
 *  and synchronization.
 *
 *  Directives provided are:
 *
 *     - send an event set to a task
 *     - receive event condition
 *
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_EVENT_H
#define _RTEMS_RTEMS_EVENT_H

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_EVENT_EXTERN
#define RTEMS_EVENT_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/options.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadsync.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/eventset.h>

/**
 *  @defgroup ClassicEvent Events
 *
 *  @ingroup ClassicRTEMS
 *
 *  @brief The event manager provides a high performance method of intertask
 *  communication and synchronization.
 *
 *  An event flag is used by a task (or ISR) to inform another task of the
 *  occurrence of a significant situation. Thirty-two event flags are
 *  associated with each task. A collection of one or more event flags is
 *  referred to as an event set. The data type rtems_event_set is used to
 *  manage event sets.
 *
 *  The application developer should remember the following key characteristics
 *  of event operations when utilizing the event manager:
 *
 *  - Events provide a simple synchronization facility.
 *  - Events are aimed at tasks.
 *  - Tasks can wait on more than one event simultaneously.
 *  - Events are independent of one another.
 *  - Events do not hold or transport data.
 *  - Events are not queued. In other words, if an event is sent more than once
 *    to a task before being received, the second and subsequent send
 *    operations to that same task have no effect. 
 *
 *  An event set is posted when it is directed (or sent) to a task. A pending
 *  event is an event that has been posted but not received. An event condition
 *  is used to specify the event set which the task desires to receive and the
 *  algorithm which will be used to determine when the request is satisfied. An
 *  event condition is satisfied based upon one of two algorithms which are
 *  selected by the user. The @ref RTEMS_EVENT_ANY algorithm states that an
 *  event condition is satisfied when at least a single requested event is
 *  posted.  The @ref RTEMS_EVENT_ALL algorithm states that an event condition
 *  is satisfied when every requested event is posted. 
 *
 *  An event set or condition is built by a bitwise or of the desired events.
 *  The set of valid events is @ref RTEMS_EVENT_0 through @ref RTEMS_EVENT_31.
 *  If an event is not explicitly specified in the set or condition, then it is
 *  not present. Events are specifically designed to be mutually exclusive,
 *  therefore bitwise or and addition operations are equivalent as long as each
 *  event appears exactly once in the event set list.
 *
 *  For example, when sending the event set consisting of @ref RTEMS_EVENT_6,
 *  @ref RTEMS_EVENT_15, and @ref RTEMS_EVENT_31, the event parameter to the
 *  rtems_event_send() directive should be @ref RTEMS_EVENT_6 |
 *  @ref RTEMS_EVENT_15 | @ref RTEMS_EVENT_31. 
 *
 *  @{
 */

/**
 *  @brief Sends an event set to the target task.
 *
 *  This directive sends an event set @a event_in to the task specified by
 *  @a id.
 *
 *  Based upon the state of the target task, one of the following situations
 *  applies. The target task is
 *  - blocked waiting for events.
 *    If the waiting task's input event condition is
 *    - satisfied, then the task is made ready for execution.
 *    - not satisfied, then the event set is posted but left pending and the
 *      task remains blocked. 
 *  - not waiting for events.
 *    - The event set is posted and left pending. 
 *
 *  Identical events sent to a task are not queued. In other words, the second,
 *  and subsequent, posting of an event to a task before it can perform an
 *  rtems_event_receive() has no effect.
 *
 *  The calling task will be preempted if it has preemption enabled and a
 *  higher priority task is unblocked as the result of this directive.
 *
 *  Sending an event set to a global task which does not reside on the local
 *  node will generate a request telling the remote node to send the event set
 *  to the appropriate task.
 *
 *  @param[in] id Identifier of the target task.  Specifying @ref RTEMS_SELF
 *  results in the event set being sent to the calling task.
 *  @param[in] event_in Event set sent to the target task.
 *
 *  @retval RTEMS_SUCCESSFUL Successful operation.
 *  @retval RTEMS_INVALID_ID Invalid task identifier.
 */
rtems_status_code rtems_event_send (
  rtems_id        id,
  rtems_event_set event_in
);

/**
 *  @brief Receives pending events.
 *
 *  This directive attempts to receive the event condition specified in
 *  @a event_in.  If @a event_in is set to @ref RTEMS_PENDING_EVENTS, then the
 *  current pending events are returned in @a event_out and left pending. The
 *  @aref RTEMS_WAIT and @aref RTEMS_NO_WAIT options in the @a option_set
 *  parameter are used to specify whether or not the task is willing to wait
 *  for the event condition to be satisfied.  The @ref RTEMS_EVENT_ANY and @ref
 *  RTEMS_EVENT_ALL are used in the @a option_set parameter to specify whether
 *  at least a single event or the complete event set is necessary to satisfy
 *  the event condition.  The @a event_out parameter is returned to the calling
 *  task with the value that corresponds to the events in @a event_in that were
 *  satisfied.
 *
 *  A task can determine the pending event set by using a value of
 *  @ref RTEMS_PENDING_EVENTS for the input event set @a event_in.  The pending
 *  events are returned to the calling task but the event set is left
 *  unaltered.
 *
 *  A task can receive all of the currently pending events by using the a value
 *  of @ref RTEMS_ALL_EVENTS for the input event set @a event_in and
 *  @ref RTEMS_NO_WAIT | @ref RTEMS_EVENT_ANY for the option set @a option_set.
 *  The pending events are returned to the calling task and the event set is
 *  cleared.  If no events are pending then the @ref RTEMS_UNSATISFIED status
 *  code will be returned.
 *
 *  If pending events satisfy the event condition, then @a event_out is set to
 *  the satisfied events and the pending events in the event condition are
 *  cleared.  If the event condition is not satisfied and @ref RTEMS_NO_WAIT is
 *  specified, then @a event_out is set to the currently satisfied events.  If
 *  the calling task chooses to wait, then it will block waiting for the event
 *  condition.
 *
 *  If the calling task must wait for the event condition to be satisfied, then
 *  the timeout parameter is used to specify the maximum interval to wait.  If
 *  it is set to @ref RTEMS_NO_TIMEOUT, then the calling task will wait forever.
 *
 *  This directive only affects the events specified in @a event_in. Any
 *  pending events that do not correspond to any of the events specified in
 *  @a event_in will be left pending.
 *
 *  A clock tick is required to support the wait with time out functionality of
 *  this directive.
 *
 *  @param[in] event_in Set of requested events (input events).
 *  @param[in] option_set Use a bitwise or of the following options
 *  - @ref RTEMS_WAIT - task will wait for event (default),
 *  - @ref RTEMS_NO_WAIT - task should not wait,
 *  - @ref RTEMS_EVENT_ALL - return after all events (default), and
 *  - @ref RTEMS_EVENT_ANY - return after any events.
 *  @param[in] ticks Time out in ticks.  Use @ref RTEMS_NO_TIMEOUT to wait
 *  without a time out (potentially forever).
 *  @param[out] event_out Set of received events (output events).
 * 
 *  @retval RTEMS_SUCCESSFUL Successful operation.
 *  @retval RTEMS_UNSATISFIED Input events not satisfied (only with the
 *  @ref RTEMS_NO_WAIT option).
 *  @retval RTEMS_INVALID_ADDRESS The @a event_out pointer is @c NULL.
 *  @retval RTEMS_TIMEOUT Timed out waiting for events.
 */
rtems_status_code rtems_event_receive (
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
);

/** @} */

/**
 *  @defgroup ScoreEvent Event Handler
 *
 *  @ingroup Score
 *
 *  @{
 */

/**
 *  This constant is passed as the event_in to the
 *  rtems_event_receive directive to determine which events are pending.
 */
#define EVENT_CURRENT  0

/**
 *  @brief Event_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Event_Manager_initialization( void );

/**
 *  @brief Event_Seize
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

/**
 *  @brief Event_Surrender
 *
 *  This routine determines if the event condition of the_thread
 *  has been satisfied.  If so, it unblocks the_thread.
 */
void _Event_Surrender (
  Thread_Control *the_thread
);

/**
 *  @brief Event_Timeout
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

/**
 *  @brief he following defines the synchronization flag used by the
 */
RTEMS_EVENT_EXTERN volatile Thread_blocking_operation_States _Event_Sync_state;

/** @} */

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
