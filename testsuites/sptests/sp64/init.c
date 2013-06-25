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

#include "tmacros.h"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

uint32_t Area1[50] CPU_STRUCTURE_ALIGNMENT;
uint32_t Area2[50] CPU_STRUCTURE_ALIGNMENT;

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_id                region1;
  rtems_id                region2;
  rtems_status_code       sc;
  bool                    ok;
  uintptr_t               to_alloc;
  void                   *alloced;
  rtems_resource_snapshot snapshot;
  void                   *greedy;

  puts( "\n\n*** TEST 64 ***" );

  puts( "Allocate one region -- so second auto extends" );
  sc = rtems_region_create(
    rtems_build_name( 'R', 'N', '1', ' ' ),
    Area1,
    sizeof(Area1),
    8,
    RTEMS_DEFAULT_ATTRIBUTES,
    &region1
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  greedy = rtems_workspace_greedy_allocate_all_except_largest( &to_alloc );
  rtems_resource_snapshot_take( &snapshot );

  puts( "Init - rtems_region_create - auto-extend - RTEMS_UNSATISFIED" );
  while ( to_alloc > 8 ) {
    ok = rtems_workspace_allocate( to_alloc, &alloced );
    rtems_test_assert( ok );

    sc = rtems_region_create(
      rtems_build_name( 'R', 'N', '2', ' ' ),
      Area2,
      sizeof(Area2),
      8,
      RTEMS_DEFAULT_ATTRIBUTES,
      &region2
    );

    rtems_workspace_free( alloced );

    if ( sc == RTEMS_SUCCESSFUL )
      break;

    rtems_test_assert( sc == RTEMS_TOO_MANY );

    /*
     * Verify heap is still in same shape if we couldn't allocate a region
     */
    ok = rtems_resource_snapshot_check( &snapshot );
    rtems_test_assert( ok );

    to_alloc -= 8;
  }

  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  /*
   * Verify heap is still in same shape after we free the region
   */
  puts( "Init - rtems_region_delete - OK" );
  sc = rtems_region_delete( region2 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "Init - verify workspace has same memory" );
  ok = rtems_resource_snapshot_check( &snapshot );
  rtems_test_assert( ok );
  rtems_workspace_greedy_free( greedy );

  puts( "*** END OF TEST 64 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_MAXIMUM_REGIONS       rtems_resource_unlimited( 2 )
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MEMORY_OVERHEAD 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
