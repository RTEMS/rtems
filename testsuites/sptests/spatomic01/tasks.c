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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include <stdlib.h>
#include <rtems/rtems/atomic.h>

#define TEST_REPEAT 200000

#define ATOMIC_LOAD_NO_BARRIER(TYPE)                     \
{                                                        \
  Atomic_##TYPE t = (Atomic_##TYPE)-1, a = 0;            \
  unsigned int i;                                        \
  printf("_Atomic_Load_" #TYPE ": ");                    \
  a = _Atomic_Load_##TYPE(&t, ATOMIC_RELAXED_BARRIER);   \
  rtems_test_assert(a == t);                             \
  for (i = 0; i < TEST_REPEAT; i++){                     \
    t = (Atomic_##TYPE)rand();                         \
    a = _Atomic_Load_##TYPE(&t, ATOMIC_RELAXED_BARRIER); \
    rtems_test_assert(a == t);                           \
  }                                                      \
  printf(" SUCCESS\n");                                  \
}

rtems_task Test_task(
    rtems_task_argument argument
    )
{
  int               cpu_num;
  char              name[5];
  char             *p;

  /* Get the task name */
  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

   /* Get the CPU Number */
  cpu_num = bsp_smp_processor_id();

  /* Print that the task is up and running. */
  Loop();
  locked_printf(" \n\n CPU %d RUN TEST OF RTEMS ATOMIC LOAD API WITHOUT ANY MEMORY BARRIER***", cpu_num);

  ATOMIC_LOAD_NO_BARRIER(int);

  ATOMIC_LOAD_NO_BARRIER(long);

  ATOMIC_LOAD_NO_BARRIER(ptr);

  ATOMIC_LOAD_NO_BARRIER(32);

//  ATOMIC_LOAD_NO_BARRIER(64);

  locked_printf(" \n\n CPU %d END OF RTEMS ATOMIC LOAD API WITHOUT ANY MEMORY BARRIER TEST***", cpu_num);

    /* Set the flag that the task is up and running */
  TaskRan[cpu_num] = true;

  /* Drop into a loop which will keep this task on
 *    * running on the cpu.
 *       */
  while(1);
}
