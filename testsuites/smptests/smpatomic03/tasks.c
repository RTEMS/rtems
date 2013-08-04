/*
 * Copyright (c) 2012 Deng Hengyi.
 *
 *  This test case is to test atomic add operation.
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

#define TEST_REPEAT 1000

#define ATOMIC_FETCH_ADD_NO_BARRIER(NAME, TYPE, R_TYPE, cpuid, mem_bar)\
{                                                        \
  Atomic_##TYPE t;                                       \
  R_TYPE a;                                              \
  R_TYPE b;                                              \
  R_TYPE c;                                              \
  unsigned int i;                                        \
  for (i = 0; i < TEST_REPEAT; i++){                     \
    a = (R_TYPE)(rand() % ((R_TYPE)-1 / 2));             \
    b = (R_TYPE)(rand() % ((R_TYPE)-1 / 2));             \
    _Atomic_Store_##NAME(&t, a, ATOMIC_ORDER_RELEASE);   \
    _Atomic_Fetch_add_##NAME(&t, b, mem_bar);            \
    c = _Atomic_Load_##NAME(&t, ATOMIC_ORDER_ACQUIRE);   \
    rtems_test_assert(c == (R_TYPE)(a + b));             \
  }                                                      \
  locked_printf("\nCPU%d Atomic_Fetch_add_" #NAME ": SUCCESS\n", cpuid); \
}

rtems_task Test_task(
    rtems_task_argument argument
    )
{
  uint32_t          cpu_num;
  char              name[5];
  char             *p;

  /* Get the task name */
  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

   /* Get the CPU Number */
  cpu_num = rtems_smp_get_current_processor();

  /* Print that the task is up and running. */
  /* test relaxed barrier */
  ATOMIC_FETCH_ADD_NO_BARRIER(uint, Uint, uint_fast32_t, cpu_num, ATOMIC_ORDER_RELAXED);

  ATOMIC_FETCH_ADD_NO_BARRIER(ptr, Pointer, uintptr_t, cpu_num, ATOMIC_ORDER_RELAXED);

  /* test acquire barrier */
  ATOMIC_FETCH_ADD_NO_BARRIER(uint, Uint, uint_fast32_t, cpu_num, ATOMIC_ORDER_ACQUIRE);

  ATOMIC_FETCH_ADD_NO_BARRIER(ptr, Pointer, uintptr_t, cpu_num, ATOMIC_ORDER_ACQUIRE);

  /* test release barrier */
  ATOMIC_FETCH_ADD_NO_BARRIER(uint, Uint, uint_fast32_t, cpu_num, ATOMIC_ORDER_RELEASE);

  ATOMIC_FETCH_ADD_NO_BARRIER(ptr, Pointer, uintptr_t, cpu_num, ATOMIC_ORDER_RELEASE);

//  ATOMIC_FETCH_ADD_NO_BARRIER(64, cpu_num);

  /* Set the flag that the task is up and running */
  TaskRan[cpu_num] = true;

  /* Drop into a loop which will keep this task on
   * running on the cpu.
   */
  while(1);
}
