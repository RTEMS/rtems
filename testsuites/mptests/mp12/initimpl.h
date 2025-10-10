/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  (void) argument;

  rtems_status_code  status;
  void              *bufaddr;

  printf(
    "\n\n*** TEST 12 -- NODE %" PRId32 " ***\n",
    rtems_object_get_local_node()
   );

  Task_name[ 1 ] = rtems_build_name( '1', '1', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( '2', '2', '2', ' ' );

  Partition_name[ 1 ] = rtems_build_name( 'P', 'A', 'R', ' ' );

  puts( "Got to initialization task" );

  if ( rtems_object_get_local_node() == 2 )  {
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
