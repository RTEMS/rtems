/*
 * Copyright (c) 2012 Deng Hengyi.
 *
 *  This test case is to test atomic sub operation.
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

#define ATOMIC_FETCH_SUB_NO_BARRIER(TYPE, cpuid, mem_bar)\
{                                                        \
  Atomic_##TYPE t = 0, a = 0, b = 0;                     \
  unsigned int i;                                        \
  for (i = 0; i < TEST_REPEAT; i++){                     \
    a = (Atomic_##TYPE)(rand() % ((Atomic_##TYPE)-1 / 2));  \
    b = (Atomic_##TYPE)(rand() % ((Atomic_##TYPE)-1 / 2));  \
    t = a;                                               \
    _Atomic_Fetch_sub_##TYPE(&t, b, mem_bar);            \
    rtems_test_assert(t == (Atomic_##TYPE)(a - b));      \
  }                                                      \
  locked_printf("\nCPU%d _Atomic_Fetch_sub_" #TYPE ": SUCCESS\n", cpuid); \
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
  /* test relaxed barrier */
  ATOMIC_FETCH_SUB_NO_BARRIER(int, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(long, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(ptr, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(32, cpu_num, ATOMIC_RELAXED_BARRIER);

  /* test acquire barrier */
  ATOMIC_FETCH_SUB_NO_BARRIER(int, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(long, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(ptr, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(32, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  /* test release barrier */
  ATOMIC_FETCH_SUB_NO_BARRIER(int, cpu_num, ATOMIC_RELEASE_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(long, cpu_num, ATOMIC_RELEASE_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(ptr, cpu_num, ATOMIC_RELEASE_BARRIER);

  ATOMIC_FETCH_SUB_NO_BARRIER(32, cpu_num, ATOMIC_RELEASE_BARRIER);

//  ATOMIC_FETCH_SUB_NO_BARRIER(64, cpu_num);

  /* Set the flag that the task is up and running */
  TaskRan[cpu_num] = true;

  /* Drop into a loop which will keep this task on
   * running on the cpu.
   */
  while(1);
}
