/**
 * @file
 *
 * @ingroup ClassicEventImpl
 *
 * @brief Classic Event Implementation
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_EVENTIMPL_H
#define _RTEMS_RTEMS_EVENTIMPL_H

#include <rtems/rtems/event.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicEventImpl Classic Event Implementation
 *
 * @ingroup ClassicEvent
 *
 * @{
 */

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_EVENT_EXTERN
#define RTEMS_EVENT_EXTERN extern
#endif

/**
 *  This constant is passed as the event_in to the
 *  rtems_event_receive directive to determine which events are pending.
 */
#define EVENT_CURRENT  0

/**
 *  The following constant is the value of an event set which
 *  has no events pending.
 */
#define EVENT_SETS_NONE_PENDING 0

RTEMS_EVENT_EXTERN Thread_blocking_operation_States _Event_Sync_state;

RTEMS_EVENT_EXTERN Thread_blocking_operation_States _System_event_Sync_state;

/**
 *  @brief Event Manager Initialization
 *
 *  Event Manager
 *
 *  This routine performs the initialization necessary for this manager.
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
void _Event_Manager_initialization( void );

void _Event_Seize(
  rtems_event_set                   event_in,
  rtems_option                      option_set,
  rtems_interval                    ticks,
  rtems_event_set                  *event_out,
  Thread_Control                   *executing,
  Event_Control                    *event,
  Thread_blocking_operation_States *sync_state,
  States_Control                    wait_state
);

/**
 *  @brief Surrender Event
 *
 *  - INTERRUPT LATENCY:
 *    + before flash
 *    + after flash
 *    + check sync
 */
void _Event_Surrender(
  Thread_Control                   *the_thread,
  rtems_event_set                   event_in,
  Event_Control                    *event,
  Thread_blocking_operation_States *sync_state,
  States_Control                    wait_state
);

/**
 *  @brief Timeout Event
 */
void _Event_Timeout(
  Objects_Id  id,
  void       *arg
);

RTEMS_INLINE_ROUTINE void _Event_Initialize( Event_Control *event )
{
  event->pending_events = EVENT_SETS_NONE_PENDING;
}

/**
 *  @brief Checks if on events are posted in the event_set.
 *
 *  This function returns TRUE if on events are posted in the event_set,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Event_sets_Is_empty(
  rtems_event_set the_event_set
)
{
  return ( the_event_set == 0 );
}

/**
 *  @brief Posts the given new_events into the event_set passed in.
 *
 *  This routine posts the given new_events into the event_set
 *  passed in.  The result is returned to the user in event_set.
 */
RTEMS_INLINE_ROUTINE void _Event_sets_Post(
  rtems_event_set  the_new_events,
  rtems_event_set *the_event_set
)
{
  *the_event_set |= the_new_events;
}

/**
 *  @brief Returns the events in event_condition that are set in event_set.
 *
 *  This function returns the events in event_condition which are
 *  set in event_set.
 */
RTEMS_INLINE_ROUTINE rtems_event_set _Event_sets_Get(
  rtems_event_set the_event_set,
  rtems_event_set the_event_condition
)
{
   return ( the_event_set & the_event_condition );
}

/**
 *  @brief Removes the events in mask from the event_set passed in.
 *
 *  This function removes the events in mask from the event_set
 *  passed in.  The result is returned to the user in event_set.
 */
RTEMS_INLINE_ROUTINE rtems_event_set _Event_sets_Clear(
 rtems_event_set the_event_set,
 rtems_event_set the_mask
)
{
   return ( the_event_set & ~(the_mask) );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/eventmp.h>
#endif

#endif
/* end of include file */
