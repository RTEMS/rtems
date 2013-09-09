/*
 * Copyright (c) 2012 Deng Hengyi.
 *
 *  This test case is to test atomic store operation.
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

#define ATOMIC_STORE_NO_BARRIER(NAME, TYPE, R_TYPE, cpuid, mem_bar)    \
{                                                        \
  Atomic_##TYPE t;                                       \
  R_TYPE a;                                              \
  R_TYPE b;                                              \
  unsigned int i;                                        \
  for (i = 0; i < TEST_REPEAT; i++){                     \
    b = (R_TYPE)rand();                                  \
    _Atomic_Store_##NAME(&t, b, mem_bar);                \
    a = _Atomic_Load_##NAME(&t, ATOMIC_ORDER_ACQUIRE);   \
    rtems_test_assert(a == b);                           \
  }                                                      \
  locked_printf("\nCPU%d Atomic_Store_" #NAME ": SUCCESS\n", cpuid); \
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
  ATOMIC_STORE_NO_BARRIER(ulong, Ulong, unsigned long, cpu_num, ATOMIC_ORDER_RELAXED);

  ATOMIC_STORE_NO_BARRIER(ptr, Pointer, uintptr_t, cpu_num, ATOMIC_ORDER_RELAXED);

  /* test release barrier */
  ATOMIC_STORE_NO_BARRIER(ulong, Ulong, unsigned long, cpu_num, ATOMIC_ORDER_RELEASE);

  ATOMIC_STORE_NO_BARRIER(ptr, Pointer, uintptr_t, cpu_num, ATOMIC_ORDER_RELEASE);

//  ATOMIC_STORE_NO_BARRIER(64, cpu_num);

  /* Set the flag that the task is up and running */
  TaskRan[cpu_num] = true;

  /* Drop into a loop which will keep this task on
   * running on the cpu.
   */
  while(1);
}
