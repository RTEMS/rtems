/*  Init
 *
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
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

#include <stdlib.h>
#include <errno.h>

/*
 *  A simple test of realloc
 */
void test_realloc(void)
{
  void *p1, *p2, *p3;
  int i;
  int sc;

  /* Test growing reallocation "in place" */
  p1 = malloc(1);
  for (i=2 ; i<2048 ; i++) {
    p2 = realloc(p1, i);
    if (p2 != p1)
      printf( "realloc - failed grow in place: "
              "%p != realloc(%p,%d)\n", p1, p2, i );
    p1 = p2;
  }
  free(p1);

  /* Test shrinking reallocation "in place" */
  p1 = malloc(2048);
  for (i=2047 ; i>=1; i--)  {
    p2 = realloc(p1, i);
    if (p2 != p1)
      printf( "realloc - failed shrink in place: "
              "%p != realloc(%p,%d)\n", p1, p2, i );
    p1 = p2;
  }
  free(p1);

  /* Test realloc that should fail "in place", i.e.,
   * fallback to free()--malloc()
   */
  p1 = malloc(32);
  p2 = malloc(32);
  p3 = realloc(p1, 64);
  if (p3 == p1 || p3 == NULL)
    printf(
      "realloc - failed non-in place: realloc(%p,%d) = %p\n", p1, 64, p3 );
  free(p3);
  free(p2);

  /*
   *  Yet another case
   */
  p1 = malloc(8);
  p2 = malloc(8);
  free(p1);
  sc = posix_memalign(&p1, 16, 32);
  printf( "sc = %d\n", sc );
  if (!sc)
    free(p1);

  /*
   *  Bad hack to get coverage
   */

  {
    void *p5;
    extern Heap_Control RTEMS_Malloc_Heap;
    p5 = _Protected_heap_Allocate_aligned( &RTEMS_Malloc_Heap, 8, 0 );
    if ( p5 )
      free( p5 );
  }
}

/*
 *  A simple test of posix_memalign
 */
void test_posix_memalign(void)
{
  void *p1, *p2;
  int i;
  int sc;

  puts( "posix_memalign - NULL return pointer -- EINVAL" );
  sc = posix_memalign( NULL, 32, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign NULL pointer" );

  puts( "posix_memalign - alignment of 0 -- EINVAL" );
  sc = posix_memalign( &p1, 0, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign alignment of 0" );

  puts( "posix_memalign - alignment  of 2-- EINVAL" );
  sc = posix_memalign( &p1, 2, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign alignment of 2" );

  for ( i=2 ; i<32 ; i++ ) {
    printf( "posix_memalign - alignment of %d -- OK\n", 1 << i );
    sc = posix_memalign( &p1, 1 << i, 8 );
    if ( sc == ENOMEM ) {
      printf( "posix_memalign - ran out of memory trying %d\n", 1<<i );
      break;
    }
    posix_service_failed( sc, "posix_memalign alignment OK" );

    free( p1 );
  }

}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_status_code status;

  puts( "\n\n*** MALLOC TEST ***" );

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  test_realloc();

  test_posix_memalign();

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] = rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ] = rtems_build_name( 'T', 'A', '5', ' ' );

  status = rtems_task_create(
     Task_name[ 1 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
     &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
     Task_name[ 2 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
     &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
     Task_name[ 3 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
     &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_create(
     Task_name[ 4 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
     &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );

  status = rtems_task_create(
     Task_name[ 5 ],
     1,
     TASK_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
     &Task_id[ 5 ]
  );
  directive_failed( status, "rtems_task_create of TA5" );

  status = rtems_task_start( Task_id[ 1 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  status = rtems_task_start( Task_id[ 4 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  status = rtems_task_start( Task_id[ 5 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA5" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
