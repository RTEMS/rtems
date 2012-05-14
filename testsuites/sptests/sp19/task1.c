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
#include "inttest.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          tid;
  rtems_time_of_day time;
  uint32_t          task_index;
  INTEGER_DECLARE;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  task_index = task_number( tid );

  INTEGER_LOAD( INTEGER_factors[ task_index ] );

  put_name( Task_name[ task_index ], FALSE );
  printf(
    " - integer base = (0x%" PRIx32 ")\n",
    INTEGER_factors[ task_index ]
  );

  while( FOREVER ) {
    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod" );

    put_name( Task_name[ task_number( tid ) ], FALSE );
    print_time( " - rtems_clock_get_tod - ", &time, "\n" );

    INTEGER_CHECK( INTEGER_factors[ task_index ] );

    status = rtems_task_wake_after(
      ( task_number( tid ) ) * 5 * rtems_clock_get_ticks_per_second()
    );
    directive_failed( status, "rtems_task_wake_after" );
  }
}
