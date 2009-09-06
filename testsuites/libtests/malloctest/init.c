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
 *  COPYRIGHT (c) 1989-2009.
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
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <rtems/score/protectedheap.h>

/*
 *  A simple test of realloc
 */
static void test_realloc(void)
{
  void *p1, *p2, *p3, *p4;
  size_t i;
  int sc;

  /* Test growing reallocation "in place" */
  p1 = malloc(1);
  for (i=2 ; i<2048 ; i++) {
    p2 = realloc(p1, i);
    if (p2 != p1)
      printf( "realloc - failed grow in place: "
              "%p != realloc(%p,%d)\n", p1, p2, i);
    p1 = p2;
  }
  free(p1);

  /* Test shrinking reallocation "in place" */
  p1 = malloc(2048);
  for (i=2047 ; i>=1; i--)  {
    p2 = realloc(p1, i);
    if (p2 != p1)
      printf( "realloc - failed shrink in place: "
              "%p != realloc(%p,%d)\n", p1, p2, i);
    p1 = p2;
  }
  free(p1);

  /* Test realloc that should fail "in place", i.e.,
   * fallback to free()-- malloc()
   */
  p1 = malloc(32);
  p2 = malloc(32);
  p3 = realloc(p1, 64);
  if (p3 == p1 || p3 == NULL)
    printf(
      "realloc - failed non-in place: realloc(%p,%d) = %p\n", p1, 64, p3);
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

static void test_heap_default_init()
{
  memset( &TestHeapMemory, 0x7f, TEST_HEAP_SIZE );
  _Heap_Initialize( &TestHeap, TestHeapMemory, TEST_HEAP_SIZE, 0 );
}

static void test_free( void *addr )
{
  rtems_test_assert( _Heap_Free( &TestHeap, addr ) );
}

static void test_heap_cases_1()
{
  void     *p1, *p2, *p3, *p4;
  intptr_t  u1, u2;
  Heap_Resize_status rsc;

  /*
   * Another odd case.  What we are trying to do from Sergei
   * 
   * 32-bit CPU when CPU_ALIGNMENT = 4 (most targets have 8) with the
   * code like this:
   */
  test_heap_default_init();
  p1 = _Heap_Allocate( &TestHeap, 12 );
  p2 = _Heap_Allocate( &TestHeap, 32 );
  p3 = _Heap_Allocate( &TestHeap, 32 );
  test_free( p2 );
  p2 = _Heap_Allocate_aligned( &TestHeap, 8, 28 );
  test_free( p1 );
  test_free( p2 );
  test_free( p3 );

  /*
   *  Odd case in resizing a block.  Again test case outline per Sergei
   */
  test_heap_default_init();
  p1 = _Heap_Allocate( &TestHeap, 32 );
  p2 = _Heap_Allocate( &TestHeap, 8 );
  p3 = _Heap_Allocate( &TestHeap, 32 );
  test_free( p2 );
  rsc = _Heap_Resize_block( &TestHeap, p1, 41, &u1, &u2 );
  /* XXX what should we expect */
  test_free( p3 );
  test_free( p1 );
  
  /*
   *  To tackle a special case of resizing a block in order to cover the 
   *  code in heapresizeblock.c
   *
   *  Re-initialise the heap, so that the blocks created from now on 
   *  are contiguous.
   */
  test_heap_default_init(); 
  puts( "Heap Initialized" );
  p1 = _Heap_Allocate( &TestHeap, 500 );
  rtems_test_assert( p1 != NULL );
  p2 = _Heap_Allocate( &TestHeap, 496 );
  rtems_test_assert( p2 != NULL );
  rsc = _Heap_Resize_block( &TestHeap, p1, 256, &u1, &u2 );
  rtems_test_assert( rsc == HEAP_RESIZE_SUCCESSFUL );
  test_free( p1 );
  test_free( p2 );  
}

#define TEST_DEFAULT_PAGE_SIZE 128

static void test_heap_init(uintptr_t page_size )
{
  uintptr_t rv = 0;

  memset( &TestHeapMemory, 0x7f, TEST_HEAP_SIZE );

  rv = _Heap_Initialize( &TestHeap, TestHeapMemory, TEST_HEAP_SIZE, page_size );
  rtems_test_assert( rv > 0 );
}

static void test_check_alloc(
  void *alloc_begin_ptr,
  void *expected_alloc_begin_ptr,
  uintptr_t alloc_size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  uintptr_t const min_block_size = TestHeap.min_block_size;
  uintptr_t const page_size = TestHeap.page_size;

  rtems_test_assert( alloc_begin_ptr == expected_alloc_begin_ptr );

  if( expected_alloc_begin_ptr != NULL ) {
    uintptr_t const alloc_begin = (uintptr_t ) alloc_begin_ptr;
    uintptr_t const alloc_end = alloc_begin + alloc_size;

    uintptr_t const alloc_area_begin = _Heap_Align_down( alloc_begin, page_size );
    uintptr_t const alloc_area_offset = alloc_begin - alloc_area_begin;
    uintptr_t const alloc_area_size = alloc_area_offset + alloc_size;

    Heap_Block *block = _Heap_Block_of_alloc_area( alloc_area_begin, page_size );
    uintptr_t const block_begin = (uintptr_t ) block;
    uintptr_t const block_size = _Heap_Block_size( block );
    uintptr_t const block_end = block_begin + block_size;

    rtems_test_assert( block_size >= min_block_size );
    rtems_test_assert( block_begin < block_end );
    rtems_test_assert(
      _Heap_Is_aligned( block_begin + HEAP_BLOCK_HEADER_SIZE, page_size )
    );
    rtems_test_assert(
      _Heap_Is_aligned( block_size, page_size )
    );

    rtems_test_assert( alloc_end <= block_end + HEAP_BLOCK_SIZE_OFFSET );
    rtems_test_assert( alloc_area_begin > block_begin );
    rtems_test_assert( alloc_area_offset < page_size );

    rtems_test_assert( _Heap_Is_aligned( alloc_area_begin, page_size ) );
    if ( alignment == 0 ) {
      rtems_test_assert( alloc_begin == alloc_area_begin );
    } else {
      rtems_test_assert( _Heap_Is_aligned( alloc_begin, alignment ) );
    }

    if ( boundary != 0 ) {
      uintptr_t boundary_line = _Heap_Align_down( alloc_end, boundary );

      rtems_test_assert( alloc_size <= boundary );
      rtems_test_assert(
        boundary_line <= alloc_begin
          || alloc_end <= boundary_line
      );
    }
  }

  rtems_test_assert(
    page_size < CPU_ALIGNMENT
      || _Heap_Walk( &TestHeap, 0, false )
  );
}

static void test_check_alloc_simple(
  void *alloc_begin_ptr,
  uintptr_t alloc_size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  test_check_alloc(
    alloc_begin_ptr,
    alloc_begin_ptr,
    alloc_size,
    alignment,
    boundary
  );
}

static void *test_alloc(
  uintptr_t alloc_size,
  uintptr_t alignment,
  uintptr_t boundary,
  void *expected_alloc_begin_ptr
)
{
  void *alloc_begin_ptr = _Heap_Allocate_aligned_with_boundary(
    &TestHeap,
    alloc_size,
    alignment,
    boundary
  );

  test_check_alloc(
    alloc_begin_ptr,
    expected_alloc_begin_ptr,
    alloc_size,
    alignment,
    boundary
  );

  return alloc_begin_ptr;
}

static void *test_alloc_simple(
  uintptr_t alloc_size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  void *alloc_begin_ptr = _Heap_Allocate_aligned_with_boundary(
    &TestHeap,
    alloc_size,
    alignment,
    boundary
  );

  test_check_alloc_simple(
    alloc_begin_ptr,
    alloc_size,
    alignment,
    boundary
  );

  rtems_test_assert( alloc_begin_ptr != NULL );

  return alloc_begin_ptr;
}

static void *test_init_and_alloc(
  uintptr_t alloc_size,
  uintptr_t alignment,
  uintptr_t boundary,
  void *expected_alloc_begin_ptr
)
{
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );

  return test_alloc(
    alloc_size,
    alignment,
    boundary,
    expected_alloc_begin_ptr
  );
}

