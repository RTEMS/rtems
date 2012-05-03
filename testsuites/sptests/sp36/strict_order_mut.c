/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define CONFIGURE_INIT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <stdio.h>
#include "tmacros.h"
#include <rtems/score/coremutex.h>

#define BACK_TYPE(_type_in_ptr,_type_out,_type_in_name)    \
  ((_type_out *)((unsigned int)_type_in_ptr - \
                (unsigned int)(&((_type_out *)0)->_type_in_name)))


/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS (RTEMS_MINIMUM_STACK_SIZE * 3)

#define CONFIGURE_MAXIMUM_TASKS 10
#define CONFIGURE_MAXIMUM_SEMAPHORES 10

rtems_task Task0(rtems_task_argument ignored);
rtems_task Task1(rtems_task_argument ignored);
rtems_task Init(rtems_task_argument ignored);
rtems_task_priority Get_current_pri(void);

#include <rtems/confdefs.h>

rtems_id   Task_id[4];
rtems_name Task_name[4];

rtems_id   Mutex_id[4];
rtems_name Mutex_name[4];

rtems_task Init(rtems_task_argument ignored)
{
  rtems_status_code status;
  printf("\n----------------TEST 36 ------------\n");

  Mutex_name[0] = rtems_build_name( 'S','0',' ',' ');
  status = rtems_semaphore_create(
    Mutex_name[0],
    1,
    RTEMS_LOCAL|
    RTEMS_SIMPLE_BINARY_SEMAPHORE|
    RTEMS_PRIORITY,
    0,
    &Mutex_id[0]
  );
  directive_failed( status, "rtems_semaphore_create of S0");
  printf("Create S0, Inherit_priority\n");

  Mutex_name[1] = rtems_build_name( 'S','1',' ',' ');
  status = rtems_semaphore_create(
    Mutex_name[1],
    1,
    RTEMS_LOCAL|
    RTEMS_SIMPLE_BINARY_SEMAPHORE|
    RTEMS_PRIORITY,
    1,
    &Mutex_id[1]
  );
  directive_failed( status, "rtems_semaphore_create of S1");
  printf("Create S1, Priority_celling  is 1\n");

  Mutex_name[2] = rtems_build_name( 'S','Y','N','C');


  Task_name[0] = rtems_build_name( 'T','0',' ',' ');
  status = rtems_task_create(
    Task_name[0],
    4,
    RTEMS_MINIMUM_STACK_SIZE *2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[0]
  );
  directive_failed( status,"rtems_task_create of T0");
  printf("Create T0,priority is 4\n");

  status = rtems_task_start( Task_id[0],Task0, 0);
  directive_failed( status,"rtems_task_start of T0");

  status = rtems_task_delete( RTEMS_SELF);
  directive_failed( status,"rtems_task_delete of INIT");
}


rtems_task Task0(rtems_task_argument ignored)
{
  rtems_status_code status;

  status = rtems_semaphore_obtain( Mutex_id[0], RTEMS_WAIT, 0 );
  printf("T0 rtems_semaphore_obtain - S0\n");
  directive_failed( status,"rtems_semaphore_obtain of S0\n");
  printf("The current priority of T0 is %d\n",Get_current_pri());

  status = rtems_semaphore_obtain( Mutex_id[1], RTEMS_WAIT, 0 );
  printf("T0 rtems_semaphore_obtain - S1\n");
  directive_failed( status,"rtems_semaphore_obtain of S1");
  printf("The current priority of T0 is %d\n",Get_current_pri());

#ifdef __RTEMS_STRICT_ORDER_MUTEX__
  status = rtems_semaphore_release( Mutex_id[0] );
  printf("T0 - rtems_semaphore_release - S0\n");
  if(status == CORE_MUTEX_RELEASE_NOT_ORDER)
    printf("T0 releasing S0 not in order\n");
#endif

  status = rtems_semaphore_release(Mutex_id[1]);
  printf("T0 - rtems_semaphore_release - S1\n");
  directive_failed( status,"rtems_semaphore_release of S1\n");
  printf("The current priority of T0 is %d\n",Get_current_pri());

  Task_name[1] = rtems_build_name( 'T','1',' ',' ');
  status = rtems_task_create(
    Task_name[1],
    1,
    RTEMS_MINIMUM_STACK_SIZE *2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[1]
  );
  directive_failed( status , "rtems_task_create of T1\n");
  printf("Create S1,priority is 1\n");

  status = rtems_task_start( Task_id[1],Task1, 0);
  directive_failed( status, "rtems_task_start of T1\n");

  printf("The current priority of T0 is %d\n",Get_current_pri());

  status = rtems_semaphore_release(Mutex_id[0]);
  printf("T0 - rtems_semaphore_release - S0\n");
  directive_failed( status, "rtems_semaphore_release of S0\n");
}

rtems_task Task1(rtems_task_argument ignored)
{
  rtems_status_code status;
  status = rtems_semaphore_obtain( Mutex_id[0], RTEMS_WAIT, 0 );
  printf("T1 - rtems_semaphore_obtain - S0");
  directive_failed( status," rtems_semaphore_obtain S0");
}

rtems_task_priority Get_current_pri(void)
{
  rtems_status_code status;
  rtems_task_priority pri;

  status = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &pri);
  directive_failed( status, " rtems_task_set_priority ");
  return pri;
}
