/*
 *  COPYRIGHT (c) 1989-2011.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_event_set   eventout;
  rtems_time_of_day time;
  rtems_status_code status;
  uint32_t          index;

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_16 to TA2" );
  status = rtems_event_send( Task_id[ 2 ], RTEMS_EVENT_16 );
  directive_failed( status, "rtems_event_send" );

  puts(
    "TA1 - rtems_event_receive - waiting forever on "
      "RTEMS_EVENT_14 and RTEMS_EVENT_15"
  );
  status = rtems_event_receive(
    RTEMS_EVENT_14 | RTEMS_EVENT_15,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );
  printf(
    "TA1 - RTEMS_EVENT_14 and RTEMS_EVENT_15 received - "
      "eventout => %08" PRIxrtems_event_set "\n",
    eventout
  );

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_18 to TA2" );
  status = rtems_event_send( Task_id[ 2 ], RTEMS_EVENT_18 );
  directive_failed( status, "rtems_event_send" );

  puts(
    "TA1 - rtems_event_receive - waiting with 10 second timeout "
      "on RTEMS_EVENT_14"
  );
  status = rtems_event_receive(
    RTEMS_EVENT_14,
    RTEMS_DEFAULT_OPTIONS,
    10 * rtems_clock_get_ticks_per_second(),
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );
  printf(
    "TA1 - RTEMS_EVENT_14 received - eventout => "
      "%08" PRIxrtems_event_set "\n",
    eventout
  );

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_19 to TA2" );
  status = rtems_event_send( Task_id[ 2 ], RTEMS_EVENT_19 );
  directive_failed( status, "rtems_event_send" );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, "\n" );

rtems_test_pause();

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_18 to self after 5 seconds");
  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    5 * rtems_clock_get_ticks_per_second(),
    TA1_send_18_to_self_5_seconds,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 5 seconds" );

  puts( "TA1 - rtems_event_receive - waiting forever on RTEMS_EVENT_18"  );
  status = rtems_event_receive(
    RTEMS_EVENT_18,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive of 18" );
  printf(
    "TA1 - RTEMS_EVENT_18 received - eventout => %08" PRIxrtems_event_set "\n",
    eventout
  );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "TA1 rtems_clock_get_tod" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, "\n" );

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_3 to self" );
  status = rtems_event_send( RTEMS_SELF, RTEMS_EVENT_3 );
  directive_failed( status, "rtems_event_send" );

  puts(
    "TA1 - rtems_event_receive - RTEMS_EVENT_3 or "
      "RTEMS_EVENT_22 - NO_WAIT and ANY"
  );
  status = rtems_event_receive(
    RTEMS_EVENT_3 | RTEMS_EVENT_22,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive of 3 and 22" );
  printf(
    "TA1 - RTEMS_EVENT_3 received - eventout => %08" PRIxrtems_event_set "\n",
    eventout
  );

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_4 to self" );
  status = rtems_event_send( RTEMS_SELF, RTEMS_EVENT_4 );
  directive_failed( status, "rtems_event_send" );

  puts (
  "TA1 - rtems_event_receive - RTEMS_EVENT_4 or "
      "RTEMS_EVENT_5 - forever and ANY"
  );
  status = rtems_event_receive(
    RTEMS_EVENT_4 | RTEMS_EVENT_5,
    RTEMS_EVENT_ANY,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );
  printf(
    "TA1 - RTEMS_EVENT_4 received - eventout => %08" PRIxrtems_event_set "\n",
    eventout 
  );

rtems_test_pause();

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_18 to self after 5 seconds");
  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    5 * rtems_clock_get_ticks_per_second(),
    TA1_send_18_to_self_5_seconds,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 5 seconds" );

  puts( "TA1 - rtems_timer_cancel - cancelling timer for event RTEMS_EVENT_18");
  status = rtems_timer_cancel( Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_cancel" );

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_8 to self after 60 seconds");
  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    60 * rtems_clock_get_ticks_per_second(),
    TA1_send_8_to_self_60_seconds,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 60 seconds" );

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_9 to self after 60 seconds");
  status = rtems_timer_fire_after(
    Timer_id[ 2 ],
    60 * rtems_clock_get_ticks_per_second(),
    TA1_send_9_to_self_60_seconds,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 60 seconds" );

  puts(
    "TA1 - rtems_event_send - send RTEMS_EVENT_10 to self after 60 seconds"
  );
  status = rtems_timer_fire_after(
    Timer_id[ 3 ],
    60 * rtems_clock_get_ticks_per_second(),
    TA1_send_10_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 60 seconds" );

  puts( "TA1 - rtems_timer_cancel - cancelling timer for event RTEMS_EVENT_8" );
  status = rtems_timer_cancel( Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_cancel" );

  build_time( &time, 2, 12, 1988, 8, 15, 0, 0 );

  print_time( "TA1 - rtems_clock_set - ", &time, "\n" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_1 every second" );
  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    rtems_clock_get_ticks_per_second(),
    TA1_send_1_to_self_every_second,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 1 second" );

  for ( index = 0; index < 3; index++ ) {

    status = rtems_event_receive(
      RTEMS_EVENT_1,
      RTEMS_EVENT_ANY,
      RTEMS_NO_TIMEOUT,
      &eventout
    );
    directive_failed( status, "rtems_event_receive" );

    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod" );

    printf(
      "TA1 - RTEMS_EVENT_1 received - eventout => %08"
        PRIxrtems_event_set " - ",
       eventout
    );
    print_time( "at ", &time, "\n" );

    if ( index < 2 ) {
      status = rtems_timer_reset( Timer_id[ 1 ] );
      directive_failed( status, "rtems_timer_reset" );
    };

  }

  puts( "TA1 - rtems_timer_cancel - cancelling timer for event RTEMS_EVENT_1" );
  status = rtems_timer_cancel( Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_cancel" );

rtems_test_pause();

  time.day = 13;
  puts( "TA1 - rtems_event_send - sending RTEMS_EVENT_11 to self in 1 day" );
  status = rtems_timer_fire_when(
    Timer_id[ 1 ],
    &time,
    TA1_send_11_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_when 1 day" );

  time.hour = 7;
  puts( "TA1 - rtems_event_send - sending RTEMS_EVENT_11 to self in 1 day" );
  status = rtems_timer_fire_when(
    Timer_id[ 2 ],
    &time,
    TA1_send_11_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_when 1 day" );

  time.hour = 8;   /* so code below has correct time/date */
  time.day = 14;
  puts( "TA1 - rtems_event_send - sending RTEMS_EVENT_11 to self in 2 days" );
  status = rtems_timer_fire_when(
    Timer_id[ 3 ],
    &time,
    TA1_send_11_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_when 2 days" );

  puts("TA1 - rtems_timer_cancel - cancelling RTEMS_EVENT_11 to self in 1 day");
  status = rtems_timer_cancel( Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_cancel" );

  puts(
    "TA1 - rtems_timer_cancel - cancelling RTEMS_EVENT_11 to self in 2 days"
  );
  status = rtems_timer_cancel( Timer_id[ 3 ] );
  directive_failed( status, "rtems_timer_cancel" );

  puts(
    "TA1 - rtems_event_send - resending RTEMS_EVENT_11 to self in 2 days"
  );
  status = rtems_timer_fire_when(
    Timer_id[ 3 ],
    &time,
    TA1_send_11_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_when 2 days" );

  time.day = 15;
  print_time( "TA1 - rtems_clock_set - ", &time, "\n" );
  status = rtems_clock_set( &time );
  directive_failed( status, "TA1 rtems_clock_set" );

  puts( "TA1 - rtems_event_receive - waiting forever on RTEMS_EVENT_11" );
  status = rtems_event_receive(
    RTEMS_EVENT_11,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );
  printf(
    "TA1 - RTEMS_EVENT_11 received - eventout => %08" PRIxrtems_event_set "\n",
     eventout
  );

rtems_test_pause();

  puts( "TA1 - rtems_event_send/rtems_event_receive combination" );
  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    10,
    TA1_send_11_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 10 ticks" );

  status = rtems_event_receive(
    RTEMS_EVENT_11,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );

  build_time( &time, 2, 12, 1988, 8, 15, 0, 0 );

  print_time( "TA1 - rtems_clock_set - ", &time, "\n" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  time.day = 13;
  puts( "TA1 - rtems_event_receive all outstanding events" );
  status  = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &eventout
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_event_receive all events"
  );

  puts( "TA1 - rtems_event_send - sending RTEMS_EVENT_10 to self in 1 day" );
  status = rtems_timer_fire_when(
    Timer_id[ 1 ],
    &time,
    TA1_send_10_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_when 1 day" );

  time.day = 14;
  puts( "TA1 - rtems_event_send - sending RTEMS_EVENT_11 to self in 2 days" );
  status = rtems_timer_fire_when(
    Timer_id[ 2 ],
    &time,
    TA1_send_11_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_when 2 days" );

  build_time( &time, 2, 12, 1988, 7, 15, 0, 0 );

  print_time( "TA1 - rtems_clock_set - ", &time, "\n" );
  puts( "TA1 - set time backwards" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  status  = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  if ( eventout )
    printf( "ERROR -0x%08" PRIxrtems_event_set " events received\n", eventout );
  else
    puts( "TA1 - no events received" );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_event_receive all events"
  );

  build_time( &time, 2, 14, 1988, 7, 15, 0, 0 );

  print_time( "TA1 - rtems_clock_set - ", &time, "\n" );
  puts( "TA1 - set time forwards (leave a timer)" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  status  = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  if ( eventout == RTEMS_EVENT_10 )
    puts( "TA1 - RTEMS_EVENT_10 received" );
  else
    printf( "ERROR -0x%08" PRIxrtems_event_set " events received\n", eventout );
  directive_failed( status, "rtems_event_receive all events" );

  puts( "TA1 - rtems_event_send - sending RTEMS_EVENT_11 to self in 100 ticks");
  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    100,
    TA1_send_11_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 100 ticks" );

  puts( "TA1 - rtems_event_send - sending RTEMS_EVENT_11 to self in 200 ticks");
  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    200,
    TA1_send_11_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after 200 ticks" );

  /***** *****/
  puts( "TA1 - rtems_event_send - send RTEMS_EVENT_4 to self" );
  status = rtems_event_send( RTEMS_SELF, RTEMS_EVENT_4 );
  directive_failed( status, "rtems_event_send" );

  eventout = 0;
  puts(
    "TA1 - rtems_event_receive - RTEMS_EVENT_4 AND RTEMS_EVENT_5 - UNSATISFIED"
  );
  status  = rtems_event_receive(
    RTEMS_EVENT_4 | RTEMS_EVENT_5,
    RTEMS_NO_WAIT | RTEMS_EVENT_ALL,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  fatal_directive_status( status, RTEMS_UNSATISFIED, "rtems_event_receive" );
  /***** *****/

  puts( "*** END OF TEST 11 ***" );
  rtems_test_exit( 0 );
}
