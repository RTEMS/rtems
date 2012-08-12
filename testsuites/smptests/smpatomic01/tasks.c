/*
 * Copyright (c) 2012 Deng Hengyi.
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

#define ATOMIC_LOAD_NO_BARRIER(NAME, TYPE, cpuid, mem_bar)     \
{                                                        \
  Atomic_##TYPE t = (Atomic_##TYPE)-1, a = 0;            \
  unsigned int i;                                        \
  a = _Atomic_Load_##NAME(&t, mem_bar);                  \
  rtems_test_assert(a == t);                             \
  for (i = 0; i < TEST_REPEAT; i++){                     \
    t = (Atomic_##TYPE)rand();                           \
    a = _Atomic_Load_##NAME(&t, mem_bar);                \
    rtems_test_assert(a == t);                           \
  }                                                      \
  locked_printf("\nCPU%d _Atomic_Load_" #NAME ": SUCCESS\n", cpuid); \
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
  ATOMIC_LOAD_NO_BARRIER(int, Int, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_LOAD_NO_BARRIER(long, Long, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_LOAD_NO_BARRIER(ptr, Pointer, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_LOAD_NO_BARRIER(32, Int32, cpu_num, ATOMIC_RELAXED_BARRIER);

  /* test acquire barrier */
  ATOMIC_LOAD_NO_BARRIER(int, Int, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_LOAD_NO_BARRIER(long, Long, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_LOAD_NO_BARRIER(ptr, Pointer, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_LOAD_NO_BARRIER(32, Int32, cpu_num, ATOMIC_ACQUIRE_BARRIER);

//  ATOMIC_LOAD_NO_BARRIER(64, cpu_num);

  /* Set the flag that the task is up and running */
  TaskRan[cpu_num] = true;

  /* Drop into a loop which will keep this task on
   * running on the cpu.
   */
  while(1);
}