static void *test_init_and_alloc_simple(
  uintptr_t alloc_size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );

  return test_alloc_simple(
    alloc_size,
    alignment,
    boundary
  );
}

static uintptr_t test_page_size(void)
{
  return TestHeap.page_size;
}

static void test_heap_cases_2()
{
  void *p1 = NULL;
  void *p2 = NULL;
  void *p3 = NULL;
  uintptr_t alloc_size = 0;
  uintptr_t alignment = 0;
  uintptr_t boundary = 0;
  uintptr_t page_size = 0;
  uintptr_t first_page_begin = 0;
  uintptr_t previous_last_block_begin = 0;
  uintptr_t previous_last_page_begin = 0;

  uintptr_t first_block_begin = 0;

  uintptr_t last_block_begin = 0;
  uintptr_t last_alloc_begin = 0;

  test_heap_init( TEST_DEFAULT_PAGE_SIZE );

  first_block_begin = (uintptr_t) TestHeap.first_block;

  last_block_begin = (uintptr_t) TestHeap.last_block;
  last_alloc_begin = _Heap_Alloc_area_of_block( TestHeap.last_block );

  puts( "run tests for _Heap_Allocate_aligned_with_boundary()");

  puts( "\tcheck if NULL will be returned if size causes integer overflow" );

  alloc_size = (uintptr_t ) -1;
  alignment = 0;
  boundary = 0;
  test_init_and_alloc( alloc_size, alignment, boundary, NULL );

  puts( "\ttry to allocate more space than the one which fits in the boundary" );

  alloc_size = 2;
  alignment = 0;
  boundary = alloc_size - 1;
  test_init_and_alloc( alloc_size, alignment, boundary, NULL );

  puts( "\tcheck if alignment will be set to page size if only a boundary is given" );

  alloc_size = 1;
  boundary = 1;

  alignment = 0;
  p1 = test_init_and_alloc_simple( alloc_size, alignment, boundary );

  alignment = test_page_size();
  test_init_and_alloc( alloc_size, alignment, boundary, p1 );

  puts( "\tcreate a block which is bigger then the first free space" );

  alignment = 0;
  boundary = 0;

  alloc_size = test_page_size();
  p1 = test_init_and_alloc_simple( alloc_size, alignment, boundary );
  p2 = test_alloc_simple( alloc_size, alignment, boundary );
  test_free( p1 );

  alloc_size = 2 * alloc_size;
  p3 = test_alloc_simple( alloc_size, alignment, boundary );
  rtems_test_assert( p1 != p3 );

  puts( "\tset boundary before allocation begin" );

  alloc_size = 1;
  alignment = 0;
  boundary = last_alloc_begin - test_page_size();
  p1 = test_init_and_alloc_simple( alloc_size, alignment, boundary );
  rtems_test_assert( (uintptr_t ) p1 >= boundary );

  puts( "\tset boundary between allocation begin and end" );
  alloc_size = test_page_size();
  alignment = 0;
  boundary = last_alloc_begin - alloc_size / 2;
  p1 = test_init_and_alloc_simple( alloc_size, alignment, boundary );
  rtems_test_assert( (uintptr_t ) p1 + alloc_size <= boundary );

  puts( "\tset boundary after allocation end" );
  alloc_size = 1;
  alignment = 0;
  boundary = last_alloc_begin;
  p1 = test_init_and_alloc_simple( alloc_size, alignment, boundary );
  rtems_test_assert( (uintptr_t ) p1 + alloc_size < boundary );

  puts( "\tset boundary on allocation end" );
  alloc_size = TEST_DEFAULT_PAGE_SIZE - HEAP_BLOCK_HEADER_SIZE;
  alignment = 0;
  boundary = last_block_begin;
  p1 = (void *) (last_alloc_begin - TEST_DEFAULT_PAGE_SIZE);
  test_init_and_alloc( alloc_size, alignment, boundary, p1);

  puts( "\talign the allocation to different positions in the block header" );

  page_size = sizeof(uintptr_t);
  alloc_size = 1;
  boundary = 0;

  test_heap_init( page_size );

  /* Force the page size to a small enough value */
  TestHeap.page_size = page_size;
  
  alignment = first_page_begin - sizeof(uintptr_t);
  p1 = test_alloc( alloc_size, alignment, boundary, NULL );
  
  first_page_begin = ((uintptr_t) TestHeap.first_block ) + HEAP_BLOCK_HEADER_SIZE;
  alignment = first_page_begin + sizeof(uintptr_t);
  p1 = test_alloc( alloc_size, alignment, boundary, NULL );

  first_page_begin = ((uintptr_t) TestHeap.first_block ) 
	  + HEAP_BLOCK_HEADER_SIZE;
  alignment = first_page_begin;
  p1 = test_alloc_simple( alloc_size, alignment, boundary );
  
  puts( "\tallocate last block with different boundarys" );
  page_size = TEST_DEFAULT_PAGE_SIZE;
  test_heap_init( page_size );
  previous_last_block_begin = ((uintptr_t) TestHeap.last_block ) 
	  - TestHeap.min_block_size;
  previous_last_page_begin = previous_last_block_begin
	  + HEAP_BLOCK_HEADER_SIZE;
  alloc_size = TestHeap.page_size - HEAP_BLOCK_HEADER_SIZE;
  alignment = sizeof(uintptr_t);
  boundary = 0;
  p1 = test_alloc( alloc_size, alignment, boundary, (void *) (previous_last_page_begin + sizeof(uintptr_t)));
  
  test_heap_init( page_size );
  boundary = ((uintptr_t) TestHeap.last_block );
  p1 = test_alloc( alloc_size, alignment, boundary, (void *) previous_last_page_begin );

  puts( "\tbreak the boundaries and aligns more than one time" );

  page_size = CPU_ALIGNMENT * 20;
  alloc_size = page_size / 4;
  alignment = page_size / 5;
  boundary = page_size / 4;
  test_heap_init( page_size );
  p1 = (void *) (_Heap_Alloc_area_of_block( TestHeap.last_block ) - page_size );
  test_alloc( alloc_size, alignment, boundary, p1);

  puts( "\tdifferent combinations, so that there is no valid block at the end" );

  page_size = sizeof(uintptr_t);

  test_heap_init( 0 );

  /* Force the page size to a small enough value */
  TestHeap.page_size = page_size;

  alloc_size = 1;
  alignment = (uintptr_t) TestHeap.last_block;
  boundary = 0;
  p1 = test_alloc( alloc_size, alignment, boundary, NULL );

  boundary = (uintptr_t) TestHeap.last_block;
  p1 = test_alloc( alloc_size, alignment, boundary, NULL );
  
  alloc_size = 0;
  p1 = test_alloc( alloc_size, alignment, boundary, NULL );
  
  alloc_size = 1;
  alignment = sizeof(uintptr_t);
  boundary = 0;
  p1 = test_alloc_simple( alloc_size, alignment, boundary );
    
  puts( "\ttry to create a block, which is not possible because of the alignment and boundary" );

  alloc_size = 2;
  boundary = _Heap_Alloc_area_of_block( TestHeap.first_block )
	  + _Heap_Block_size( TestHeap.first_block ) / 2;
  alignment = boundary - 1;
  p1 = test_init_and_alloc( alloc_size, alignment, boundary, NULL );

  alloc_size = 2;
  alignment = _Heap_Alloc_area_of_block( TestHeap.first_block );
  boundary = alignment + 1;
  p1 = test_init_and_alloc( alloc_size, alignment, boundary, NULL );
}

