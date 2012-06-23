/*
 * Copyright (c) 2012 Wenjie Zhang.
 *
 *  This test case is to test atomic load operation.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <tmacros.h>
#include <rtems/atomic.h>

#define TEST_REPEAT 200000

#define ATOMIC_LOAD_NO_BARRIER(TYPE)                  \
{                                                     \
  Atomic_##TYPE t = (Atomic_##TYPE)-1, a = 0;         \
  unsigned int i;                                     \
  printf("_Atomic_Load_" #TYPE ": ");                 \
                                                      \
  a = _Atomic_Load_##TYPE(&t);                        \
                                                      \
  rtems_test_assert(a == t);                          \
                                                      \
  for (i = 0; i < TEST_REPEAT; i++){                  \
    t = (Atomic_##TYPE)random();                      \
    a = _Atomic_Load_##TYPE(&t);                      \
    rtems_test_assert(a == t);                        \			
  }                                                   \ 
                                                      \
  printf(" SUCCESS\n");                                 \
}

rtems_task Init(
    rtems_task_argument argument
    )
{
  rtems_rbtree_control  rbtree1;
  rtems_rbtree_node    *p;
  test_node            node1, node2;
  test_node            node_array[100];
  int                  id;
  int i;

  puts( "\n\n*** TEST OF RTEMS ATOMIC LOAD API WITHOUT ANY MEMORY BARRIER***" );

  ATOMIC_LOAD_NO_BARRIER(int);

  ATOMIC_LOAD_NO_BARRIER(long);

  ATOMIC_LOAD_NO_BARRIER(ptr);

  ATOMIC_LOAD_NO_BARRIER(32);

  ATOMIC_LOAD_NO_BARRIER(64);

  puts( "*** END OF RTEMS ATOMIC LOAD API WITHOUT ANY MEMORY BARRIER TEST ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */

