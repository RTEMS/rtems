/*  Test_task2
 *
 *  This task attempts to receive control of a global semaphore.
 *  If running on the node on which the semaphore resides, the wait is
 *  forever, otherwise it times out on a remote semaphore.
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

rtems_task Test_task2(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "Getting SMID of semaphore" );
  do {
    status = rtems_semaphore_ident(
      Semaphore_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Semaphore_id[ 1 ]
    );
  } while ( !rtems_is_status_successful( status ) );

  directive_failed( status, "rtems_semaphore_ident" );

  if ( Multiprocessing_configuration.node == 1 ) {
    status = rtems_task_wake_after( TICKS_PER_SECOND );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "Releasing semaphore ..." );
    status = rtems_semaphore_release( Semaphore_id[ 1 ] );
    directive_failed( status, "rtems_semaphore_release" );

    status = rtems_task_wake_after( TICKS_PER_SECOND / 2 );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "Getting semaphore ..." );
    status = rtems_semaphore_obtain(
      Semaphore_id[ 1 ],
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    directive_failed( status, "rtems_semaphore_obtain" );

    puts( "Getting semaphore ..." );
    status = rtems_semaphore_obtain(
      Semaphore_id[ 1 ],
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    puts( "How did I get back from here????" );
    directive_failed( status, "rtems_semaphore_obtain" );
  }

/*
  status = rtems_task_wake_after( TICKS_PER_SECOND / 2 );
  directive_failed( status, "rtems_task_wake_after" );
*/

  puts( "Getting semaphore ..." );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain" );

  puts( "Releasing semaphore ..." );
  status = rtems_semaphore_release( Semaphore_id[ 1 ] );
  directive_failed( status, "rtems_semaphore_release" );

  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Getting semaphore ..." );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    2 * TICKS_PER_SECOND
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_semaphore_obtain"
  );
  puts( "rtems_semaphore_obtain correctly returned RTEMS_TIMEOUT" );

  puts( "*** END OF TEST 13 ***" );
  exit( 0 );
}
