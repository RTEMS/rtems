/*  Init
 *
 *  This routine is the initialization and test routine for
 *  this test program.  It attempts to create more global
 *  objects than are configured (zero should be configured).
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

#define CONFIGURE_INIT
#include "system.h"

uint8_t   my_partition[0x30000] CPU_STRUCTURE_ALIGNMENT;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          junk_id;
  rtems_status_code status;

  printf(
    "\n\n*** TEST 11 -- NODE %" PRIu32 " ***\n",
    Multiprocessing_configuration.node
  );

  Task_name[ 1 ] = rtems_build_name( '1', '1', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( '2', '2', '2', ' ' );

  Queue_name[ 1 ] = rtems_build_name( 'M', 'S', 'G', ' ' );

  Semaphore_name[ 1 ] = rtems_build_name( 'S', 'E', 'M', ' ' );

  if ( Multiprocessing_configuration.node == 1 ) {
    puts( "Attempting to create Test_task (Global)" );
    status = rtems_task_create(
      Task_name[ 1 ],
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_GLOBAL,
      &junk_id
    );
    fatal_directive_status( status, RTEMS_TOO_MANY, "rtems_task_create" );
    puts( "rtems_task_create correctly returned RTEMS_TOO_MANY" );

    puts( "Attempting to create Message Queue (Global)" );
    status = rtems_message_queue_create(
      Queue_name[ 1 ],
      3,
      16,
      RTEMS_GLOBAL,
      &junk_id
    );
    fatal_directive_status(
      status,
      RTEMS_TOO_MANY,
      "rtems_message_queue_create"
    );
    puts( "rtems_message_queue_create correctly returned RTEMS_TOO_MANY" );

    puts( "Attempting to create Semaphore (Global)" );
    status = rtems_semaphore_create(
      Semaphore_name[ 1 ],
      1,
      RTEMS_GLOBAL,
      RTEMS_NO_PRIORITY,
      &junk_id
    );
    fatal_directive_status( status, RTEMS_TOO_MANY, "rtems_semaphore_create" );
    puts( "rtems_semaphore_create correctly returned RTEMS_TOO_MANY" );

    puts( "Attempting to create Partition (Global)" );
    status = rtems_partition_create(
      1,
      (uint8_t   *) my_partition,
      128,
      64,
      RTEMS_GLOBAL,
      &junk_id
    );
    fatal_directive_status( status, RTEMS_TOO_MANY, "rtems_partition_create" );
    puts( "rtems_partition_create correctly returned RTEMS_TOO_MANY" );
  }
  puts( "*** END OF TEST 11 ***" );
  rtems_test_exit( 0 );
}
