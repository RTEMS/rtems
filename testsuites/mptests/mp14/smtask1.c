/*  Sm_test_task
 *
 *  This task continuously obtains and releases a global semaphore.
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

extern rtems_multiprocessing_table Multiprocessing_configuration;

rtems_task Semaphore_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32  count;
  rtems_status_code status;
  rtems_unsigned32  yield_count;

  puts( "Getting SMID of semaphore" );
  while ( FOREVER ) {
    status = rtems_semaphore_ident(
      Semaphore_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Semaphore_id[ 1 ]
    );
    if ( status == RTEMS_SUCCESSFUL )
      break;
    puts( "rtems_semaphore_ident FAILED!!" );
  }

  while ( Stop_Test == FALSE ) {
    yield_count = 100;

    for ( count=SEMAPHORE_DOT_COUNT ; Stop_Test == FALSE && count ; count-- ) {
      status = rtems_semaphore_obtain(
        Semaphore_id[ 1 ],
        RTEMS_DEFAULT_OPTIONS,
        RTEMS_NO_TIMEOUT
      );
      directive_failed( status, "rtems_semaphore_obtain" );

      status = rtems_semaphore_release( Semaphore_id[ 1 ] );
      directive_failed( status, "rtems_semaphore_release" );

      if ( Multiprocessing_configuration.node == 1 && --yield_count == 0 ) {
        status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
        directive_failed( status, "rtems_task_wake_after" );

        yield_count = 100;
      }
    }
    put_dot( 's' );
  }

  Exit_test();
}
