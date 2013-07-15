/*
 * Copyright (c) 2012 Deng Hengyi.
 *
 *  This test case is to test atomic and operation.
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

#define ATOMIC_FETCH_AND_NO_BARRIER(NAME, TYPE, R_TYPE, task_id, mem_bar)\
{                                                        \
  Atomic_##TYPE t;                                       \
  R_TYPE a;                                              \
  R_TYPE b;                                              \
  R_TYPE c;                                              \
  unsigned int i;                                        \
  for (i = 0; i < TEST_REPEAT; i++){                     \
    a = (R_TYPE)(rand() % ((R_TYPE)-1 / 2));             \
    b = (R_TYPE)(rand() % ((R_TYPE)-1 / 2));             \
    _Atomic_Store_##NAME(&t, a, ATOMIC_ORDER_RELAXED);   \
    _Atomic_Fetch_and_##NAME(&t, b, mem_bar);            \
    c = _Atomic_Load_##NAME(&t, ATOMIC_ORDER_RELAXED);   \
    rtems_test_assert(c == (R_TYPE)(a & b));             \
  }                                                      \
  printf("\ntask%d: Atomic_Fetch_and_" #NAME ": SUCCESS\n", (unsigned int)task_id); \
}

rtems_task Test_task(
    rtems_task_argument argument
    )
{
  char              name[5];
  char             *p;
  rtems_status_code  status;

  /* Get the task name */
  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

  /* Print that the task is up and running. */
  /* test relaxed barrier */
  ATOMIC_FETCH_AND_NO_BARRIER(uint, Uint, uint_fast32_t, argument, ATOMIC_ORDER_RELAXED);

  ATOMIC_FETCH_AND_NO_BARRIER(ptr, Pointer, uintptr_t, argument, ATOMIC_ORDER_RELAXED);

  /* test acquire barrier */
  ATOMIC_FETCH_AND_NO_BARRIER(uint, Uint, uint_fast32_t, argument, ATOMIC_ORDER_ACQUIRE);

  ATOMIC_FETCH_AND_NO_BARRIER(ptr, Pointer, uintptr_t, argument, ATOMIC_ORDER_ACQUIRE);

  /* test release barrier */
  ATOMIC_FETCH_AND_NO_BARRIER(uint, Uint, uint_fast32_t, argument, ATOMIC_ORDER_RELEASE);

  ATOMIC_FETCH_AND_NO_BARRIER(ptr, Pointer, uintptr_t, argument, ATOMIC_ORDER_RELEASE);

  /* Set the flag that the task is up and running */
  TaskRan[argument] = true;

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "delete" );
}

rtems_task Wait_task(
    rtems_task_argument argument
    )
{
  char              name[5];
  char             *p;
  bool               allDone;
  int                i;                

  /* Get the task name */
  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

  /* Wait on the all tasks to run */
  while (1) {
    allDone = true;
    for ( i=0; i<TASK_NUMS ; i++ ) {
      if (TaskRan[i] == false)
        allDone = false;
    }
    if (allDone) {
      puts( "\n\n*** END OF TEST spatomic05 ***\n" );
      rtems_test_exit( 0 );
    }
  }
}
