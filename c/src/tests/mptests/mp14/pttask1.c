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

rtems_task Partition_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32   count;
  rtems_status_code  status;
  rtems_unsigned32   yield_count;
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

  while ( Stop_Test == FALSE ) {
    for ( count=PARTITION_DOT_COUNT ; Stop_Test == FALSE && count ; count-- ) {
      status = rtems_partition_get_buffer( Partition_id[ 1 ], &buffer );
      directive_failed( status, "rtems_partition_get_buffer" );

      status = rtems_partition_return_buffer( Partition_id[ 1 ], buffer );
      directive_failed( status, "rtems_partition_return_buffer" );

      if (Stop_Test == FALSE)
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
