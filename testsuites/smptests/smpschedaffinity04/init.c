/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 * Use the Init task to walk the higher priority TA1 across all the cores.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDAFFINITY 4";

#define NUM_CPUS   4
#define TASK_COUNT 2

struct task_data_t {
  rtems_id   id;
  int        expected_cpu;
  bool       ran;
  int        actual_cpu;
};

struct task_data_t task_data = {
  0x0, 2, false, 0xff
};

rtems_id           task_sem;

static void task(rtems_task_argument arg);

static void test_delay(int ticks)
{ 
  rtems_interval start, stop;
  start = rtems_clock_get_ticks_since_boot();
  do {
    stop = rtems_clock_get_ticks_since_boot();
  } while ( (stop - start) < ticks );
}

/*
 * Task that continually sets the cpu and 
 * run indicators without blocking.
 */
static void task(rtems_task_argument arg)
{
  rtems_status_code   sc;

  while (true) {
    sc = rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0);
    if (sc == RTEMS_SUCCESSFUL) {
      task_data.ran = true;
      task_data.actual_cpu = rtems_scheduler_get_processor();
      rtems_semaphore_release(task_sem);
    }
  }
}

static void test(void)
{
  rtems_status_code   sc;
  uint32_t            cpu_count;
  int                 cpu;
  int                 i;
  cpu_set_t           cpuset;

  /* Get the number of processors that we are using. */
  cpu_count = rtems_scheduler_get_processor_maximum();
  if (cpu_count < 2) {
    printf("Error: Test requires at least 2 cpus\n");
    return;
  }

  printf("Create Semaphore\n");
  sc = rtems_semaphore_create(  
    rtems_build_name('S', 'E', 'M', '0'),
    1,                                               /* initial count = 1 */
    RTEMS_BINARY_SEMAPHORE |
    RTEMS_PRIORITY | 
    RTEMS_PRIORITY_CEILING,
    0,
    &task_sem
  );  
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /*
   * Create and start TA1 at a higher priority
   * than the init task. 
   */
  sc = rtems_task_create(
    rtems_build_name('T', 'A', '0', '1'),
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_data.id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  printf("Start TA1\n");
  sc = rtems_task_start( task_data.id, task, 0 );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* 
   * Verify the Init task is running on the max core.
   */ 
  printf("Verify Init task is on cpu %" PRIu32 "\n",cpu_count-1);
  cpu = rtems_scheduler_get_processor();
  rtems_test_assert(cpu == (cpu_count-1));

  /* Walk TA1 across all of the cores */
  for(i=0; i < cpu_count; i++) {
    /* Set the Affinity to core i */
    CPU_ZERO(&cpuset);
    CPU_SET(i, &cpuset);
    printf("Set Affinity TA1 to cpu %d\n", i);
    sc = rtems_task_set_affinity( task_data.id, sizeof(cpuset), &cpuset );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    /* Wait a bit to be sure it has switched cores then clear the task data */
    test_delay(50);
    while( rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0) != RTEMS_SUCCESSFUL );
    task_data.ran = false;
    task_data.expected_cpu = i;
    rtems_semaphore_release(task_sem);
    test_delay(50);

    /* Verify the task ran on core i */
    while( rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0) != RTEMS_SUCCESSFUL );
    if (task_data.ran != true)
      printf("Error: TA01 never ran.\n");
    else
      printf(
        "TA1 expected cpu: %d actual cpu %d\n", 
        task_data.expected_cpu,
        task_data.actual_cpu
      );
    rtems_test_assert(task_data.ran == true);
    rtems_test_assert(task_data.expected_cpu == task_data.actual_cpu);
    rtems_semaphore_release(task_sem);
  }
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

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_PROCESSORS NUM_CPUS

#define CONFIGURE_MAXIMUM_TASKS          TASK_COUNT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY      8
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
