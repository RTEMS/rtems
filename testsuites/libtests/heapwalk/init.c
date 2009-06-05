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
#include <rtems/score/heap.h>

#define TEST_HEAP_SIZE 1024
uint8_t TestHeapMemory[TEST_HEAP_SIZE];
Heap_Control TestHeap;

void test_heap_init(void)
{
  _Heap_Initialize( &TestHeap, TestHeapMemory, TEST_HEAP_SIZE, 0 );
}

void test_walk_freshly_initialized(void)
{
  puts( "Walk freshly initialized heap" );
  test_heap_init();
  _Heap_Walk( &TestHeap, 0x01, true );
}

void test_negative_source_value(void)
{
  test_heap_init();
/* Passing a negative value for source so that
 * the control enters the if block on line 67
 */
  puts( "Passing negative value for source" );
  _Heap_Walk( &TestHeap, -1, true );

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
