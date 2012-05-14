/*  Task_1_through_3
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
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

rtems_task Task_1_through_3(
  rtems_task_argument argument
)
{
  rtems_id          tid;
  rtems_time_of_day time;
  rtems_status_code status;
  rtems_interval    ticks;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  /*
   * Use TOD_MILLISECONDS_TO_TICKS not RTEMS_MILLISECONDS_TO_TICKS to
   * test C implementation in SuperCore -- not macro version used
   * everywhere else.
   */
  ticks = TOD_MILLISECONDS_TO_TICKS( task_number( tid ) * 5 * 1000 );

  while( FOREVER ) {
    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod" );

    if ( time.second >= 35 ) {
      puts( "*** END OF CBS SCHEDULER TEST 1 ***" );
      rtems_test_exit( 0 );
    }

    put_name( Task_name[ task_number( tid ) ], FALSE );
    print_time( " - rtems_clock_get_tod - ", &time, "\n" );

    status = rtems_task_wake_after( ticks );
    directive_failed( status, "rtems_task_wake_after" );
  }
}
