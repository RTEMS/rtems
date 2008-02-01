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
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#define CONFIGURE_INIT
#include "system.h"

#include <stdlib.h>
#include <errno.h>
#include <rtems/score/protectedheap.h>

/*
 *  A simple test of realloc
 */
void test_realloc(void)
{
  void *p1, *p2, *p3, *p4;
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
  if (!sc)
    free(p1);

  /*
   *  Allocate with default alignment coverage
   */
  sc = rtems_memalign( &p4, 0, 8 );
  if ( !sc && p4 )
    free( p4 );

  /*
   * Walk the C Program Heap
   */
  puts( "malloc_walk - normal path" );
  malloc_walk( 1234, 0 );

  puts( "malloc_walk - in critical section path" );
  _Thread_Disable_dispatch();
  malloc_walk( 1234, 0 );
  _Thread_Enable_dispatch();

  /*
   *  Realloc with a bad pointer to force a point
   */
  p4 = realloc( test_realloc, 32 );
}

#define TEST_HEAP_SIZE 1024
uint8_t TestHeapMemory[TEST_HEAP_SIZE];
Heap_Control TestHeap;

void test_heap_init()
{
  _Heap_Initialize( &TestHeap, TestHeapMemory, TEST_HEAP_SIZE, 0 );
}

void test_heap_cases_1()
{
  void     *p1, *p2, *p3, *p4;
  uint32_t  u1, u2;
  Heap_Resize_status rsc;

  /*
   * Another odd case.  What we are trying to do from Sergei
   * 
   * 32-bit CPU when CPU_ALIGNMENT = 4 (most targets have 8) with the
   * code like this:
   */
  test_heap_init();
  p1 = _Heap_Allocate( &TestHeap, 12 );
  p2 = _Heap_Allocate( &TestHeap, 32 );
  p3 = _Heap_Allocate( &TestHeap, 32 );
  _Heap_Free( &TestHeap, p2 );
  p2 = _Heap_Allocate_aligned( &TestHeap, 8, 28 );
  _Heap_Free( &TestHeap, p1 );
  _Heap_Free( &TestHeap, p2 );
  _Heap_Free( &TestHeap, p3 );

  /*
   *  Odd case in resizing a block.  Again test case outline per Sergei
   */
  test_heap_init();
  p1 = _Heap_Allocate( &TestHeap, 32 );
  p2 = _Heap_Allocate( &TestHeap, 8 );
  p3 = _Heap_Allocate( &TestHeap, 32 );
  _Heap_Free( &TestHeap, p2 );
  rsc = _Heap_Resize_block( &TestHeap, p1, 41, &u1, &u2 );
  /* XXX what should we expect */
  _Heap_Free( &TestHeap, p3 );
  _Heap_Free( &TestHeap, p4 );
}

void test_heap_extend()
{
  void     *p1, *p2, *p3, *p4;
  uint32_t  u1, u2;
  boolean   ret;

  /*
   * Easier to hit extend with a dedicated heap.
   * 
   */
  _Heap_Initialize( &TestHeap, TestHeapMemory, 512, 0 );

  puts( "heap extend - bad address" );
  ret = _Protected_heap_Extend( &TestHeap, TestHeapMemory - 512, 512 );
  rtems_test_assert( ret == FALSE );

  puts( "heap extend - OK" );
  ret = _Protected_heap_Extend( &TestHeap, &TestHeapMemory[ 512 ], 512 );
  rtems_test_assert( ret == TRUE );
}

void test_heap_info(void)
{
  size_t                  s1, s2;
  void                   *p1;
  int                     sc;
  Heap_Information_block  the_info;

  s1 = malloc_free_space();
  p1 = malloc( 512 );
  s2 = malloc_free_space();
  puts( "malloc_free_space - check malloc space drops after malloc" ); 
  rtems_test_assert( s1 );
  rtems_test_assert( s2 );
  rtems_test_assert( s2 <= s1 );
  free( p1 );

  puts( "malloc_free_space - verify free space returns to previous value" );
  s2 = malloc_free_space();
  rtems_test_assert( s1 == s2 );

  puts( "malloc_info - called with NULL\n" );
  sc = malloc_info( NULL );
  rtems_test_assert( sc == -1 );

  puts( "malloc_info - check free space drops after malloc" );
  sc = malloc_info( &the_info );
  rtems_test_assert( sc == -1 );
  s1 = the_info.Free.largest;

  p1 = malloc( 512 );

  sc = malloc_info( &the_info );
  rtems_test_assert( sc == 0 );
  s2 = the_info.Free.largest;

  rtems_test_assert( s1 );
  rtems_test_assert( s2 );
  rtems_test_assert( s2 <= s1 );
  free( p1 );

  puts( "malloc_info - verify free space returns to previous value" );
  sc = malloc_info( &the_info );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( s1 == the_info.Free.largest );

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
  test_heap_cases_1();
  test_heap_extend();
  test_heap_info();

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
