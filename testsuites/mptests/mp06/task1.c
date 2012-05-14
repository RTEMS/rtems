/*  Test_task
 *
 *  This task tests global event operations.  If running on node one, it
 *  continuously sends events.   If running on node two, it continuously
 *  receives events.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
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

#define DOT_COUNT 25

/*PAGE
 *
 *  Stop_Test_TSR
 */

rtems_timer_service_routine Stop_Test_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  Stop_Test = true;
}

/*PAGE
 *
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

/*PAGE
 *
 *  Test_task
 */

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  uint32_t    count;
  uint32_t    remote_node;
  rtems_id          remote_tid;
  rtems_event_set   event_out;
  rtems_event_set   event_for_this_iteration;

  Stop_Test = false;

  remote_node = (Multiprocessing_configuration.node == 1) ? 2 : 1;
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

  if ( Multiprocessing_configuration.node == 1 )
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

    if ( Multiprocessing_configuration.node == 1 ) {
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
        if ( Multiprocessing_configuration.node == 2 )
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

  if ( Multiprocessing_configuration.node == 2 ) {
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
