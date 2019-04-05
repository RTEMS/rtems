/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 * Start 4 tasks with affinity for each of the 4 cpus.
 * Allow tasks to set their actual cpu value and delete themselves.
 * Verify the actual cpu values match the expected cpu values.
 *
 * Init task is at a lower priority 8 and the threads
 * with affinity are at priority 4, so the affinity task
 * on the core init is running on will preempt it.
 *
 * Test tasks run and delete themselves.
 * Init task never blocks.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>
#include <inttypes.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDAFFINITY 5";

#define NUM_CPUS   4
#define TASK_COUNT 5

struct task_data_t {
  rtems_id            id;
  cpu_set_t           cpuset;
  rtems_task_priority priority;
  bool                ran;
  int                 expected_cpu;
  int                 actual_cpu;
  int                 migrate_cpu;
};

static struct task_data_t task_data[TASK_COUNT] = {
  {0x0, {{0xc}}, 7, false,  3, -1,  2},
  {0x0, {{0xf}}, 8, false,  2, -1, -1},
  {0x0, {{0x3}}, 5, false,  1, -1,  0},
  {0x0, {{0x9}}, 6, false,  0, -1,  3},
  {0x0, {{0x2}}, 9, false, -1, -1,  1}
};

rtems_id           task_sem;

static void verify_tasks(void);

/*
 * Spin loop to allow tasks to delay without yeilding the
 * processor.
 */
static void test_delay(int ticks)
{
  rtems_interval start, stop;
  start = rtems_clock_get_ticks_since_boot();
  do {
    stop = rtems_clock_get_ticks_since_boot();
  } while ( (stop - start) < ticks );
}

static void task(rtems_task_argument arg)
{
  rtems_status_code   sc;

  while (true) {
    sc = rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0);
    if (sc == RTEMS_SUCCESSFUL) {
      task_data[arg].ran = true;
      task_data[arg].actual_cpu = rtems_scheduler_get_processor();
      rtems_semaphore_release(task_sem);
    }
  }
}
static void verify_tasks(void)
{
  int i;

  printf("Verify Tasks Ran\n");

  while( rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0) != RTEMS_SUCCESSFUL );

  /* Set Init task data */
  task_data[0].ran = true;
  task_data[0].actual_cpu = rtems_scheduler_get_processor();

  /* Verify all tasks */
  for (i = 0; i < NUM_CPUS; i++) {
    if (i==0)
      printf("Init(%" PRIu32 "): ran=%d expected=%d actual=%d\n",
        task_data[i].priority,
        task_data[i].ran,
        task_data[i].expected_cpu,
        task_data[i].actual_cpu
      );
    else
      printf( "TA0%d(%" PRIu32 "): ran=%d expected=%d actual=%d\n",
        i,
        task_data[i].priority,
        task_data[i].ran,
        task_data[i].expected_cpu,
        task_data[i].actual_cpu
      );

    /*  Abort test if values are not as expected */
    if ( task_data[i].expected_cpu == -1 )
      rtems_test_assert( task_data[i].ran == false );
    else {
      rtems_test_assert( task_data[i].ran == true );
      rtems_test_assert( task_data[i].expected_cpu == task_data[i].actual_cpu );
    }
  }

  rtems_semaphore_release(task_sem);
}

static void test(void)
{
  rtems_status_code   sc;
  rtems_task_argument i;
  size_t              size;
  uint32_t            cpu_count;
  rtems_task_priority priority;

  /* Get the number of processors that we are using. */
  cpu_count = rtems_scheduler_get_processor_maximum();
  if (cpu_count != 4) {
    printf("Test requires a minimum of 4 cores\n");
    return;
  }

  size = sizeof(cpu_set_t);
  task_data[0].id = rtems_task_self();
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


  /* Create and start tasks on each cpu with the appropriate affinity. */
  for (i = 1; i < TASK_COUNT; i++) {

      sc = rtems_task_create(
        rtems_build_name('T', 'A', '0', '0'+i),
        task_data[ i ].priority,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &task_data[ i ].id
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_task_set_affinity(
        task_data[ i ].id,
        size,
        &task_data[i].cpuset
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      printf(
        "Start TA%" PRIdrtems_task_argument " at priority %"
          PRIu32 " on cpu %d\n",
         i,
         task_data[i].priority,
         task_data[i].expected_cpu
      );
      sc = rtems_task_start( task_data[ i ].id, task, i );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  /* spin for 100 ticks */
  test_delay(100);

  verify_tasks();

  i = TASK_COUNT - 1;
  task_data[ i ].priority = 4;
  printf(
    "Set TA%" PRIdrtems_task_argument " priority %" PRIu32 "\n",
    i,
    task_data[i].priority
  );
  sc = rtems_task_set_priority(
    task_data[ i ].id,
    task_data[ i ].priority,
    &priority
  );
  test_delay(25);

  while( rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0) != RTEMS_SUCCESSFUL );
  for (i = 0; i < TASK_COUNT; i++) {
    task_data[ i ].expected_cpu = task_data[ i ].migrate_cpu;
    task_data[ i ].actual_cpu = -1;
    task_data[ i ].ran = false;
  }
  rtems_semaphore_release(task_sem);
  test_delay(25);
  verify_tasks();
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

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY       7
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
