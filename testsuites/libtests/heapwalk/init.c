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

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** HEAP WALK TEST ***" );

  test_walk_freshly_initialized();

  puts( "*** END OF HEAP WALK TEST ***" );
  rtems_test_exit(0);
}
