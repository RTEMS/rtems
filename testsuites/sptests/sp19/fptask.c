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
#include "fptest.h"
#include "inttest.h"

rtems_task FP_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          tid;
  rtems_time_of_day time;
  uint32_t          task_index;
  uint32_t          previous_seconds;
  INTEGER_DECLARE;
  FP_DECLARE;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident of self" );

  task_index = task_number( tid );

  INTEGER_LOAD( INTEGER_factors[ task_index ] );
  FP_LOAD( FP_factors[ task_index ] );

  put_name( Task_name[ task_index ], FALSE );
  printf(
    " - integer base = (0x%" PRIx32 ")\n",
    INTEGER_factors[ task_index ]
  );
  put_name( Task_name[ task_index ], FALSE );
  #if ( RTEMS_HAS_HARDWARE_FP == 1 )
    printf( " - float base = (%g)\n", FP_factors[ task_index ] );
  #else
    printf( " - float base = (NA)\n" );
  #endif

  previous_seconds = (uint32_t)-1;

  while( FOREVER ) {

    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod" );

    if ( time.second >= 16 ) {
      if ( task_number( tid ) == 4 ) {
        puts( "TA4 - rtems_task_delete - self" );
        status = rtems_task_delete( RTEMS_SELF );
        directive_failed( status, "rtems_task_delete of TA4" );
      }
      puts( "TA5 - rtems_task_delete - TA3" );
      status = rtems_task_delete( Task_id[ 3 ] );
      directive_failed( status, "rtems_task_delete of TA3" );

      puts( "*** END OF TEST 19 *** " );
      rtems_test_exit( 0 );
    }

    if (previous_seconds != time.second) {
      put_name( Task_name[ task_index ], FALSE );
      print_time( " - rtems_clock_get_tod - ", &time, "\n" );
      previous_seconds = time.second;
    }

    INTEGER_CHECK( INTEGER_factors[ task_index ] );
    FP_CHECK( FP_factors[ task_index ] );

    /* for the first 4 seconds we spin as fast as possible
     * so that we likely are interrupted
     * After that, we go to sleep for a second at a time
     */
    if (time.second >= 4) {
      status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
      directive_failed( status, "rtems_task_wake_after" );
    }
  }
}