static void test_block_alloc( uintptr_t alloc_begin, uintptr_t alloc_size )
{
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );

  _Heap_Block_allocate(
    &TestHeap,
    TestHeap.first_block,
    alloc_begin,
    alloc_size
  );

  test_check_alloc_simple( (void *) alloc_begin, alloc_size, 0, 0 );
}

static void test_heap_cases_block_allocate()
{
  uintptr_t alloc_begin = 0;
  uintptr_t alloc_size = 0;
  uintptr_t alloc_box_begin = 0;
  uintptr_t alloc_box_end = 0;
  uintptr_t alloc_box_size = 0;

  test_heap_init( TEST_DEFAULT_PAGE_SIZE );

  alloc_box_begin = _Heap_Alloc_area_of_block( TestHeap.first_block );
  alloc_box_size = _Heap_Block_size( TestHeap.first_block );
  alloc_box_end = alloc_box_begin + alloc_box_size;

  puts( "run tests for _Heap_Block_allocate()" );

  puts( "\tallocate block at the beginning");
  alloc_begin = alloc_box_begin;
  alloc_size = 0;
  test_block_alloc( alloc_begin, alloc_size );
  
  puts( "\tallocate block full space");
  alloc_begin = alloc_box_begin;
  alloc_size = alloc_box_size + HEAP_BLOCK_SIZE_OFFSET
    - HEAP_BLOCK_HEADER_SIZE;
  test_block_alloc( alloc_begin, alloc_size );

  puts( "\tallocate block in the middle");
  alloc_begin = alloc_box_begin + TEST_DEFAULT_PAGE_SIZE;
  alloc_size = 0;
  test_block_alloc( alloc_begin, alloc_size );
  
  puts( "\tallocate block at the end");
  alloc_begin = alloc_box_end - TEST_DEFAULT_PAGE_SIZE;
  alloc_size = TEST_DEFAULT_PAGE_SIZE + HEAP_BLOCK_SIZE_OFFSET
    - HEAP_BLOCK_HEADER_SIZE;
  test_block_alloc( alloc_begin, alloc_size );
}

