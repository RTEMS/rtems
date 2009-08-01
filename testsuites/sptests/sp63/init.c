/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>

uint32_t      Memory[256];
Heap_Control  Heap;

void test_case_one(void)
{
  uint32_t           heap_size;
  void              *ptr1;
  intptr_t           old;
  intptr_t           avail;
  Heap_Resize_status hc;

  puts( "Init - _Heap_Initialize (for test one) - OK" );
  heap_size = _Heap_Initialize( &Heap, Memory, sizeof(Memory), 8 );
  printf( "Init - Heap size=%d\n", heap_size );

  puts( "Init - _Heap_Allocate_aligned - OK");
  ptr1 = _Heap_Allocate_aligned( &Heap, 64, 32 );
  assert( ptr1 );

  puts( "Init - _Heap_Resize_block - OK");
  hc = _Heap_Resize_block( &Heap, ptr1, 4, &old, &avail );
  assert( !hc );
}

void test_case_two(void)
{
  uint32_t           heap_size;
  void              *ptr1;
  intptr_t           old;
  intptr_t           avail;
  Heap_Resize_status hc;

  puts( "\nInit - _Heap_Initialize (for test two) - OK" );
  heap_size = _Heap_Initialize( &Heap, Memory, sizeof(Memory), 8 );
  printf( "Init - Heap size=%d\n", heap_size );

  puts( "Init - _Heap_Allocate_aligned - OK");
  ptr1 = _Heap_Allocate_aligned( &Heap, 64, 32 );
  assert( ptr1 );

  puts( "Init - _Heap_Resize_block - OK");
  hc = _Heap_Resize_block( &Heap, ptr1, 56, &old, &avail );
  assert( !hc );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  puts( "\n\n*** TEST 63 ***" );
  
  test_case_one();

  test_case_two();

  puts( "*** END OF TEST 63 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_MAXIMUM_REGIONS       1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
