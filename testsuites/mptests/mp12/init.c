/*  Init
 *
 *  This routine is the initialization routine and test code for
 *  global partitions.  It creates a global partition, obtains and
 *  releases a buffer, and deletes the partition.  The partition
 *  is created on node one, and an attempt is made to delete it
 *  by node two.
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
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

#define CONFIGURE_INIT
#include "system.h"

uint8_t   Partition_area[ 1024 ] CPU_STRUCTURE_ALIGNMENT;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code  status;
  void              *bufaddr;

  printf(
    "\n\n*** TEST 12 -- NODE %" PRId32 " ***\n",
    Multiprocessing_configuration.node
   );

  Task_name[ 1 ] = rtems_build_name( '1', '1', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( '2', '2', '2', ' ' );

  Partition_name[ 1 ] = rtems_build_name( 'P', 'A', 'R', ' ' );

  puts( "Got to initialization task" );

  if ( Multiprocessing_configuration.node == 2 )  {
    status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "Getting ID of remote Partition (Global)" );

    do {
      status = rtems_partition_ident(
        Partition_name[ 1 ],
        RTEMS_SEARCH_ALL_NODES,
        &Partition_id[ 1 ]
      );
    } while ( !rtems_is_status_successful( status ) );

    puts( "Attempting to delete remote Partition (Global)" );
    status = rtems_partition_delete( Partition_id[ 1 ] );
    fatal_directive_status(
      status,
      RTEMS_ILLEGAL_ON_REMOTE_OBJECT,
      "rtems_partition_delete"
    );
    puts(
     "rtems_partition_delete correctly returned RTEMS_ILLEGAL_ON_REMOTE_OBJECT"
    );

    puts( "Obtaining a buffer from the global partition" );
    status = rtems_partition_get_buffer( Partition_id[ 1 ], &bufaddr );
    directive_failed( status, "rtems_partition_get_buffer" );
    printf( "Address returned was : 0x%p\n", bufaddr );

    puts( "Releasing a buffer to the global partition" );
    status = rtems_partition_return_buffer( Partition_id[ 1 ], bufaddr );
    directive_failed( status, "rtems_partition_return_buffer" );

    status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );
  }
  else {
    puts( "Creating Partition (Global)" );
    status = rtems_partition_create(
      Partition_name[ 1 ],
      Partition_area,
      128,
      64,
      RTEMS_GLOBAL,
      &Partition_id[ 1 ]
    );
    directive_failed( status, "rtems_partition_create" );

    puts( "Sleeping for two seconds" );
    status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "Deleting Partition (Global)" );
    status = rtems_partition_delete( Partition_id[ 1 ] );
    directive_failed( status, "rtems_partition_delete" );
 }
 puts( "*** END OF TEST 12 ***" );
 rtems_test_exit( 0 );
}
