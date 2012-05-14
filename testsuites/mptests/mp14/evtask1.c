/*  Test_task
 *
 *  This task either continuously sends events to a remote task, or
 *  continuously receives events sent by a remote task.  This decision
 *  is based upon the local node number.
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

extern rtems_multiprocessing_table Multiprocessing_configuration;

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  uint32_t    count;
  uint32_t    remote_node;
  rtems_id          remote_tid;
  rtems_event_set   event_out;

  remote_node = ((Multiprocessing_configuration.node == 1) ? 2 : 1);

  puts( "About to go to sleep!" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );
  puts( "Waking up!" );

  puts_nocr( "Remote task's name is : " );
  put_name( Task_name[ remote_node ], TRUE );

  puts( "Getting TID of remote task" );
  while ( FOREVER ) {
    status = rtems_task_ident(
      Task_name[ remote_node ],
      RTEMS_SEARCH_ALL_NODES,
      &remote_tid
    );

    if ( status == RTEMS_SUCCESSFUL )
      break;
    puts( "rtems_task_ident FAILED!!" );
    rtems_task_wake_after(2);
  }

  if ( Multiprocessing_configuration.node == 1 ) {
    puts( "Sending events to remote task" );
    while ( Stop_Test == false ) {
      for ( count=EVENT_TASK_DOT_COUNT; Stop_Test == false && count; count-- ) {
        status = rtems_event_send( remote_tid, RTEMS_EVENT_16 );
        directive_failed( status, "rtems_event_send" );
     }
     put_dot( 'e' );
    }
  }

  puts( "Receiving events from remote task" );
  while ( Stop_Test == false ) {
    for ( count=EVENT_TASK_DOT_COUNT ; Stop_Test == false && count ; count-- ) {
      status = rtems_event_receive(
        RTEMS_EVENT_16,
        RTEMS_DEFAULT_OPTIONS,
        RTEMS_NO_TIMEOUT,
        &event_out
      );
      directive_failed( status, "rtems_event_receive" );
    }
    put_dot( 'e' );
  }

  Exit_test();
}
