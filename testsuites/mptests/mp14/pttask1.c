/*  Partition_task
 *
 *  This task continuously gets a buffer from and returns that buffer
 *  to a global partition.
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

extern rtems_multiprocessing_table Multiprocessing_configuration;

rtems_task Partition_task(
  rtems_task_argument argument
)
{
  uint32_t     count;
  rtems_status_code  status;
  uint32_t     yield_count;
  void              *buffer;

  puts( "Getting ID of partition" );
  while ( FOREVER ) {
    status = rtems_partition_ident(
      Partition_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Partition_id[ 1 ]
    );

    if ( status == RTEMS_SUCCESSFUL )
      break;

    puts( "rtems_partition_ident FAILED!!" );
    rtems_task_wake_after(2);
  }

  yield_count = 100;

  while ( Stop_Test == false ) {
    for ( count=PARTITION_DOT_COUNT ; Stop_Test == false && count ; count-- ) {
      status = rtems_partition_get_buffer( Partition_id[ 1 ], &buffer );
      directive_failed( status, "rtems_partition_get_buffer" );

      status = rtems_partition_return_buffer( Partition_id[ 1 ], buffer );
      directive_failed( status, "rtems_partition_return_buffer" );

      if (Stop_Test == false)
        if ( Multiprocessing_configuration.node == 1 && --yield_count == 0 ) {
          status = rtems_task_wake_after( 1 );
          directive_failed( status, "rtems_task_wake_after" );
          yield_count = 100;
        }
    }
    put_dot( 'p' );
  }

  Exit_test();
}
