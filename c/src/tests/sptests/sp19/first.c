/*  First_FP_task
 *
 *  This routine serves as a floating point test task.  It verifies the
 *  basic task switching capabilities of the executive when floating
 *  point is configured.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

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
  rtems_unsigned32  task_index;
  INTEGER_DECLARE;
  FP_DECLARE;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  task_index = task_number( tid );

  INTEGER_LOAD( INTEGER_factors[ task_index ] );
  FP_LOAD( FP_factors[ task_index ] );

  put_name( Task_name[ task_index ], FALSE );
  printf( " - integer base = (0x%x)\n", INTEGER_factors[ task_index ] );
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
