/* SPDX-License-Identifier: BSD-2-Clause */

/* 
 *  COPYRIGHT (c) 1989-2011, 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2009, 2016 embedded brains GmbH & Co. KG
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

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <rtems/score/protectedheap.h>
#include <rtems/malloc.h>
#include <rtems/sysinit.h>
#include <malloc.h>

const char rtems_test_name[] = "MALLOCTEST";

/*
 *  A simple test of realloc
 */
static void test_realloc(void)
{
  void *p1, *p2, *p3, *p4;
  size_t i;
  int sc;
  bool malloc_walk_ok;

  /* Test growing reallocation "in place" */
  p1 = malloc(1);
  for (i=2 ; i<2048 ; i++) {
    p2 = realloc(p1, i);
    if (p2 != p1)
/*
 * This was added to address the following warning.
 * warning: pointer 'p1' may be used after 'realloc'
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
      printf( "realloc - failed grow in place: "
              "%p != realloc(%p,%zu)\n", p1, p2, i);
#pragma GCC diagnostic pop
    p1 = p2;
  }
  free(p1);

  /* Test shrinking reallocation "in place" */
  p1 = malloc(2048);
  for (i=2047 ; i>=1; i--)  {
    p2 = realloc(p1, i);
    if (p2 != p1)
/*
 * This was added to address the following warning.
 * warning: pointer 'p1' may be used after 'realloc'
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
       printf( "realloc - failed shrink in place: "
              "%p != realloc(%p,%zu)\n", p1, p2, i);
#pragma GCC diagnostic pop
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
/*
 * This was added to address the following warning.
 * warning: pointer may be used after 'realloc'
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
     printf(
      "realloc - failed non-in place: realloc(%p,%d) = %p\n", p1, 64, p3);
#pragma GCC diagnostic pop
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
  malloc_walk_ok = malloc_walk( 1234, false );
  rtems_test_assert( malloc_walk_ok );

  puts( "malloc_walk - in critical section path" );
  _Thread_Dispatch_disable();
  malloc_walk_ok = malloc_walk( 1234, false );
  rtems_test_assert( malloc_walk_ok );
  _Thread_Dispatch_enable( _Per_CPU_Get() );

  /*
   *  Realloc with a bad pointer to force a point
   */
/*
 * This was added to address the following warning.
 * warning: 'realloc' called on unallocated object
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
   p4 = realloc( test_realloc, 32 );
#pragma GCC diagnostic pop

  p4 = _realloc_r( NULL, NULL, 1 );
}

#define TEST_HEAP_SIZE 2048

uint8_t TestHeapMemory[TEST_HEAP_SIZE];

Heap_Control TestHeap;

static void test_heap_default_init(void)
{
  memset( &TestHeapMemory, 0x7f, TEST_HEAP_SIZE );
  _Heap_Initialize( &TestHeap, TestHeapMemory, TEST_HEAP_SIZE, 0 );
}

static void test_free( void *addr )
{
  uint32_t failed_allocs;

  rtems_test_assert( _Heap_Free( &TestHeap, addr ) );

  failed_allocs = TestHeap.stats.failed_allocs;
  _Heap_Protection_free_all_delayed_blocks( &TestHeap );
  rtems_test_assert( failed_allocs == TestHeap.stats.failed_allocs );
}

static void test_heap_cases_1(void)
{
  void     *p1, *p2, *p3;
  uintptr_t  u1, u2;
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
  p1 = _Heap_Allocate( &TestHeap, 400 );
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
#if UNUSED
    uintptr_t const alloc_area_size = alloc_area_offset + alloc_size;
#endif
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

    rtems_test_assert( alloc_end <= block_end + HEAP_ALLOC_BONUS );
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

static void test_heap_do_initialize(
  uintptr_t area_size,
  uintptr_t page_size,
  uintptr_t success_expected
)
{
  uintptr_t rv =
    _Heap_Initialize( &TestHeap, TestHeapMemory, area_size, page_size );

  if ( success_expected ) {
    rtems_test_assert( rv > 0 && _Heap_Walk( &TestHeap, 0, false ) );
  } else {
    rtems_test_assert( rv == 0 );
  }
}

static void test_heap_initialize(void)
{
  puts( "run tests for _Heap_Initialize()" );

  test_heap_do_initialize( TEST_HEAP_SIZE, 0, true );

  test_heap_do_initialize( TEST_HEAP_SIZE, TEST_DEFAULT_PAGE_SIZE, true );

  test_heap_do_initialize( 0, 0, false );

  test_heap_do_initialize( (uintptr_t) -1, 0, false );

  test_heap_do_initialize( TEST_HEAP_SIZE, (uintptr_t) -1, false );

  test_heap_do_initialize(
    TEST_HEAP_SIZE,
    (uintptr_t) (-2 * CPU_ALIGNMENT),
    false
  );
}

static void test_heap_allocate(void)
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

  uintptr_t last_block_begin = 0;
  uintptr_t last_alloc_begin = 0;

  test_heap_init( TEST_DEFAULT_PAGE_SIZE );

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
  rtems_test_assert( p2 );

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

static void test_heap_free(void)
{
  Heap_Control *heap = &TestHeap;
  void *p;
  Heap_Block *block;
  bool ok;

  _Heap_Initialize( heap, &TestHeapMemory[0], sizeof(TestHeapMemory), 0 );

  p = _Heap_Allocate( heap, 1 );
  rtems_test_assert( p != NULL );

  block = _Heap_Block_of_alloc_area( (uintptr_t) p, heap->page_size );

  /*
   * This will kick the next block outside of the heap area and the next
   * _Heap_Free() will detect this.
   */
  block->size_and_flag += sizeof(TestHeapMemory);

  ok = _Heap_Free( heap, p );
  rtems_test_assert( !ok );
}

static void *test_create_used_block( void )
{
  uintptr_t const alloc_size = 3 * TEST_DEFAULT_PAGE_SIZE;
  uintptr_t const alignment = 0;
  uintptr_t const boundary = 0;

  return test_alloc_simple( alloc_size, alignment, boundary );
}

static void test_block_alloc(
  int free_variant,
  int alloc_variant,
  uintptr_t alloc_begin,
  uintptr_t alloc_size
)
{
  void *p1 = NULL;
  void *p2 = NULL;
  void *p3 = NULL;

  uintptr_t size_fresh_heap = 0;
  uintptr_t pages_per_default_block = 0;
  uint32_t exp_free_pages = 0;
  uint32_t exp_free_blocks = 0;
  uint32_t exp_used_blocks = 0;

  test_heap_init( TEST_DEFAULT_PAGE_SIZE );

  size_fresh_heap = _Heap_Get_size( &TestHeap );
  exp_free_pages = size_fresh_heap / TestHeap.page_size;

  p1 = test_create_used_block();
  p2 = test_create_used_block();
  p3 = test_create_used_block();

  pages_per_default_block = _Heap_Block_size(
    _Heap_Block_of_alloc_area( (uintptr_t) p1, TestHeap.page_size )
  ) / TestHeap.page_size;

  if (free_variant == 1) {
    test_free( p1 );
  } else if (free_variant == 2) {
    test_free( p3 );
  } else if (free_variant == 3) {
    test_free( p2 );
    test_free( p3 );
  }

  _Heap_Block_allocate(
    &TestHeap,
    _Heap_Block_of_alloc_area( (uintptr_t) p2, test_page_size()),
    alloc_begin,
    alloc_size
  );

  test_check_alloc_simple( (void *) alloc_begin, alloc_size, 0, 0 );

  /* check statistics */
  switch( free_variant ) {
    case 1:
      exp_free_pages = exp_free_pages - 2 * pages_per_default_block;
      exp_used_blocks = 2;

      switch( alloc_variant ) {
	case 1:
	  /* allocate block full space */
	  exp_free_blocks = 2;
	  break;
	case 2:
	  /* allocate block in the middle */
	  exp_free_pages = exp_free_pages + pages_per_default_block - 1;
	  exp_free_blocks = 3;
	  break;
	case 3:
	  /* allocate block at the end */
	  exp_free_pages = exp_free_pages + pages_per_default_block - 2;
	  exp_free_blocks = 2;
	  break;
	default:
	  /* allocate block at the beginning */
	  exp_free_pages = exp_free_pages + pages_per_default_block - 1;
	  exp_free_blocks = 3;
	  break;
      }
      break;
    case 2:
      exp_free_pages = exp_free_pages - 2 * pages_per_default_block;
      exp_used_blocks = 2;

      switch( alloc_variant ) {
	case 1:
	  /* allocate block full space */
	  exp_free_blocks = 1;
	  break;
	case 2:
	  /* allocate block in the middle */
	  exp_free_pages = exp_free_pages + pages_per_default_block - 1;
	  exp_free_blocks = 2;
	  break;
	case 3:
	  /* allocate block at the end */
	  exp_free_pages = exp_free_pages + pages_per_default_block - 1;
	  exp_free_blocks = 2;
	  break;
	default:
	  /* allocate block at the beginning */
	  exp_free_pages = exp_free_pages + pages_per_default_block - 1;
	  exp_free_blocks = 1;
	  break;
      }
      break;
    case 3:
      exp_free_pages = exp_free_pages - pages_per_default_block;
      exp_used_blocks = 2;

      switch( alloc_variant ) {
	case 1:
	  /* allocate block full space */
	  exp_free_pages = exp_free_pages - pages_per_default_block;
	  exp_free_blocks = 1;
	  break;
	case 2:
	  /* allocate block in the middle */
	  exp_free_pages = exp_free_pages - 1;
	  exp_free_blocks = 2;
	  break;
	case 3:
	  /* allocate block at the end */
	  exp_free_pages = exp_free_pages - 2;
	  exp_free_blocks = 2;
	  break;
	default:
	  /* allocate block at the beginning */
	  exp_free_pages = exp_free_pages - 1;
	  exp_free_blocks = 1;
	  break;
      }
      break;
    default:
      exp_free_pages = exp_free_pages - 3 * pages_per_default_block;
      exp_used_blocks = 3;

      switch( alloc_variant ) {
	case 1:
	  /* allocate block full space */
	  exp_free_blocks = 1;
	  break;
	case 2:
	  /* allocate block in the middle */
	  exp_free_blocks = 3;
	  exp_free_pages = exp_free_pages + pages_per_default_block - 1;
	  break;
	case 3:
	  /* allocate block at the end */
	  exp_free_blocks = 2;
	  exp_free_pages = exp_free_pages + pages_per_default_block - 1;
	  break;
	default:
	  /* allocate block at the beginning */
	  exp_free_blocks = 2;
	  exp_free_pages = exp_free_pages + pages_per_default_block - 1;
      }
  }

  rtems_test_assert( TestHeap.stats.free_size == exp_free_pages * TestHeap.page_size );
  rtems_test_assert( TestHeap.stats.free_blocks == exp_free_blocks );
  rtems_test_assert( TestHeap.stats.used_blocks == exp_used_blocks );
}

static void test_heap_do_block_allocate( int variant, void *p2 )
{
  Heap_Block *const block =
    _Heap_Block_of_alloc_area( (uintptr_t) p2, test_page_size());
  uintptr_t const alloc_box_begin = _Heap_Alloc_area_of_block( block );
  uintptr_t const alloc_box_size = _Heap_Block_size( block );
  uintptr_t const alloc_box_end = alloc_box_begin + alloc_box_size;
  uintptr_t alloc_begin = 0;
  uintptr_t alloc_size = 0;

  puts( "\tallocate block at the beginning");
  alloc_begin = alloc_box_begin;
  alloc_size = 0;
  test_block_alloc( variant, 0, alloc_begin, alloc_size );

  puts( "\tallocate block full space");
  alloc_begin = alloc_box_begin;
  alloc_size = alloc_box_size + HEAP_ALLOC_BONUS
    - HEAP_BLOCK_HEADER_SIZE;
  test_block_alloc( variant, 1, alloc_begin, alloc_size );

  puts( "\tallocate block in the middle");
  alloc_begin = alloc_box_begin + TEST_DEFAULT_PAGE_SIZE;
  alloc_size = 0;
  test_block_alloc( variant, 2, alloc_begin, alloc_size );

  puts( "\tallocate block at the end");
  alloc_begin = alloc_box_end - TEST_DEFAULT_PAGE_SIZE;
  alloc_size = TEST_DEFAULT_PAGE_SIZE + HEAP_ALLOC_BONUS
    - HEAP_BLOCK_HEADER_SIZE;
  test_block_alloc( variant, 3, alloc_begin, alloc_size );
}

static void test_heap_block_allocate( void )
{
  void *p2 = NULL;

  puts( "run tests for _Heap_Block_allocate()" );

  test_heap_init( TEST_DEFAULT_PAGE_SIZE );

  test_create_used_block();
  p2 = test_create_used_block();

  test_heap_do_block_allocate( 0, p2 );
  test_heap_do_block_allocate( 1, p2 );
  test_heap_do_block_allocate( 2, p2 );
  test_heap_do_block_allocate( 3, p2 );
}

static void *test_alloc_one_page(void)
{
  void *alloc_begin_ptr = _Heap_Allocate_aligned_with_boundary(
    &TestHeap,
    1,
    0,
    0
  );

  test_check_alloc_simple(
    alloc_begin_ptr,
    1,
    0,
    0
  );

  rtems_test_assert( alloc_begin_ptr != NULL );

  return alloc_begin_ptr;
}

static void *test_alloc_two_pages(void)
{
  void *alloc_begin_ptr = _Heap_Allocate_aligned_with_boundary(
    &TestHeap,
    3 * TestHeap.page_size / 2,
    0,
    0
  );

  test_check_alloc_simple(
    alloc_begin_ptr,
    3 * TestHeap.page_size / 2,
    0,
    0
  );

  rtems_test_assert( alloc_begin_ptr != NULL );

  return alloc_begin_ptr;
}

static void test_simple_resize_block(
  void *alloc_pointer,
  uintptr_t new_alloc_size,
  Heap_Resize_status expected_status
)
{
  uintptr_t old_size = 0;
  uintptr_t new_size = 0;

  Heap_Resize_status status = _Heap_Resize_block(
    &TestHeap,
    alloc_pointer,
    new_alloc_size,
    &old_size,
    &new_size
  );

  rtems_test_assert( status == expected_status );
}

static void test_heap_resize_block(void)
{
  void *p1, *p2, *p3;
  uintptr_t new_alloc_size = 0;
  Heap_Block *block = NULL;

  puts( "run tests for _Heap_Resize_Block()" );

  puts( "\tgive a block outside the heap to the function" );
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );
  p1 = TestHeap.first_block - TEST_DEFAULT_PAGE_SIZE;
  new_alloc_size = 1;
  test_simple_resize_block( p1, new_alloc_size, HEAP_RESIZE_FATAL_ERROR );

  puts( "\tincrease size");

  puts( "\t\tlet the next block be used alredy and try to get a size bigger than the actual block" );
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );
  p1 = test_alloc_one_page();
  rtems_test_assert( p1 );

  p2 = test_alloc_one_page();
  rtems_test_assert( p2 );

  new_alloc_size = 3 * TEST_DEFAULT_PAGE_SIZE / 2;
  test_simple_resize_block( p1, new_alloc_size, HEAP_RESIZE_UNSATISFIED );

  puts( "\t\tnext block not used and try to set the new allocation size between the page-alignments" );
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );
  p1 = test_alloc_one_page();
  new_alloc_size = 3 * TEST_DEFAULT_PAGE_SIZE / 2;
  test_simple_resize_block( p1, new_alloc_size, HEAP_RESIZE_SUCCESSFUL );

  puts( "\t\tlet the block after the next be used and try to allocate more then one pagesize more" );
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );
  p1 = test_alloc_one_page();
  rtems_test_assert( p1 );

  p2 = test_alloc_one_page();
  rtems_test_assert( p2 );

  p3 = test_alloc_one_page();
  rtems_test_assert( p3 );

  test_free( p2 );
  new_alloc_size = 5 * TEST_DEFAULT_PAGE_SIZE / 2;
  test_simple_resize_block( p1, new_alloc_size, HEAP_RESIZE_UNSATISFIED );

  puts( "\ttry to resize to the same size" );
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );
  p1 = test_alloc_one_page();
  block = _Heap_Block_of_alloc_area( (uintptr_t) p1, TestHeap.page_size );
  new_alloc_size = _Heap_Block_size( block );
  test_simple_resize_block( p1, new_alloc_size, HEAP_RESIZE_SUCCESSFUL );

  puts( "\tdecrease size");

  puts( "\t\tdecrease a block with two pages to one page" );
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );
  p1 = test_alloc_two_pages();
  new_alloc_size = 1;
  test_simple_resize_block( p1, new_alloc_size, HEAP_RESIZE_SUCCESSFUL );

  puts( "\t\tresize the block to the size 0" );
  test_heap_init( TEST_DEFAULT_PAGE_SIZE );
  p1 = test_alloc_one_page();
  new_alloc_size = 0;
  test_simple_resize_block( p1, new_alloc_size, HEAP_RESIZE_SUCCESSFUL );
}

