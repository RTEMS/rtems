/*  Delayed_events_task
 *
 *  This task continuously sends itself events at one tick
 *  intervals.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
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

#include "system.h"

rtems_task Delayed_events_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32  count;
  rtems_unsigned32  previous_mode;
  rtems_status_code status;
  rtems_event_set   events;

  status = rtems_task_mode(
    RTEMS_PREEMPT | RTEMS_TIMESLICE,
    RTEMS_PREEMPT_MASK | RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );

  status = rtems_timer_create( Timer_name[ 1 ], &Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_create" );

  while ( Stop_Test == FALSE ) {
    for ( count=DELAYED_EVENT_DOT_COUNT; Stop_Test == FALSE && count; count-- ){
      status = rtems_timer_fire_after(
        Timer_id[ 1 ],
        1,
        Delayed_send_event,
        NULL
      );
      directive_failed( status, "rtems_timer_reset" );

      status = rtems_event_receive(
        RTEMS_EVENT_16,
        RTEMS_DEFAULT_OPTIONS,
        RTEMS_NO_TIMEOUT,
        &events
      );
      directive_failed( status, "rtems_event_receive" );
    }
    put_dot('.');
  }

  Exit_test();
}
