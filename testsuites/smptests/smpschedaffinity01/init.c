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

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDAFFINITY 1";

#define NUM_CPUS   4
#define TASK_COUNT 5

struct task_data_t {
  rtems_id   id;
  int        expected_cpu;
  cpu_set_t  cpuset;
  bool       ran;
  int        actual_cpu;
};

static struct task_data_t task_data[NUM_CPUS] = {
  {0x0, 2, {{0x4}}, false, -1},
  {0x0, 0, {{0x1}}, false, -1},
  {0x0, 3, {{0x8}}, false, -1},
  {0x0, 1, {{0x2}}, false, -1}
};
  
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
  uint32_t cpu;
  cpu_set_t cpuset;

  cpu = rtems_scheduler_get_processor();

  rtems_task_get_affinity( rtems_task_self(), sizeof(cpuset), &cpuset );

  task_data[arg].ran = true;
  task_data[arg].actual_cpu = cpu;

  rtems_task_exit();
}

static void test(void)
{
  rtems_status_code   sc;
  rtems_task_argument i;
  size_t              size;
  uint32_t            cpu_count;

  /* Get the number of processors that we are using. */
  cpu_count = rtems_scheduler_get_processor_maximum();
 
  size = sizeof(cpu_set_t);

  /* Create and start tasks on each cpu with the appropriate affinity. */
  for (i = 0; i < NUM_CPUS; i++) {

      /* Skip if this cpu doesn't exist, don't create task */
      if ( task_data[i].expected_cpu >= cpu_count ) {
        printf(
          "Skipping TA0%" PRIdrtems_task_argument
            " because on a core we do not have\n",
          i
        );
        continue;
      }

      sc = rtems_task_create(
        rtems_build_name('T', 'A', '0', '0'+i),
        4,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &task_data[ i ].id
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      printf(
        "Set TA%" PRIdrtems_task_argument " affinity to cpu %d\n",
        i,
        task_data[i].expected_cpu
      );
      sc = rtems_task_set_affinity( task_data[ i ].id, size, &task_data[i].cpuset );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      printf(
        "Start TA%" PRIdrtems_task_argument " on cpu %d\n",
        i,
        task_data[i].expected_cpu
      );
      sc = rtems_task_start( task_data[ i ].id, task, i );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  /* spin for 100 ticks */
  test_delay(100);

  printf("Verify Tasks Ran\n");
  for (i = 0; i < NUM_CPUS; i++) {

    /* Skip if this cpu doesn't exist, task doesn't exist */
    if ( task_data[i].expected_cpu >= cpu_count ) {
      printf(
        "Skipping TA0%" PRIdrtems_task_argument
          " because on a core we do not have\n",
        i
      );
      continue;
    }

    /* print the expected and actual values */
    printf(
      "TA0%" PRIdrtems_task_argument ": ran=%d expected=%d actual=%d\n",
      i,
      task_data[i].ran,
      task_data[i].expected_cpu,
      task_data[i].actual_cpu
    );

    /*  Abort test if values are not as expected */
    rtems_test_assert( task_data[i].ran == true );
    rtems_test_assert( task_data[i].expected_cpu == task_data[i].actual_cpu );
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

#define CONFIGURE_MAXIMUM_PROCESSORS NUM_CPUS

#define CONFIGURE_MAXIMUM_TASKS          TASK_COUNT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

  #define CONFIGURE_INIT_TASK_PRIORITY      8
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
