/**
 * @file rtems/rtems/event.h
 *
 * @defgroup ClassicEvent Events
 *
 * @ingroup ClassicRTEMS
 * @brief Information Related to Event Manager
 *
 * This include file contains the information pertaining to the Event
 * Manager. This manager provides a high performance method of communication
 * and synchronization.
 *
 * Directives provided are:
 *
 * - send an event set to a task
 * - receive event condition
 *
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_EVENT_H
#define _RTEMS_RTEMS_EVENT_H

#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/options.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicEventSet Event Set
 *
 *  @ingroup ClassicEvent
 *
 *  @{
 */

/**
 *  @brief Integer type to hold an event set of up to 32 events represented as
 *  a bit field.
 */
typedef uint32_t   rtems_event_set;

/**
 *  @brief Constant used to send or receive all events.
 */
#define RTEMS_ALL_EVENTS  0xFFFFFFFF

/** @brief Defines the bit in the event set associated with event 0. */
#define RTEMS_EVENT_0     0x00000001
/** @brief Defines the bit in the event set associated with event 1. */
#define RTEMS_EVENT_1     0x00000002
/** @brief Defines the bit in the event set associated with event 2. */
#define RTEMS_EVENT_2     0x00000004
/** @brief Defines the bit in the event set associated with event 3. */
#define RTEMS_EVENT_3     0x00000008
/** @brief Defines the bit in the event set associated with event 4. */
#define RTEMS_EVENT_4     0x00000010
/** @brief Defines the bit in the event set associated with event 5. */
#define RTEMS_EVENT_5     0x00000020
/** @brief Defines the bit in the event set associated with event 6. */
#define RTEMS_EVENT_6     0x00000040
/** @brief Defines the bit in the event set associated with event 7. */
#define RTEMS_EVENT_7     0x00000080
/** @brief Defines the bit in the event set associated with event 8. */
#define RTEMS_EVENT_8     0x00000100
/** @brief Defines the bit in the event set associated with event 9. */
#define RTEMS_EVENT_9     0x00000200
/** @brief Defines the bit in the event set associated with event 10. */
#define RTEMS_EVENT_10    0x00000400
/** @brief Defines the bit in the event set associated with event 11. */
#define RTEMS_EVENT_11    0x00000800
/** @brief Defines the bit in the event set associated with event 12. */
#define RTEMS_EVENT_12    0x00001000
/** @brief Defines the bit in the event set associated with event 13. */
#define RTEMS_EVENT_13    0x00002000
/** @brief Defines the bit in the event set associated with event 14. */
#define RTEMS_EVENT_14    0x00004000
/** @brief Defines the bit in the event set associated with event 15. */
#define RTEMS_EVENT_15    0x00008000
/** @brief Defines the bit in the event set associated with event 16. */
#define RTEMS_EVENT_16    0x00010000
/** @brief Defines the bit in the event set associated with event 17. */
#define RTEMS_EVENT_17    0x00020000
/** @brief Defines the bit in the event set associated with event 18. */
#define RTEMS_EVENT_18    0x00040000
/** @brief Defines the bit in the event set associated with event 19. */
#define RTEMS_EVENT_19    0x00080000
/** @brief Defines the bit in the event set associated with event 20. */
#define RTEMS_EVENT_20    0x00100000
/** @brief Defines the bit in the event set associated with event 21. */
#define RTEMS_EVENT_21    0x00200000
/** @brief Defines the bit in the event set associated with event 22. */
#define RTEMS_EVENT_22    0x00400000
/** @brief Defines the bit in the event set associated with event 23. */
#define RTEMS_EVENT_23    0x00800000
/** @brief Defines the bit in the event set associated with event 24. */
#define RTEMS_EVENT_24    0x01000000
/** @brief Defines the bit in the event set associated with event 25. */
#define RTEMS_EVENT_25    0x02000000
/** @brief Defines the bit in the event set associated with event 26. */
#define RTEMS_EVENT_26    0x04000000
/** @brief Defines the bit in the event set associated with event 27. */
#define RTEMS_EVENT_27    0x08000000
/** @brief Defines the bit in the event set associated with event 29. */
#define RTEMS_EVENT_28    0x10000000
/** @brief Defines the bit in the event set associated with event 29. */
#define RTEMS_EVENT_29    0x20000000
/** @brief Defines the bit in the event set associated with event 30. */
#define RTEMS_EVENT_30    0x40000000
/** @brief Defines the bit in the event set associated with event 31. */
#define RTEMS_EVENT_31    0x80000000

