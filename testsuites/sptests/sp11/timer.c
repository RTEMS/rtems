/*  Timer_functions
 *
 *  These routines are the timer service routines used by this test.
 *
 *  Input parameters:  NONE
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

rtems_timer_service_routine TA1_send_18_to_self_5_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_18 );
  directive_failed_with_level( status, "rtems_event_send of 18", 1 );
}

rtems_timer_service_routine TA1_send_8_to_self_60_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_8 );
  directive_failed_with_level( status, "rtems_event_send of 8", 1 );
}

rtems_timer_service_routine TA1_send_9_to_self_60_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_9 );
  directive_failed_with_level( status, "rtems_event_send of 9", 1 );
}

rtems_timer_service_routine TA1_send_10_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_10 );
  directive_failed_with_level( status, "rtems_event_send of 10", -1 );
}

rtems_timer_service_routine TA1_send_1_to_self_every_second(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_1 );
  directive_failed_with_level( status, "rtems_event_send of 1", 1 );
}

rtems_timer_service_routine TA1_send_11_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_11 );
  directive_failed_with_level( status, "rtems_event_send of 11", -1 );
}

rtems_timer_service_routine TA2_send_10_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 2 ], RTEMS_EVENT_10 );
  directive_failed_with_level( status, "rtems_event_send of 10", 1 );
}