static void test_heap_assert(bool ret, bool expected)
{
  rtems_test_assert( ret == expected );
  rtems_test_assert( _Heap_Walk( &TestHeap, 0, false ) );
}

static void test_heap_extend(void)
{
  bool ret = false;
  Heap_Control *heap = &TestHeap;
  uint8_t *area_begin = TestHeapMemory;
  uint8_t *sub_area_begin;
  uint8_t *sub_area_end;

  _Heap_Initialize( heap, area_begin + 768, 256, 0 );
  sub_area_begin = (uint8_t *) heap->first_block;
  sub_area_end = (uint8_t *) heap->first_block->prev_size;

  puts( "heap extend - link below" );
  ret = _Protected_heap_Extend( heap, area_begin + 0, 256 );
  test_heap_assert( ret, true );

  puts( "heap extend - merge below overlap" );
  ret = _Protected_heap_Extend( heap, sub_area_begin - 128, 256 );
  test_heap_assert( ret, false );

  puts( "heap extend - merge below" );
  ret = _Protected_heap_Extend( heap, sub_area_begin - 256, 256 );
  test_heap_assert( ret, true );

  puts( "heap extend - merge above overlap" );
  ret = _Protected_heap_Extend( heap, sub_area_end - 128, 256 );
  test_heap_assert( ret, false );

  puts( "heap extend - merge above" );
  ret = _Protected_heap_Extend( heap, sub_area_end, 256 );
  test_heap_assert( ret, true );

  puts( "heap extend - link above" );
  ret = _Protected_heap_Extend( heap, area_begin + 1536, 256 );
  test_heap_assert( ret, true );

  puts( "heap extend - area too small" );
  ret = _Protected_heap_Extend( heap, area_begin + 2048, 0 );
  test_heap_assert( ret, false );

  puts( "heap extend - invalid area" );
  ret = _Protected_heap_Extend( heap, (void *) -1, 2 );
  test_heap_assert( ret, false );

  area_begin = (uint8_t *) (((uintptr_t) area_begin) | 1);

  _Heap_Initialize( heap, area_begin + 768, 256, 0 );

  puts( "heap extend - merge below with align up" );
  ret = _Protected_heap_Extend( heap, area_begin + 512, 256 );
  test_heap_assert( ret, true );
}