static void test_heap_extend()
{
  void     *p1, *p2, *p3, *p4;
  uint32_t  u1, u2;
  bool      ret;

  /*
   * Easier to hit extend with a dedicated heap.
   * 
   */
  _Heap_Initialize( &TestHeap, TestHeapMemory, 512, 0 );

  puts( "heap extend - bad address" );
  ret = _Protected_heap_Extend( &TestHeap, TestHeapMemory - 512, 512 );
  rtems_test_assert( ret == false );

  puts( "heap extend - OK" );
  ret = _Protected_heap_Extend( &TestHeap, &TestHeapMemory[ 512 ], 512 );
  rtems_test_assert( ret == true );
}

static void test_heap_info(void)
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
  rtems_test_assert( sc == 0 );
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

static void test_protected_heap_info(void)
{
  Heap_Control           heap;
  Heap_Information_block info;
  bool                   rc;

  puts( "_Protected_heap_Get_information - NULL heap" );
  rc = _Protected_heap_Get_information( NULL, &info );
  rtems_test_assert( rc == false );

  puts( "_Protected_heap_Get_information - NULL info" );
  rc = _Protected_heap_Get_information( &heap, NULL );
  rtems_test_assert( rc == false );
}

static void test_heap_resize(void)
{
  Heap_Resize_status  rc;
  void               *p1;
  intptr_t            oldsize;
  intptr_t            avail;

  puts( "Initialize Test Heap" );
  test_heap_default_init();
  
  puts( "Allocate most of heap" );
  p1 = _Heap_Allocate( &TestHeap, TEST_HEAP_SIZE - 32 );
  rtems_test_assert( p1 != NULL );

  puts( "Resize (shrink) the area to 8 bytes to ensure remainder gets freed" );
  rc = _Heap_Resize_block( &TestHeap, p1, 8, &oldsize, &avail );
  rtems_test_assert( rc == HEAP_RESIZE_SUCCESSFUL );
}

