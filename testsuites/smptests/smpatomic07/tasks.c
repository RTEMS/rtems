/*
 * Copyright (c) 2012 Wenjie Zhang.
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

#define TEST_REPEAT 200000

#define ATOMIC_CAS_NO_BARRIER(TYPE, cpuid, mem_bar)     \
{                                                       \
  Atomic_##TYPE a = 0, b = 0;                           \
  unsigned int i;                                       \
  int r;                                                \
  for (i = 0; i < TEST_REPEAT; i++){                    \
    a = rand() % (Atomic_##TYPE)-1;                     \
    b = a;                                              \
    r = _Atomic_Compare_exchange_##TYPE(&b, a + 1, a - 1, mem_bar);                 \
    if(r != 0){                                                                     \
      locked_printf("\nCPU%d _Atomic_Compare_exchange_" #TYPE ": FAILED\n", cpuid); \
      rtems_test_exit( 0 );                                                         \
    }                                                                               \
    b = a;                                                                          \
    r = _Atomic_Compare_exchange_##TYPE(&b, a, a - 1, mem_bar);                     \
    if((r == 0) ||((r != 0) && ((a - 1) != b))){                                    \
      locked_printf("\nCPU%d _Atomic_Compare_exchange_" #TYPE ": FAILED\n", cpuid); \
      rtems_test_exit( 0 );                                                         \
    }                                                                               \
    b = a;                                                                          \
    r = _Atomic_Compare_exchange_##TYPE(&b, a + 1, a, mem_bar);                     \
    if(r != 0){                                                                     \
      locked_printf("\nCPU%d _Atomic_Compare_exchange_" #TYPE ": FAILED\n", cpuid); \
      rtems_test_exit( 0 );                                                         \
    }                                                                               \
  }                                                                                 \
  locked_printf("\nCPU%d _Atomic_Compare_exchange_" #TYPE ": SUCCESS\n", cpuid);    \
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
  ATOMIC_CAS_NO_BARRIER(int, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_CAS_NO_BARRIER(long, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_CAS_NO_BARRIER(ptr, cpu_num, ATOMIC_RELAXED_BARRIER);

  ATOMIC_CAS_NO_BARRIER(32, cpu_num, ATOMIC_RELAXED_BARRIER);

  /* test acquire barrier */
  ATOMIC_CAS_NO_BARRIER(int, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_CAS_NO_BARRIER(long, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_CAS_NO_BARRIER(ptr, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  ATOMIC_CAS_NO_BARRIER(32, cpu_num, ATOMIC_ACQUIRE_BARRIER);

  /* test release barrier */
  ATOMIC_CAS_NO_BARRIER(int, cpu_num, ATOMIC_RELEASE_BARRIER);

  ATOMIC_CAS_NO_BARRIER(long, cpu_num, ATOMIC_RELEASE_BARRIER);

  ATOMIC_CAS_NO_BARRIER(ptr, cpu_num, ATOMIC_RELEASE_BARRIER);

  ATOMIC_CAS_NO_BARRIER(32, cpu_num, ATOMIC_RELEASE_BARRIER);

//  ATOMIC_CAS_NO_BARRIER(64, cpu_num);

  /* Set the flag that the task is up and running */
  TaskRan[cpu_num] = true;

  /* Drop into a loop which will keep this task on
   * running on the cpu.
   */
  while(1);
}
