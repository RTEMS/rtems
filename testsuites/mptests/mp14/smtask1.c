/*  Sm_test_task
 *
 *  This task continuously obtains and releases a global semaphore.
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
    rtems_task_wake_after(2);
  }

  yield_count = 100;

  while ( Stop_Test == FALSE ) {

    for ( count=SEMAPHORE_DOT_COUNT ; Stop_Test == FALSE && count ; count-- ) {
      status = rtems_semaphore_obtain(
        Semaphore_id[ 1 ],
        RTEMS_DEFAULT_OPTIONS,
        RTEMS_NO_TIMEOUT
      );
      directive_failed( status, "rtems_semaphore_obtain" );

      status = rtems_semaphore_release( Semaphore_id[ 1 ] );
      directive_failed( status, "rtems_semaphore_release" );

      if ( Stop_Test == FALSE )
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
