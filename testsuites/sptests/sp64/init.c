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
  bool                    sb;
  Heap_Information_block  start;
  Heap_Information_block  info;
  size_t                  to_alloc;
  void                   *alloced;

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
  directive_failed( sc, "rtems_region_create of RN1" );

  puts( "Init - rtems_workspace_get_information - OK" );
  sb = rtems_workspace_get_information( &start );
  rtems_test_assert( sb );

  #if 0
    printf( "Init - workspace free = %d\n", start.Free.largest );
    printf( "Init - workspace free blocks = %d\n", start.Free.number );
  #endif
  rtems_test_assert( start.Free.number == 1 );
  to_alloc = start.Free.largest;

  /* find the largest we can actually allocate */
  while ( 1 ) {
    sb = rtems_workspace_allocate( to_alloc, &alloced );
    if ( sb )
      break;
    to_alloc -= 4;
  }

  rtems_workspace_free( alloced );

  #if 0
    printf( "Init - start with to_alloc of = %d\n", to_alloc );
  #endif

  /*
   * Verify heap is still in same shape if we couldn't allocate a region
   */
  sb = rtems_workspace_get_information( &info );
  rtems_test_assert( sb );
  rtems_test_assert( info.Free.largest == start.Free.largest );
  rtems_test_assert( info.Free.number  == start.Free.number  );

  puts( "Init - rtems_region_create - auto-extend - RTEMS_UNSATISFIED" );
  while (1) {

    sb = rtems_workspace_allocate( to_alloc, &alloced );
    rtems_test_assert( sb );

    sc = rtems_region_create(
      rtems_build_name( 'R', 'N', '2', ' ' ),
      Area2,
      sizeof(Area2),
      8,
      RTEMS_DEFAULT_ATTRIBUTES,
      &region2
    );

    /* free the memory we snagged, then check the status */
    rtems_workspace_free( alloced );

    if ( sc == RTEMS_SUCCESSFUL )
      break;

    if ( sc != RTEMS_TOO_MANY ) {
      printf( "region create returned %d or %s\n", sc, rtems_status_text(sc) );
      rtems_test_exit(0);
    }

    /*
     * Verify heap is still in same shape if we couldn't allocate a region
     */
    sb = rtems_workspace_get_information( &info );
    #if 0
      printf( "Init - workspace free/blocks = %d/%d\n",
        info.Free.largest, info.Free.number );
    #endif
    rtems_test_assert( sb );
    rtems_test_assert( info.Free.largest == start.Free.largest );
    rtems_test_assert( info.Free.number  == start.Free.number  );

    to_alloc -= 8;
    if ( to_alloc == 0 )
     break;
  }

  if ( sc )
    rtems_test_exit(0);

  /*
   * Verify heap is still in same shape after we free the region
   */
  puts( "Init - rtems_region_delete - OK" );
  sc = rtems_region_delete( region2 );
  rtems_test_assert( sc == 0 );

  /*
   *  Although it is intuitive that after deleting the region the
   *  object space would shrink and go back to its original shape,
   *  we could end up with fragmentation which prevents a simple
   *  check from verifying this.
   */
  #if 0
    puts( "Init - verify workspace has same memory" );
    sb = rtems_workspace_get_information( &info );
    #if 0
        printf( "Init - workspace free/blocks = %d/%d\n",
      info.Free.largest, info.Free.number );
    #endif
    rtems_test_assert( sb );
    rtems_test_assert( info.Free.largest == start.Free.largest );
    rtems_test_assert( info.Free.number  == start.Free.number  );
  #endif

  puts( "*** END OF TEST 64 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         2
#define CONFIGURE_MAXIMUM_REGIONS       rtems_resource_unlimited(1)
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