/*
 *  A simple test of posix_memalign
 */
static void test_posix_memalign(void)
{
  void *p1, *p2;
  int i;
  int sc;
  int maximumShift;

  puts( "posix_memalign - NULL return pointer -- EINVAL" );
  sc = posix_memalign( NULL, 32, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign NULL pointer" );

  puts( "posix_memalign - alignment of 0 -- EINVAL" );
  sc = posix_memalign( &p1, 0, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign alignment of 0" );

  puts( "posix_memalign - alignment  of 2-- EINVAL" );
  sc = posix_memalign( &p1, 2, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign alignment of 2" );

  if ( sizeof(int) == 4 )
    maximumShift = 31;
  else if ( sizeof(int) == 2 )
    maximumShift = 15;
  else {
    printf( "Unsupported int size == %d\n", sizeof(int) );
    rtems_test_exit(0);
  }
  for ( i=2 ; i<maximumShift ; i++ ) {
    printf( "posix_memalign - alignment of %" PRId32 " -- OK\n",
      (int32_t) 1 << i );
    sc = posix_memalign( &p1, 1 << i, 8 );
    if ( sc == ENOMEM ) {
      printf( "posix_memalign - ran out of memory trying %d\n", 1<<i );
      break;
    }
    posix_service_failed( sc, "posix_memalign alignment OK" );

    free( p1 );
  }
  for ( ; i<maximumShift ; i++ ) {
    printf( "posix_memalign - alignment of %" PRId32 " -- SKIPPED\n",
      (int32_t) 1 << i );
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

  test_heap_cases_2();
  test_heap_cases_block_allocate();
  test_realloc();
  test_heap_cases_1();
  test_heap_extend();
  test_heap_info();
  test_protected_heap_info();
  test_heap_resize();

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
