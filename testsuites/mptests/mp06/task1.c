/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#define DOT_COUNT 25

/*
 *  Stop_Test_TSR
 */
static rtems_timer_service_routine Stop_Test_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  (void) ignored_id;
  (void) ignored_address;

  Stop_Test = true;
}

/*
 *  Event_set_table
 */
rtems_event_set Event_set_table[] = {
  RTEMS_EVENT_0,
  RTEMS_EVENT_1,
  RTEMS_EVENT_2,
  RTEMS_EVENT_3,
  RTEMS_EVENT_4,
  RTEMS_EVENT_5,
  RTEMS_EVENT_6,
  RTEMS_EVENT_7,
  RTEMS_EVENT_8,
  RTEMS_EVENT_9,
  RTEMS_EVENT_10,
  RTEMS_EVENT_11,
  RTEMS_EVENT_12,
  RTEMS_EVENT_13,
  RTEMS_EVENT_14,
  RTEMS_EVENT_15,
  RTEMS_EVENT_16,
  RTEMS_EVENT_17,
  RTEMS_EVENT_18,
  RTEMS_EVENT_19,
  RTEMS_EVENT_20,
  RTEMS_EVENT_21,
  RTEMS_EVENT_22,
  RTEMS_EVENT_23,
  RTEMS_EVENT_24,
  RTEMS_EVENT_25,
  RTEMS_EVENT_26,
  RTEMS_EVENT_27,
  RTEMS_EVENT_28,
  RTEMS_EVENT_29,
  RTEMS_EVENT_30,
  RTEMS_EVENT_31
};

/*
 *  Test_task
 */
rtems_task Test_task(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  uint32_t    count;
  uint32_t    remote_node;
  rtems_id          remote_tid;
  rtems_event_set   event_out;
  rtems_event_set   event_for_this_iteration;

  Stop_Test = false;

  remote_node = (rtems_object_get_local_node() == 1) ? 2 : 1;
  puts_nocr( "Remote task's name is : " );
  put_name( Task_name[ remote_node ], TRUE );

  puts( "Getting TID of remote task" );
  do {
      status = rtems_task_ident(
          Task_name[ remote_node ],
          RTEMS_SEARCH_ALL_NODES,
          &remote_tid
          );
  } while ( status != RTEMS_SUCCESSFUL );
  directive_failed( status, "rtems_task_ident FAILED!!" );

  if ( rtems_object_get_local_node() == 1 )
    puts( "Sending events to remote task" );
  else
    puts( "Receiving events from remote task" );

  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    5 * rtems_clock_get_ticks_per_second(),
    Stop_Test_TSR,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  count = 0;

  for ( ; ; ) {
    if ( Stop_Test == true )
      break;

    event_for_this_iteration = Event_set_table[ count % 32 ];

    if ( rtems_object_get_local_node() == 1 ) {
      status = rtems_event_send( remote_tid, event_for_this_iteration );
      directive_failed( status, "rtems_event_send" );

      status = rtems_task_wake_after( 1 );
      directive_failed( status, "rtems_task_wake_after" );
    } else {
      status = rtems_event_receive(
        event_for_this_iteration,
        RTEMS_DEFAULT_OPTIONS,
        1 * rtems_clock_get_ticks_per_second(),
        &event_out
      );
      if ( rtems_are_statuses_equal( status, RTEMS_TIMEOUT ) ) {
        if ( rtems_object_get_local_node() == 2 )
          puts( "\nCorrect behavior if the other node exitted." );
        else
          puts( "\nERROR... node 1 died" );
        break;
      } else
        directive_failed( status, "rtems_event_receive" );
    }

    if ( (count % DOT_COUNT) == 0 )
      put_dot('.');

    count++;
  }

  putchar( '\n' );

  if ( rtems_object_get_local_node() == 2 ) {
    /* Flush events */
    puts( "Flushing RTEMS_EVENT_16" );
    (void) rtems_event_receive(RTEMS_EVENT_16, RTEMS_NO_WAIT, 0, &event_out);

    puts( "Waiting for RTEMS_EVENT_16" );
    status = rtems_event_receive(
      RTEMS_EVENT_16,
      RTEMS_DEFAULT_OPTIONS,
      1 * rtems_clock_get_ticks_per_second(),
      &event_out
    );
    fatal_directive_status( status, RTEMS_TIMEOUT, "rtems_event_receive" );
    puts( "rtems_event_receive - correctly returned RTEMS_TIMEOUT" );
  }
  puts( "*** END OF TEST 6 ***" );
  rtems_test_exit( 0 );
}