/** @} */

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
 *  @brief Constant used to receive the set of currently pending events in
 *  rtems_event_receive().
 */
#define RTEMS_PENDING_EVENTS      0

/**
 * @brief Sends an Event Set to the Target Task
 *
 * This directive sends an event set @a event_in to the task specified by
 * @a id.
 *
 * Based upon the state of the target task, one of the following situations
 * applies. The target task is
 * - blocked waiting for events.
 * If the waiting task's input event condition is
 * - satisfied, then the task is made ready for execution.
 * - not satisfied, then the event set is posted but left pending and the
 * task remains blocked.
 * - not waiting for events.
 * - The event set is posted and left pending.
 *
 * Identical events sent to a task are not queued. In other words, the second,
 * and subsequent, posting of an event to a task before it can perform an
 * rtems_event_receive() has no effect.
 *
 * The calling task will be preempted if it has preemption enabled and a
 * higher priority task is unblocked as the result of this directive.
 *
 * Sending an event set to a global task which does not reside on the local
 * node will generate a request telling the remote node to send the event set
 * to the appropriate task.
 *
 * @param[in] id Identifier of the target task. Specifying @ref RTEMS_SELF
 * results in the event set being sent to the calling task.
 * @param[in] event_in Event set sent to the target task.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid task identifier.
 */
rtems_status_code rtems_event_send (
  rtems_id        id,
  rtems_event_set event_in
);

/**
 * @brief Receives pending events.
 *
 * This directive attempts to receive the event condition specified in
 * @a event_in. If @a event_in is set to @ref RTEMS_PENDING_EVENTS, then the
 * current pending events are returned in @a event_out and left pending. The
 * @ref RTEMS_WAIT and @ref RTEMS_NO_WAIT options in the @a option_set
 * parameter are used to specify whether or not the task is willing to wait
 * for the event condition to be satisfied. The @ref RTEMS_EVENT_ANY and @ref
 * RTEMS_EVENT_ALL are used in the @a option_set parameter to specify whether
 * at least a single event or the complete event set is necessary to satisfy
 * the event condition. The @a event_out parameter is returned to the calling
 * task with the value that corresponds to the events in @a event_in that were
 * satisfied.
 *
 * A task can determine the pending event set by using a value of
 * @ref RTEMS_PENDING_EVENTS for the input event set @a event_in. The pending
 * events are returned to the calling task but the event set is left
 * unaltered.
 *
 * A task can receive all of the currently pending events by using the a value
 * of @ref RTEMS_ALL_EVENTS for the input event set @a event_in and
 * @ref RTEMS_NO_WAIT | @ref RTEMS_EVENT_ANY for the option set @a option_set.
 * The pending events are returned to the calling task and the event set is
 * cleared. If no events are pending then the @ref RTEMS_UNSATISFIED status
 * code will be returned.
 *
 * If pending events satisfy the event condition, then @a event_out is set to
 * the satisfied events and the pending events in the event condition are
 * cleared.  If the event condition is not satisfied and @ref RTEMS_NO_WAIT is
 * specified, then @a event_out is set to the currently satisfied events.  If
 * the calling task chooses to wait, then it will block waiting for the event
 * condition.
 *
 * If the calling task must wait for the event condition to be satisfied, then
 * the timeout parameter is used to specify the maximum interval to wait. If
 * it is set to @ref RTEMS_NO_TIMEOUT, then the calling task will wait forever.
 *
 * This directive only affects the events specified in @a event_in. Any
 * pending events that do not correspond to any of the events specified in
 * @a event_in will be left pending.
 *
 * A clock tick is required to support the wait with time out functionality of
 * this directive.
 *
 * @param[in] event_in Set of requested events (input events).
 * @param[in] option_set Use a bitwise or of the following options
 * - @ref RTEMS_WAIT - task will wait for event (default),
 * - @ref RTEMS_NO_WAIT - task should not wait,
 * - @ref RTEMS_EVENT_ALL - return after all events (default), and
 * - @ref RTEMS_EVENT_ANY - return after any events.
 * @param[in] ticks Time out in ticks. Use @ref RTEMS_NO_TIMEOUT to wait
 * without a time out (potentially forever).
 * @param[out] event_out Set of received events (output events).
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_UNSATISFIED Input events not satisfied (only with the
 * @ref RTEMS_NO_WAIT option).
 * @retval RTEMS_INVALID_ADDRESS The @a event_out pointer is @c NULL.
 * @retval RTEMS_TIMEOUT Timed out waiting for events.
 */
