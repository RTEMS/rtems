/*  Test_task
 *
 *  This task continuously sends an event to its counterpart on the
 *  other node, and then waits for it to send an event.  The copy
 *  running on node one send the first event.
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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"

#define DOT_COUNT 100

rtems_timer_service_routine Stop_Test_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  Stop_Test = TRUE;
}

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_unsigned32  count;
  rtems_unsigned32  remote_node;
  rtems_id          remote_tid;
  rtems_event_set   event_out;

  Stop_Test = FALSE;

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
  } while ( !rtems_is_status_successful( status ) );

  if ( Multiprocessing_configuration.node == 1 ) {
    puts( "Sending first event to remote task" );
    status = rtems_event_send( remote_tid, RTEMS_EVENT_16 );
    directive_failed( status, "rtems_event_send" );
  }

  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    5 * TICKS_PER_SECOND,
    Stop_Test_TSR,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  while ( Stop_Test == FALSE ) {
    for ( count=DOT_COUNT ; count && (Stop_Test == FALSE) ; count-- ) {
      status = rtems_event_receive(
        RTEMS_EVENT_16,
        RTEMS_DEFAULT_OPTIONS,
        RTEMS_NO_TIMEOUT,
        &event_out
      );
      if ( status == RTEMS_TIMEOUT ) {
        puts( "\nTA1 - RTEMS_TIMEOUT .. probably OK if the other node exits" );
        break;
      } else
        directive_failed( status, "rtems_event_receive" );

      status = rtems_event_send( remote_tid, RTEMS_EVENT_16 );
      directive_failed( status, "rtems_event_send" );
    }
    put_dot('.');
  }

  /*
   * Wait a bit before shutting down so we don't screw up the other node
   * when our MPCI shuts down
   */

  rtems_task_wake_after(10);

  puts( "\n*** END OF TEST 7 ***" );
  rtems_test_exit( 0 );
}
