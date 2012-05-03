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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Delayed_events_task(
  rtems_task_argument argument
)
{
  uint32_t          count;
  uint32_t          previous_mode;
  rtems_status_code status;
  rtems_event_set   events;
  rtems_id          self;

  status = rtems_task_mode(
    RTEMS_PREEMPT | RTEMS_TIMESLICE,
    RTEMS_PREEMPT_MASK | RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );

  status = rtems_timer_create( Timer_name[ 1 ], &Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_create" );

  self = rtems_task_self();

  while ( Stop_Test == false ) {
    for ( count=DELAYED_EVENT_DOT_COUNT; Stop_Test == false && count; count-- ){
      status = rtems_timer_fire_after(
        Timer_id[ 1 ],
        1,
        Delayed_send_event,
        &self
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
