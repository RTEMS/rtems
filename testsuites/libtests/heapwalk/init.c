/*
 *  Test of Heap Walker
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2009 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#define CONFIGURE_INIT
#include "system.h"

#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <rtems/score/heap.h>
#include <rtems/dumpbuf.h>

#define TEST_HEAP_SIZE 1024
#define TEST_DEFAULT_PAGESIZE 128
#define DUMP false

unsigned TestHeapMemory[TEST_HEAP_SIZE];
Heap_Control TestHeap;

static void test_heap_init_with_page_size( uintptr_t page_size )
{
  memset( TestHeapMemory, 0xFF, sizeof(TestHeapMemory) );
  _Heap_Initialize( &TestHeap, TestHeapMemory, sizeof(TestHeapMemory), page_size );
}

static void test_heap_init_default(void)
{
  test_heap_init_with_page_size( 0 );
}

static void test_heap_init_custom(void)
{
  test_heap_init_with_page_size( TEST_DEFAULT_PAGESIZE );
}

static void test_call_heap_walk( bool expectet_retval )
{
  bool retval = _Heap_Walk( &TestHeap, 0, DUMP );
  rtems_test_assert( retval == expectet_retval );
}

static void *test_allocate_block(void)
{
  return _Heap_Allocate_aligned_with_boundary( &TestHeap, 1, 0, 0 );
}

static void test_create_heap_with_gaps(void)
{
  void *p1 = test_allocate_block();
  /* void *p2 = */ test_allocate_block();
  void *p3 = test_allocate_block();
  /* void *p4 = */ test_allocate_block();
  void *p5 = test_allocate_block();
  /* void *p6 = */ test_allocate_block();
  _Heap_Free( &TestHeap, p1 );
  _Heap_Free( &TestHeap, p3 );
  _Heap_Free( &TestHeap, p5 );
}

static void *test_fill_heap(void)
{
  void *p1 = NULL;
  void *p2 = NULL;

  do {
    p2 = p1;
    p1 = test_allocate_block();
  }while( p1 != NULL );

  return p2;
}

static void test_system_not_up(void)
{
  rtems_interrupt_level level;

  puts( "start with a system state != SYSTEM_STATE_UP" );

  rtems_interrupt_disable( level );
  System_state_Codes state = _System_state_Get();
  _System_state_Set( SYSTEM_STATE_FAILED );
  test_call_heap_walk( true );
  _System_state_Set( state );
  rtems_interrupt_enable( level );
}

static void test_check_control(void)
{
  puts( "testing the _Heap_Walk_check_control() function" );

  puts( "\ttest what happens if page size = 0" );
  test_heap_init_default();
  TestHeap.page_size = 0;
  test_call_heap_walk( false );

  puts( "\tset page size to a not CPU-aligned value" );
  test_heap_init_default();
  TestHeap.page_size = 3 * (CPU_ALIGNMENT) / 2;
  test_call_heap_walk( false );

  puts( "\tset minimal block size to a not page aligned value" );
  test_heap_init_custom();
  TestHeap.min_block_size = TEST_DEFAULT_PAGESIZE / 2;
  test_call_heap_walk( false );

  puts( "\tset allocation area of the first block to be non page-aligned" );
  test_heap_init_custom();
  TestHeap.first_block = (Heap_Block *) ((char *) TestHeap.first_block + CPU_ALIGNMENT);
  test_call_heap_walk( false );

  puts( "\tclear the previous used flag of the first block" );
  test_heap_init_default();
  TestHeap.first_block->size_and_flag &= ~HEAP_PREV_BLOCK_USED;
  test_call_heap_walk( false );

  puts( "\tmark last block as free" );
  test_heap_init_custom();
  TestHeap.last_block->size_and_flag = 0;
  test_call_heap_walk( false );

  puts( "\tset invalid next block for last block" );
  test_heap_init_custom();
  TestHeap.last_block->size_and_flag = HEAP_PREV_BLOCK_USED;
  test_call_heap_walk( false );
}

static void test_check_free_list(void)
{
  void *p1 = NULL;
  Heap_Block *first_free_block = NULL;
  Heap_Block *secound_free_block = NULL;
  Heap_Block *third_free_block = NULL;
  Heap_Block *used_block = NULL;

  puts( "testing the _Heap_Walk_check_free_list() function" );

  puts( "\tno free blocks" );
  test_heap_init_custom();
  test_fill_heap();
  test_call_heap_walk( true );

  puts( "\tcreate a loop in the free list" );
  test_heap_init_default();
  test_create_heap_with_gaps();
  /* find free blocks */
  first_free_block = _Heap_Free_list_first( &TestHeap );
  secound_free_block = first_free_block->next;
  third_free_block = secound_free_block->next;
  /* create a loop */
  third_free_block->next = secound_free_block;
  secound_free_block->prev = third_free_block;
  test_call_heap_walk( false );

  puts( "\tput a block outside the heap to the free list" );
  test_heap_init_default();
  first_free_block = _Heap_Free_list_first( &TestHeap );
  first_free_block->next = TestHeap.first_block - 1;
  test_call_heap_walk( false );

  puts( "\tput a block on the free list, which is not page-aligned" );
  test_heap_init_custom();
  test_create_heap_with_gaps();
  first_free_block = _Heap_Free_list_first( &TestHeap );
  first_free_block->next = (Heap_Block *) ((uintptr_t) first_free_block->next + CPU_ALIGNMENT);
  first_free_block->next->prev = first_free_block;
  test_call_heap_walk( false );

  puts( "\tput a used block on the free list" );
  test_heap_init_custom();
  test_create_heap_with_gaps();
  p1 = test_allocate_block();
  first_free_block = _Heap_Free_list_first( &TestHeap );
  used_block = _Heap_Block_of_alloc_area( (uintptr_t) p1, TestHeap.page_size );
  _Heap_Free_list_insert_after( first_free_block, used_block );
  test_call_heap_walk( false );
}

