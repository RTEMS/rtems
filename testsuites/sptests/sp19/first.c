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

rtems_task First_FP_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          tid;
  rtems_time_of_day time;
  uint32_t    task_index;
  INTEGER_DECLARE;
  FP_DECLARE;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

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

  if ( argument == 0 ) {
    status = rtems_task_restart( RTEMS_SELF, 1 );
    directive_failed( status, "rtems_task_restart of RTEMS_SELF" );
  } else {
    build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
    status = rtems_clock_set( &time );
    directive_failed( status, "rtems_clock_set" );

    status = rtems_task_delete( RTEMS_SELF );
    directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
  }
}
