/*  Test_task
 *
 *  This task suspends and resumes a remote task.
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

/*PAGE
 *
 *  Test_Task_Support
 *
 */

void Test_Task_Support(
  uint32_t    node
)
{
  rtems_event_set   events;
  rtems_status_code status;

  if ( Multiprocessing_configuration.node == node ) {

    for ( ; ; ) {

      status = rtems_event_receive(
        RTEMS_EVENT_16,
        RTEMS_NO_WAIT,
        RTEMS_NO_TIMEOUT,
        &events
      );

      if ( status == RTEMS_SUCCESSFUL )
        break;

      fatal_directive_status(status, RTEMS_UNSATISFIED, "rtems_event_receive");

      status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
      directive_failed( status, "rtems_task_wake_after" );

      put_name( Task_name[ node ], FALSE );
      puts( " - Suspending remote task" );

      status = rtems_task_suspend( remote_tid );
      directive_failed( status, "rtems_task_suspend" );

      status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
      directive_failed( status, "rtems_task_wake_after" );

      put_name( Task_name[ node ], FALSE );
      puts( " - Resuming remote task" );

      status = rtems_task_resume( remote_tid ) ;
      directive_failed( status, "rtems_task_resume" );

    }

  }  else {

    for ( ; ; ) {
      status = rtems_event_receive(
        RTEMS_EVENT_16,
        RTEMS_NO_WAIT,
        RTEMS_NO_TIMEOUT,
        &events
      );

      if ( status == RTEMS_SUCCESSFUL )
        break;

      fatal_directive_status(status, RTEMS_UNSATISFIED, "rtems_event_receive");

      put_name( Task_name[ remote_node ], FALSE );
      puts( " - have I been suspended???" );

      status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 2 );
      directive_failed( status, "rtems_task_wake_after" );
    }

  }

}

/*PAGE
 *
 *  Test_task
 */

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_id          tid;
  rtems_status_code status;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  puts( "Getting TID of remote task" );
  remote_node = (Multiprocessing_configuration.node == 1) ? 2 : 1;
  printf( "Remote task's name is : " );
  put_name( Task_name[ remote_node ], TRUE );

  do {
      status = rtems_task_ident(
          Task_name[ remote_node ],
          RTEMS_SEARCH_ALL_NODES,
          &remote_tid
          );
  } while ( status != RTEMS_SUCCESSFUL );

  directive_failed( status, "rtems_task_ident" );

  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    5 * rtems_clock_get_ticks_per_second(),
    Delayed_send_event,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  Test_Task_Support( 1 );

  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    5 * rtems_clock_get_ticks_per_second(),
    Delayed_send_event,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  if ( Multiprocessing_configuration.node == 1 ) {
    status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );
  }

  Test_Task_Support( 2 );

  puts( "*** END OF TEST 3 ***" );
  rtems_test_exit( 0 );
}