static void test_freshly_initialized(void)
{
  puts( "Walk freshly initialized heap" );
  test_heap_init_default();
  test_call_heap_walk( true );
}

static void test_main_loop(void)
{
  void *p1 = NULL;
  void *p2 = NULL;
  Heap_Block *block = NULL;
  Heap_Block *next_block = NULL;

  puts( "Test the main loop" );

  puts( "\tset the blocksize so, that the next block is outside the heap" );
  test_heap_init_custom();
  /* use all blocks */
  p1 = test_fill_heap();
  block = _Heap_Block_of_alloc_area( (uintptr_t) p1, TestHeap.page_size );
  block->size_and_flag = ( 2 * _Heap_Block_size( block ) ) | HEAP_PREV_BLOCK_USED;
  test_call_heap_walk( false );

  puts( "\twalk a heap with blocks with different states of the previous-used flag" );
  test_heap_init_custom();
  test_create_heap_with_gaps();
  test_allocate_block(); /* fill one gap */
  test_call_heap_walk( true );

  puts( "\tcreate a block with a not page aligned size" );
  test_heap_init_custom();
  p1 = test_allocate_block();
  p2 = test_allocate_block();
  _Heap_Free( &TestHeap, p1 );
  block = _Heap_Block_of_alloc_area( (uintptr_t) p2, TestHeap.page_size );
  block->size_and_flag = (3 * TestHeap.page_size / 2) & ~HEAP_PREV_BLOCK_USED;
  test_call_heap_walk( false );

  puts( "\tcreate a block with a size smaller than the min_block_size" );
  test_heap_init_default();
  p1 = test_allocate_block();
  p2 = test_allocate_block();
  _Heap_Free( &TestHeap, p1 );
  block = _Heap_Block_of_alloc_area( (uintptr_t) p2, TestHeap.page_size );
  block->size_and_flag = 0;
  test_call_heap_walk( false );

  puts( "\tmake a block with a size, so that the block reaches into the next block" );
  test_heap_init_default();
  p1 = test_allocate_block();
  p2 = test_allocate_block();
  block = _Heap_Block_of_alloc_area( (uintptr_t) p1, TestHeap.page_size  );
  block->size_and_flag = ( 3 * _Heap_Block_size( block ) / 2 ) | HEAP_PREV_BLOCK_USED;
  test_call_heap_walk( false );

  puts( "\tcreate a block with invalid successor" );
  test_heap_init_default();
  test_allocate_block();
  p1 = test_allocate_block();
  block = _Heap_Block_of_alloc_area( (uintptr_t) p1, TestHeap.page_size  );
  block->size_and_flag = (0 - TestHeap.page_size) | HEAP_PREV_BLOCK_USED;
  test_call_heap_walk( false );

  puts( "\tmake a block with a size, so that it includes the next block" );
  test_heap_init_default();
  p1 = test_allocate_block();
  p2 = test_allocate_block();
  block = _Heap_Block_of_alloc_area( (uintptr_t) p1, TestHeap.page_size  );
  next_block = _Heap_Block_at( block, _Heap_Block_size( block ) );
  block->size_and_flag = ( _Heap_Block_size( block ) + _Heap_Block_size( next_block ) ) | HEAP_PREV_BLOCK_USED;
  test_call_heap_walk( true );
}

static void test_check_free_block(void)
{
  Heap_Block *block = NULL;
  Heap_Block *next_block = NULL;
  Heap_Block *first_free_block = NULL;
  Heap_Block *secound_free_block = NULL;
  void *p1 = NULL;

  puts( "test the _Heap_Walk_check_free_block() function" );

  puts( "\tset a previous size for the next block which is not equal to the size of the actual block" );
  test_heap_init_default();
  block = _Heap_Free_list_first( &TestHeap );
  next_block = _Heap_Block_at( block, _Heap_Block_size( block ) );
  next_block->prev_size = _Heap_Block_size( block ) - 1;
  test_call_heap_walk( false );

  puts( "\tclear the previous_used flag of the first free block after an used block" );
  test_heap_init_default();
  p1 = test_allocate_block();
  block = _Heap_Block_of_alloc_area( (uintptr_t) p1, TestHeap.page_size );
  first_free_block = _Heap_Free_list_first( &TestHeap );
  first_free_block->size_and_flag &= ~HEAP_PREV_BLOCK_USED;
  first_free_block->prev_size = _Heap_Block_size( block );
  _Heap_Free_list_insert_after( first_free_block, block );
  test_call_heap_walk( false );

  puts( "\ttake a free block out of the free list" );
  test_heap_init_custom();
  test_create_heap_with_gaps();
  first_free_block = _Heap_Free_list_first( &TestHeap );
  secound_free_block = first_free_block->next;
  _Heap_Free_list_remove( secound_free_block );
  test_call_heap_walk( false );
}

static void test_output(void)
{
  puts( "test the output-function for the _Heap_Walk()" );
  puts( "therefore use the (already tested) case with a page size of 0" );
  /* use simple case where one PASS and one FAIL will be put out */
  test_heap_init_default();
  TestHeap.page_size = 0;
  test_call_heap_walk( false );
   _Heap_Walk( &TestHeap, 0, true );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** HEAP WALK TEST ***" );

  test_system_not_up();
  test_check_control();
  test_check_free_list();
  test_freshly_initialized();
  test_main_loop();
  test_check_free_block();
  test_output();

  puts( "*** END OF HEAP WALK TEST ***" );
  rtems_test_exit(0);
}