rtems_status_code rtems_event_receive (
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
);

/** @} */

/**
 * @defgroup ClassicEventSystem System Events
 *
 * @ingroup ClassicEvent
 *
 * System events are similar to normal events.  They offer a second set of
 * events.  These events are intended for internal RTEMS use and should not be
 * used by applications (with the exception of the transient system event).
 *
 * The event @ref RTEMS_EVENT_SYSTEM_TRANSIENT is used for transient usage.
 * See also @ref ClassicEventTransient.  This event may be used by every entity
 * that fulfils its usage pattern.
 */
/**@{**/

/**
 * @brief Reserved system event for network SBWAIT usage.
 */
#define RTEMS_EVENT_SYSTEM_NETWORK_SBWAIT RTEMS_EVENT_24

/**
 * @brief Reserved system event for network SOSLEEP usage.
 */
#define RTEMS_EVENT_SYSTEM_NETWORK_SOSLEEP RTEMS_EVENT_25

/**
 * @brief Reserved system event for network socket close.
 */
#define RTEMS_EVENT_SYSTEM_NETWORK_CLOSE RTEMS_EVENT_26

/**
 * @brief Reserved system event for the timer server.
 */
#define RTEMS_EVENT_SYSTEM_TIMER_SERVER RTEMS_EVENT_30

/**
 * @brief Reserved system event for transient usage.
 */
#define RTEMS_EVENT_SYSTEM_TRANSIENT RTEMS_EVENT_31

/**
 * @brief See rtems_event_send().
 */
rtems_status_code rtems_event_system_send(
  rtems_id id,
  rtems_event_set event_in
);

/**
 * @brief See rtems_event_receive().
 */
rtems_status_code rtems_event_system_receive(
  rtems_event_set event_in,
  rtems_option option_set,
  rtems_interval ticks,
  rtems_event_set *event_out
);

/** @} */

