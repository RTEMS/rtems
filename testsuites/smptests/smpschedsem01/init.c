/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDSEM 01";

#define NUM_CPUS   1
#define TASK_COUNT 2
#define TASK_PRIORITY 8
#define SEM_PRIORITY 5

/*
 * Test verifies priority,
 * Changes priority by obtaining a higher priority semaphore
 * Releases semaphore to return priority
 */
static void test(void)
{
  rtems_status_code   sc;
  rtems_task_priority priority;
  rtems_id            task_sem;

  sc = rtems_semaphore_create(  
    rtems_build_name('S', 'E', 'M', '0'),
    1,
    RTEMS_BINARY_SEMAPHORE |
    RTEMS_PRIORITY     |
    RTEMS_PRIORITY_CEILING, 
    5,
    &task_sem
  );  
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);
  printf("Init: priority %d expected %d\n",(int)priority, TASK_PRIORITY );
  rtems_test_assert( priority == TASK_PRIORITY );

  printf("Init: Obtain Semaphore\n");
  sc = rtems_semaphore_obtain (task_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);
  printf("Init: priority %d expected %d\n",(int)priority, SEM_PRIORITY );
  rtems_test_assert( priority == SEM_PRIORITY );

  printf("Init: Release Semaphore\n");
  rtems_semaphore_release(task_sem);
  rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);
  printf("Init: priority %d expected %d\n",(int)priority, TASK_PRIORITY );
  rtems_test_assert( priority == TASK_PRIORITY );
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_MAXIMUM_PROCESSORS NUM_CPUS

#define CONFIGURE_MAXIMUM_TASKS          TASK_COUNT
#define CONFIGURE_MAXIMUM_SEMAPHORES     1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY     TASK_PRIORITY
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
