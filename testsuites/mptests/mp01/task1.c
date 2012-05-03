/*  Test_task
 *
 *  This task is used for three test tasks.  It obtains its task id and
 *  based upon that id, performs certain actions.
 *
 *       Task_1 delays 5 seconds and deletes itself.
 *       Task_2 delays 10 seconds and then loops until
 *             deleted by the third task.
 *       Task 3 delays 15 seconds, then deletes task 2 and itself.
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

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          tid;
  rtems_time_of_day time;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );

  put_name( Task_name[ task_number( tid ) ], FALSE );
  print_time( " - rtems_clock_get_tod - ", &time, "\n" );

  status = rtems_task_wake_after( task_number( tid ) * 1 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  put_name( Task_name[ task_number( tid ) ], FALSE );
  print_time( " - rtems_clock_get_tod - ", &time, "\n" );

  if ( task_number(tid) == 1 ) {          /* TASK 1 */
    put_name( Task_name[ 1 ], FALSE );
    printf( " - deleting self\n" );
    status = rtems_task_delete( RTEMS_SELF );
    directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
  }
  else if ( task_number(tid) == 2 ) {     /* TASK 2 */
    put_name( Task_name[ 2 ], FALSE );
    printf( " - waiting to be deleted by " );
    put_name( Task_name[ 3 ], TRUE );
    while ( FOREVER );
  }
  else {                                  /* TASK 3 */
    put_name( Task_name[ 3 ], FALSE );
    printf( " - getting TID of " );
    put_name( Task_name[ 2 ], TRUE );
    do {
      status = rtems_task_ident( Task_name[ 2 ], RTEMS_SEARCH_ALL_NODES, &tid );
    } while ( status != RTEMS_SUCCESSFUL );
    directive_failed( status, "rtems_task_ident" );

    put_name( Task_name[ 3 ], FALSE );
    printf( " - deleting " );
    put_name( Task_name[ 2 ], TRUE );
    status = rtems_task_delete( tid );
    directive_failed( status, "rtems_task_delete of Task 2" );

    puts( "*** END OF TEST 1 ***" );
    rtems_test_exit(0);
  }
}