/**
 * @defgroup ClassicEventTransient Transient Event
 *
 * @ingroup ClassicEvent
 *
 * The transient event can be used by a client task to issue a request to
 * another task or interrupt service (server).  The server can send the
 * transient event to the client task to notify about a request completion, see
 * rtems_event_transient_send().  The client task can wait for the transient
 * event reception with rtems_event_transient_receive().
 *
 * The user of the transient event must ensure that this event is not pending
 * once the request is finished or cancelled.  A successful reception of the
 * transient event with rtems_event_transient_receive() will clear the
 * transient event.  If a reception with timeout is used the transient event
 * state is undefined after a timeout return status.  The transient event can
 * be cleared unconditionally with the non-blocking
 * rtems_event_transient_clear().
 *
 * @msc
 *   hscale="1.6";
 *   M [label="Main Task"], IDLE [label="Idle Task"], S [label="Server"], TIME [label="System Tick Handler"];
 *   |||;
 *   --- [label="sequence with request completion"];
 *   M box M [label="prepare request\nissue request\nrtems_event_transient_receive()"];
 *   M=>>IDLE [label="blocking operation"];
 *   IDLE=>>S [label="request completion"];
 *   S box S [label="rtems_event_transient_send()"];
 *   S=>>M [label="task is ready again"];
 *   M box M [label="finish request"];
 *   |||;
 *   --- [label="sequence with early request completion"];
 *   M box M [label="prepare request\nissue request"];
 *   M=>>S [label="request completion"];
 *   S box S [label="rtems_event_transient_send()"];
 *   S=>>M [label="transient event is now pending"];
 *   M box M [label="rtems_event_transient_receive()\nfinish request"];
 *   |||;
 *   --- [label="sequence with timeout event"];
 *   M box M [label="prepare request\nissue request\nrtems_event_transient_receive()"];
 *   M=>>IDLE [label="blocking operation"];
 *   IDLE=>>TIME [label="timeout expired"];
 *   TIME box TIME [label="cancel blocking operation"];
 *   TIME=>>M [label="task is ready again"];
 *   M box M [label="cancel request\nrtems_event_transient_clear()"];
 * @endmsc
 *
 * Suppose you have a task that wants to issue a certain request and then waits
 * for request completion.  It can create a request structure and store its
 * task identifier there.  Now it can place the request on a work queue of
 * another task (or interrupt handler).  Afterwards the task waits for the
 * reception of the transient event.  Once the server task is finished with the
 * request it can send the transient event to the waiting task and wake it up.
 *
 * @code
 * #include <assert.h>
 * #include <rtems.h>
 *
 * typedef struct {
 *   rtems_id task_id;
 *   bool work_done;
 * } request;
 *
 * void server(rtems_task_argument arg)
 * {
 *   rtems_status_code sc;
 *   request *req = (request *) arg;
 *
 *   req->work_done = true;
 *
 *   sc = rtems_event_transient_send(req->task_id);
 *   assert(sc == RTEMS_SUCCESSFUL);
 *
 *   sc = rtems_task_delete(RTEMS_SELF);
 *   assert(sc == RTEMS_SUCCESSFUL);
 * }
 *
 * void issue_request_and_wait_for_completion(void)
 * {
 *   rtems_status_code sc;
 *   rtems_id id;
 *   request req;
 *
 *   req.task_id = rtems_task_self();
 *   req.work_done = false;
 *
 *   sc = rtems_task_create(
 *     rtems_build_name('S', 'E', 'R', 'V'),
 *     1,
 *     RTEMS_MINIMUM_STACK_SIZE,
 *     RTEMS_DEFAULT_MODES,
 *     RTEMS_DEFAULT_ATTRIBUTES,
 *     &id
 *   );
 *   assert(sc == RTEMS_SUCCESSFUL);
 *
 *   sc = rtems_task_start(id, server, (rtems_task_argument) &req);
 *   assert(sc == RTEMS_SUCCESSFUL);
 *
 *   sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
 *   assert(sc == RTEMS_SUCCESSFUL);
 *
 *   assert(req.work_done);
 * }
 * @endcode
 */
/**@{**/

/**
 * @brief See rtems_event_system_send().
 *
 * The system event @ref RTEMS_EVENT_SYSTEM_TRANSIENT will be sent.
 */
RTEMS_INLINE_ROUTINE rtems_status_code rtems_event_transient_send(
  rtems_id id
)
{
  return rtems_event_system_send( id, RTEMS_EVENT_SYSTEM_TRANSIENT );
}

/**
 * @brief See rtems_event_system_receive().
 *
 * The system event @ref RTEMS_EVENT_SYSTEM_TRANSIENT will be received.
 */
RTEMS_INLINE_ROUTINE rtems_status_code rtems_event_transient_receive(
  rtems_option option_set,
  rtems_interval ticks
)
{
  rtems_event_set event_out;

  return rtems_event_system_receive(
    RTEMS_EVENT_SYSTEM_TRANSIENT,
    RTEMS_EVENT_ALL | option_set,
    ticks,
    &event_out
  );
}

/**
 * @brief See rtems_event_system_receive().
 *
 * The system event @ref RTEMS_EVENT_SYSTEM_TRANSIENT will be cleared.
 */
RTEMS_INLINE_ROUTINE void rtems_event_transient_clear( void )
{
  rtems_event_set event_out;

  rtems_event_system_receive(
    RTEMS_EVENT_SYSTEM_TRANSIENT,
    RTEMS_EVENT_ALL | RTEMS_NO_WAIT,
    0,
    &event_out
  );
}

/** @} */

typedef struct {
  rtems_event_set pending_events;
} Event_Control;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