static void test_heap_extend_allocation_order(void)
{
  Heap_Control *heap = &TestHeap;
  uintptr_t size = 256;
  uintptr_t gap = 256;
  uint8_t *init_area_begin = TestHeapMemory;
  uint8_t *extend_area_begin = init_area_begin + size + gap;
  bool ret;
  uint8_t *p;

  _Heap_Initialize( heap, init_area_begin, size, 0 );

  ret = _Protected_heap_Extend( heap, extend_area_begin, size );
  test_heap_assert( ret, true );

  p = _Heap_Allocate( heap, 1 );
  rtems_test_assert( (uintptr_t) (p - init_area_begin) < size );
}

static void test_heap_extend_allocation_order_with_empty_heap(void)
{
  Heap_Control *heap = &TestHeap;
  uintptr_t size = 256;
  uintptr_t gap = 256;
  uint8_t *init_area_begin = TestHeapMemory;
  uint8_t *extend_area_begin = init_area_begin + size + gap;
  bool ret;
  uint8_t *p;

  _Heap_Initialize( heap, init_area_begin, size, 0 );

  _Heap_Greedy_allocate( heap, NULL, 0 );

  ret = _Protected_heap_Extend( heap, extend_area_begin, size );
  test_heap_assert( ret, true );

  p = _Heap_Allocate( heap, 1 );
  rtems_test_assert( (uintptr_t) (p - extend_area_begin) < size );
}

