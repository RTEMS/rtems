/*
 * Copyright (c) 2012 Deng Hengyi.
 *
 *  This test case is to test atomic compare and exchange operation.
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

#define ATOMIC_CAS_NO_BARRIER(NAME, TYPE, R_TYPE, cpuid, mem_bar)     \
{                                                       \
  Atomic_##TYPE t;                                      \
  R_TYPE a;                                             \
  R_TYPE b;                                             \
  unsigned int i;                                       \
  int r;                                                \
  for (i = 0; i < TEST_REPEAT; i++){                    \
    a = rand() % (R_TYPE)-1;                            \
    _Atomic_Store_##NAME(&t, a, mem_bar);               \
    b = a + 1;                                                           \
    r = _Atomic_Compare_exchange_##NAME(&t, &b, a - 1, mem_bar, memory_order_relaxed);\
    if(r != 0){                                                          \
      locked_printf("\ntask%d: Atomic_Compare_exchange_" #NAME ": FAILED\n", (unsigned int)cpuid); \
      rtems_test_exit( 0 );                                              \
    }                                                                    \
    _Atomic_Store_##NAME(&t, a, mem_bar);                                \
    r = _Atomic_Compare_exchange_##NAME(&t, &a, a - 1, mem_bar, memory_order_relaxed);\
    b = _Atomic_Load_##NAME(&t, mem_bar);                                \
    if((r == 0) ||((r != 0) && ((a - 1) != b))){                         \
      locked_printf("\ntask%d: Atomic_Compare_exchange_" #NAME ": FAILED\n", (unsigned int)cpuid); \
      rtems_test_exit( 0 );                                              \
    }                                                                    \
    _Atomic_Store_##NAME(&t, a, mem_bar);                                \
    b = a + 1;                                                           \
    r = _Atomic_Compare_exchange_##NAME(&t, &b, a, mem_bar, memory_order_relaxed); \
    if(r != 0){                                                          \
      locked_printf("\ntask%d: Atomic_Compare_exchange_" #NAME ": FAILED\n", (unsigned int)cpuid); \
      rtems_test_exit( 0 );                                              \
    }                                                                    \
  }                                                                                 \
  locked_printf("\nCPU%d Atomic_Compare_exchange_" #NAME ": SUCCESS\n", cpuid);    \
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
  ATOMIC_CAS_NO_BARRIER(uint, Uint, uint_fast32_t, cpu_num, ATOMIC_ORDER_RELAXED);

  ATOMIC_CAS_NO_BARRIER(ptr, Pointer, uintptr_t, cpu_num, ATOMIC_ORDER_RELAXED);

  /* test acquire barrier */
  ATOMIC_CAS_NO_BARRIER(uint, Uint, uint_fast32_t, cpu_num, ATOMIC_ORDER_ACQUIRE);

  ATOMIC_CAS_NO_BARRIER(ptr, Pointer, uintptr_t, cpu_num, ATOMIC_ORDER_ACQUIRE);

  /* test release barrier */
  ATOMIC_CAS_NO_BARRIER(uint, Uint, uint_fast32_t, cpu_num, ATOMIC_ORDER_RELEASE);

  ATOMIC_CAS_NO_BARRIER(ptr, Pointer, uintptr_t, cpu_num, ATOMIC_ORDER_RELEASE);

//  ATOMIC_CAS_NO_BARRIER(64, cpu_num);

  /* Set the flag that the task is up and running */
  TaskRan[cpu_num] = true;

  /* Drop into a loop which will keep this task on
   * running on the cpu.
   */
  while(1);
}
