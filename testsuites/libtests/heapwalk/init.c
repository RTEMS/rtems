/*
 *  Test of Heap Walker
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
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <rtems/score/heap.h>

#define TEST_HEAP_SIZE 1024
uint32_t TestHeapMemory[TEST_HEAP_SIZE];
Heap_Control TestHeap;

void test_heap_init(void)
{
  memset( TestHeapMemory, '\0', sizeof(TestHeapMemory) );
  _Heap_Initialize( &TestHeap, TestHeapMemory, sizeof(TestHeapMemory), 0 );
}

void test_heap_walk( int source, bool do_dump )
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
  test_heap_walk(1, true);
}

void test_negative_source_value(void)
{
  test_heap_init();
  /*
   * Passing a negative value for source so that
   * the control enters the if block on line 67
   */
  puts( "Passing negative value for source" );
  test_heap_walk( -1, true );
}

void test_prev_block_flag_check(void)
{
  /* Calling heapwalk without initialising the heap.
   * Covers line 80 and 85 on heapwalk.
   * Actually covers more than that.
   */
  puts( "Calling Heap Walk without initialising" );
  _Heap_Walk( &TestHeap, 0x01, true );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** HEAP WALK TEST ***" );

  test_prev_block_flag_check();
  test_walk_freshly_initialized();
  test_negative_source_value();

  puts( "*** END OF HEAP WALK TEST ***" );
  rtems_test_exit(0);
}
