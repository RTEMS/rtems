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
    status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "Releasing semaphore ..." );
    status = rtems_semaphore_release( Semaphore_id[ 1 ] );
    directive_failed( status, "rtems_semaphore_release" );

    status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 2 );
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
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 2 );
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

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Getting semaphore ..." );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    2 * rtems_clock_get_ticks_per_second()
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_semaphore_obtain"
  );
  puts( "rtems_semaphore_obtain correctly returned RTEMS_TIMEOUT" );

  puts( "*** END OF TEST 13 ***" );
  rtems_test_exit( 0 );
}