static void test_heap_no_extend(void)
{
  uintptr_t extended_space = _Heap_No_extend( NULL, 0, 0, 0 );
  rtems_test_assert( extended_space == 0 );
}

static void free_all_delayed_blocks( void )
{
  rtems_resource_snapshot unused;

  rtems_resource_snapshot_take( &unused );
}

static void do_free( void *p )
{
  free( p );
  free_all_delayed_blocks();
}

static void test_heap_info(void)
{
  size_t                  s1, s2;
  void                   *p1;
  int                     sc;
  Heap_Information_block  the_info;

  free_all_delayed_blocks();

  s1 = malloc_free_space();
  p1 = malloc( 512 );
  s2 = malloc_free_space();
  puts( "malloc_free_space - check malloc space drops after malloc" );
  rtems_test_assert( s1 );
  rtems_test_assert( s2 );
  rtems_test_assert( s2 <= s1 );
  do_free( p1 );

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
  do_free( p1 );

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

static void test_rtems_heap_allocate_aligned_with_boundary(void)
{
  void *p = NULL;

  p = rtems_heap_allocate_aligned_with_boundary(1, 1, 1);
  rtems_test_assert( p != NULL );
  free(p);

  _Thread_Dispatch_disable();
  p = rtems_heap_allocate_aligned_with_boundary(1, 1, 1);
  _Thread_Dispatch_enable( _Per_CPU_Get() );
  rtems_test_assert( p == NULL );
}

static void test_rtems_malloc(void)
{
  void *p;

  p = rtems_malloc(0);
  rtems_test_assert(p == NULL);

  errno = 0;
  p = rtems_malloc(SIZE_MAX / 2);
  rtems_test_assert(p == NULL);
  rtems_test_assert(errno == 0);

  p = rtems_malloc(1);
  rtems_test_assert(p != NULL);

  RTEMS_OBFUSCATE_VARIABLE(p);
  free(p);
}

static void test_rtems_calloc(void)
{
  void *p;
  int *i;

  p = rtems_calloc(0, 0);
  rtems_test_assert(p == NULL);

  p = rtems_calloc(0, 1);
  rtems_test_assert(p == NULL);

  p = rtems_calloc(1, 0);
  rtems_test_assert(p == NULL);

  errno = 0;
  p = rtems_calloc(1, SIZE_MAX / 2);
  rtems_test_assert(p == NULL);
  rtems_test_assert(errno == 0);

  errno = 0;
  p = rtems_calloc(SIZE_MAX / 2, 1);
  rtems_test_assert(p == NULL);
  rtems_test_assert(errno == 0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-size-larger-than=N"
  errno = 0;
  p = rtems_calloc(SIZE_MAX, SIZE_MAX);
  rtems_test_assert(p == NULL);
  rtems_test_assert(errno == 0);
#pragma GCC diagnostic pop

  i = rtems_calloc(1, sizeof(*i));
  rtems_test_assert(i != NULL);
  rtems_test_assert(*i == 0);


  RTEMS_OBFUSCATE_VARIABLE(p);
  free(i);
}

static void test_heap_size_with_overhead(void)
{
  uintptr_t s;

  puts( "_Heap_Size_with_overhead" );

  s = _Heap_Size_with_overhead(0, 0, 0);
  rtems_test_assert(s == HEAP_BLOCK_HEADER_SIZE + CPU_ALIGNMENT - 1);

  s = _Heap_Size_with_overhead(CPU_ALIGNMENT, 0, 0);
  rtems_test_assert(s == HEAP_BLOCK_HEADER_SIZE + CPU_ALIGNMENT - 1);

  s = _Heap_Size_with_overhead(CPU_ALIGNMENT, 0, 2 * CPU_ALIGNMENT);
  rtems_test_assert(s == HEAP_BLOCK_HEADER_SIZE + 2 * CPU_ALIGNMENT - 1);

  s = _Heap_Size_with_overhead(CPU_ALIGNMENT, 123, 0);
  rtems_test_assert(s == HEAP_BLOCK_HEADER_SIZE + CPU_ALIGNMENT - 1 + 123);
}

/*
 *  A simple test of posix_memalign
 */
static void test_posix_memalign(void)
{
  void *p1;
  void **p2;
  size_t i;
  int sc;
  int maximumShift;

  /*
   * posix_memalign() is declared as never having a NULL first parameter.
   * We need to hide the NULL value from the compiler.
   */
  puts( "posix_memalign - NULL return pointer -- EINVAL" );
  p2 = NULL;
  RTEMS_OBFUSCATE_VARIABLE( p2 );
  sc = posix_memalign( p2, 32, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign NULL pointer" );

  puts( "posix_memalign - alignment of 0 -- EINVAL" );
  sc = posix_memalign( &p1, 0, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign alignment of 0" );

  puts( "posix_memalign - alignment  of 2-- EINVAL" );
  sc = posix_memalign( &p1, 2, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign alignment of 2" );

  maximumShift = (sizeof(size_t) * CHAR_BIT) - 1;
  for ( i=sizeof(void *) ; i<maximumShift ; i++ ) {
    size_t alignment = 1;

    alignment <<= i;

    p1 = NULL; /* Initialize p1 to aovid used uninitialized */

    printf( "posix_memalign - alignment of %zu -- OK\n", alignment);
    sc = posix_memalign( &p1, alignment, 8 );
    if ( sc == ENOMEM ) {
      printf( "posix_memalign - ran out of memory trying %zu\n", alignment );
      break;
    }
    posix_service_failed( sc, "posix_memalign alignment OK" );

    free( p1 );
  }
  for ( ; i<maximumShift ; i++ ) {
    size_t alignment = 1 << i;
    printf( "posix_memalign - alignment of %zd -- SKIPPED\n", alignment);
  }

}

static void test_greedy_allocate(void)
{
  Heap_Control *heap = &TestHeap;
  uintptr_t block_size = 1;
  void *p;

  _Heap_Initialize( heap, &TestHeapMemory[0], sizeof(TestHeapMemory), 0 );

  _Heap_Greedy_allocate( heap, &block_size, 1 );

  p = _Heap_Allocate( heap, 1 );
  rtems_test_assert( p != NULL );

  p = _Heap_Allocate( heap, 1 );
  rtems_test_assert( p == NULL );

  /* The internal allocation fails */
  _Heap_Greedy_allocate( heap, &block_size, 1 );

  p = _Heap_Allocate( heap, 1 );
  rtems_test_assert( p == NULL );
}

static void test_alloc_zero_size(void)
{
  size_t size;
  void *p;
  int eno;

  size = 0;
  errno = -1;

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = malloc( size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = calloc( size, 1 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = rtems_malloc( size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = rtems_calloc( 1, size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = (void *)(uintptr_t) 1;
  eno = posix_memalign( &p, 32, size );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = (void *)(uintptr_t) 1;
  eno = rtems_memalign( &p, 32, size );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = aligned_alloc( 32, size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = realloc( NULL, size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = reallocarray( NULL, 1, size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );
}

static void test_malloc_usable_size(void)
{
  void *p1, *p2;
  size_t sz1, sz2;

  p1 = malloc( 120 );
  rtems_test_assert( p1 != NULL );
  rtems_test_assert( errno == -1 );

  sz1 = malloc_usable_size( p1 );
  rtems_test_assert( sz1 != 0 );
  rtems_test_assert( sz1 >= 120 );

  p2 = malloc( 500 );
  rtems_test_assert( p2 != NULL );
  rtems_test_assert( errno == -1 );

  sz2 = malloc_usable_size( p2 );
  rtems_test_assert( sz2 != 0 );
  rtems_test_assert( sz2 >= 500 );

  p1 = realloc( p1, 500 );
  rtems_test_assert( p1 != NULL );
  rtems_test_assert( errno == -1 );

  sz1 = malloc_usable_size( p1 );
  rtems_test_assert( sz1 != 0 );
  rtems_test_assert( sz1 >= 500 );

  free( p1 );
  free( p2 );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  void             *p1;
  rtems_time_of_day time;
  rtems_status_code status;

  TEST_BEGIN();

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  /*
   * Verify case where block is too large to calloc.
   */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-size-larger-than=N"
  p1 = calloc( 1, SIZE_MAX );
  rtems_test_assert( p1 == NULL );

  p1 = calloc( SIZE_MAX, SIZE_MAX );
  rtems_test_assert( p1 == NULL );
#pragma GCC diagnostic pop

  test_heap_initialize();
  test_heap_block_allocate();
  test_heap_allocate();
  test_heap_free();
  test_heap_resize_block();
  test_realloc();
  test_heap_cases_1();
  test_heap_extend();
  test_heap_extend_allocation_order();
  test_heap_extend_allocation_order_with_empty_heap();
  test_heap_no_extend();
  test_heap_info();
  test_heap_size_with_overhead();
  test_protected_heap_info();
  test_rtems_heap_allocate_aligned_with_boundary();
  test_rtems_malloc();
  test_rtems_calloc();
  test_greedy_allocate();
  test_alloc_zero_size();
  test_malloc_usable_size();

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

  rtems_task_exit();
}

static void test_early_malloc( void )
{
  void *p;
  char *q;
  void *r;
  void *s;
  void *t;
  void *u;

  p = malloc( 1 );
  rtems_test_assert( p != NULL );

  free( p );

  q = calloc( 1, 1 );
  rtems_test_assert( q != NULL );
  rtems_test_assert( p != q );
  rtems_test_assert( q[0] == 0 );

  free( q );

  r = realloc( NULL, 128 );
  rtems_test_assert( r != q );

  s = malloc( 1 );
  rtems_test_assert( s != NULL );

  free( s );

  t = realloc( r, 256 );
  rtems_test_assert( t != NULL );
  rtems_test_assert( t != r );

  u = realloc( t, 384 );
  rtems_test_assert( t == u );

  free( u );
}

RTEMS_SYSINIT_ITEM(
  test_early_malloc,
  RTEMS_SYSINIT_INITIAL_EXTENSIONS,
  RTEMS_SYSINIT_ORDER_FIRST
);
