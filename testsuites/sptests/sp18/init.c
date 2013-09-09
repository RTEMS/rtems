/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libcsupport.h>

#include <tmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_id                task_id;
  rtems_status_code       sc;
  bool                    ok;
  uintptr_t               stack_size;
  rtems_resource_snapshot snapshot;
  void                   *greedy;

  puts( "\n\n*** TEST 18 ***" );

  greedy = rtems_workspace_greedy_allocate_all_except_largest( &stack_size );
  rtems_resource_snapshot_take( &snapshot );

  /* Make sure the first allocation fails */
  ++stack_size;

  puts( "Init - rtems_task_create - Unsatisfied on Extensions" );
  while (1) {

    sc = rtems_task_create(
      rtems_build_name( 'T', 'E', 'S', 'T' ),
      1,
      stack_size,
      RTEMS_DEFAULT_MODES,
      RTEMS_FLOATING_POINT,
      &task_id
    );

    if ( sc == RTEMS_SUCCESSFUL )
      break;

    fatal_directive_status( sc, RTEMS_UNSATISFIED, "rtems_task_create" );

    /*
     * Verify heap is still in same shape if we couldn't allocate a task
     */
    ok = rtems_resource_snapshot_check( &snapshot );
    rtems_test_assert( ok );

    stack_size -= 8;
    if ( stack_size <= RTEMS_MINIMUM_STACK_SIZE )
     break;
  }

  if ( sc != RTEMS_SUCCESSFUL )
    rtems_test_exit(0);

  /*
   * Verify heap is still in same shape after we free the task
   */
  puts( "Init - rtems_task_delete - OK" );
  sc = rtems_task_delete( task_id );
  directive_failed( sc, "rtems_task_delete" );

  puts( "Init - verify workspace has same memory" );
  ok = rtems_resource_snapshot_check( &snapshot );
  rtems_test_assert( ok );

  rtems_workspace_greedy_free( greedy );

  puts( "*** END OF TEST 18 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            2
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS  20
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
