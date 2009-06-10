/*
 *  Test of Heap Walker
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
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <rtems/score/heap.h>
#include <rtems/dumpbuf.h>

#define TEST_HEAP_SIZE 1024
uint32_t TestHeapMemory[TEST_HEAP_SIZE];
Heap_Control TestHeap;

void test_heap_init(void)
{
  memset( TestHeapMemory, '\0', sizeof(TestHeapMemory) );
  _Heap_Initialize( &TestHeap, TestHeapMemory, sizeof(TestHeapMemory), 0 );
}

void test_heap_walk_body( int source, bool do_dump );

void test_heap_walk( int source )
{
  test_heap_walk_body( source, true );
  test_heap_walk_body( source, false );
}

void test_heap_walk_body( int source, bool do_dump )
{
  int i, j, original;

  _Heap_Walk( &TestHeap, source, do_dump );

  /*
   *  Now corrupt all non-zero values
   */
  for (i=0 ; i<TEST_HEAP_SIZE ; i++) {
    original = TestHeapMemory[i];
    if ( original == 0 )
      continue;

    /* mark it free -- may or may not have already been */
    TestHeapMemory[i] &= ~0x01;
    _Heap_Walk( &TestHeap, source, do_dump );

    /* mark it used -- may or may not have already been */
    TestHeapMemory[i] |= 0x01;
    _Heap_Walk( &TestHeap, source, do_dump );

    /* try values of 2-7 in the last three bits -- push alignment issues */
    for (j=2 ; j<=7 ; j++) {
      TestHeapMemory[i] = (TestHeapMemory[i] & ~0x7) | j;
      _Heap_Walk( &TestHeap, source, do_dump );
    }


    /* try 0 and see what that does */
    TestHeapMemory[i] = 0x00;
    _Heap_Walk( &TestHeap, source, do_dump );

    /* try 0xffffffff and see what that does */
    TestHeapMemory[i] = 0xffffffff;
    _Heap_Walk( &TestHeap, source, do_dump );

    TestHeapMemory[i] = original;
  }
}

void test_walk_freshly_initialized(void)
{
  puts( "Walk freshly initialized heap" );
  test_heap_init();
  test_heap_walk(1);
}

void test_negative_source_value(void)
{
  /*
   * Passing a negative value for source so that
   * the control enters the if block on line 67
   */
  puts( "Passing negative value for source" );
  test_heap_init();
  test_heap_walk( -1 );
}

void test_prev_block_flag_check(void)
{
  /* Calling heapwalk without initialising the heap.
   * Covers line 80 and 85 on heapwalk.
   * Actually covers more than that.
   */
  puts( "Calling Heap Walk without initialising" );
  test_heap_walk( 1 );
}

void test_not_aligned(void)
{
  /*
   * Hack to get to the error case where the blocks are
   * not on the page size.  We initialize a heap with
   * default settings and change the page size to something
   * large.
   */
  puts( "Testing case of blocks not on page size" );
  test_heap_init();
  TestHeap.page_size = 128;
  test_heap_walk( -1 );
}

void test_first_block_not_aligned(void)
{
  /*
   * Hack to get to the error case where the blocks are
   * not on the page size.  We initialize a heap with
   * default settings and change the page size to something
   * large.
   */
  puts( "Testing case of blocks not on page size" );
  test_heap_init();
  _Heap_Head(&TestHeap)->next = (void *)1;
  test_heap_walk( -1 );
}

void test_not_in_free_list(void)
{
  void *p1, *p2, *p3;

  /*
   * Hack to get to the error case where the blocks are
   * not on the page size.  We initialize a heap with
   * default settings and change the page size to something
   * large.
   */
  puts( "Testing case of blocks not on page size" );
  test_heap_init();
  p1 =_Heap_Allocate( &TestHeap, 32 );
  p2 =_Heap_Allocate( &TestHeap, 32 );
  p3 =_Heap_Allocate( &TestHeap, 32 );
  _Heap_Free( &TestHeap, p2 );
  test_heap_walk( -1 );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** HEAP WALK TEST ***" );

  test_prev_block_flag_check();
  test_walk_freshly_initialized();
  test_negative_source_value();
  test_not_aligned();
  test_not_in_free_list();
  test_first_block_not_aligned();

  puts( "*** END OF HEAP WALK TEST ***" );
  rtems_test_exit(0);
}
