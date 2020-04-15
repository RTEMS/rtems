/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 * The init task UT1 should start on cpu 3 and has priority:affinity set
 * 7:{2,3} The test creates 4 more tasks TA1 - TA4
 * with priorty:affinity sets 8:{2,3}, 5:{0,1}, 6:{0,3}, and 9:{1}.
 * This should result in cpu:task  0:TA3, 1:TA2, 2:TA1, 3:UT1 with
 * TA4 waiting on a cpu.
 *
 * The test then raises the priority of TA4 to 4, resulting
 * in the following cpu:task 0:TA2, 1:TA4, 2:UT1, 3:TA3 with
 * TA1 waiting on a CPU.  The tasks are then terminated.
 *
 * The capture engine is set up read and report the results.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/captureimpl.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPCAPTURE 1";

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
      test_delay(1);
    }
  }
}

static void set_init_task(void)
{
  while( rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0) != RTEMS_SUCCESSFUL );

  /* Set Init task data */
  task_data[0].ran = true;
  task_data[0].actual_cpu = rtems_scheduler_get_processor();

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

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', '0'),
    1,                                               /* initial count = 1 */
    RTEMS_LOCAL                   |
    RTEMS_SIMPLE_BINARY_SEMAPHORE |
    RTEMS_NO_INHERIT_PRIORITY     |
    RTEMS_NO_PRIORITY_CEILING     |
    RTEMS_FIFO,
    0,
    &task_sem
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_affinity(
    task_data[ 0 ].id,
    size,
    &task_data[0].cpuset
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

      sc = rtems_task_start( task_data[ i ].id, task, i );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  /* spin for 10 ticks */
  test_delay(10);

  set_init_task();

  i = TASK_COUNT - 1;
  task_data[ i ].priority = 4;
  sc = rtems_task_set_priority(
    task_data[ i ].id,
    task_data[ i ].priority,
    &priority
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_delay(10);

  while( rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0) != RTEMS_SUCCESSFUL );
  for (i = 0; i < TASK_COUNT; i++) {
    task_data[ i ].expected_cpu = task_data[ i ].migrate_cpu;
    task_data[ i ].actual_cpu = -1;
    task_data[ i ].ran = false;
  }
  rtems_semaphore_release(task_sem);
  test_delay(10);
  set_init_task();

  for (i = 1; i < TASK_COUNT; i++) {
    sc = rtems_task_delete( task_data[ i ].id );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
  test_delay(25);
}

static void Init(rtems_task_argument arg)
{
  rtems_status_code   sc;
  rtems_name          to_name = rtems_build_name('I', 'D', 'L', 'E');;
  uint32_t            i;

  TEST_BEGIN();

  sc = rtems_capture_open (5000, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_capture_watch_ceiling (0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_capture_watch_floor (20);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_capture_watch_global (true);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_capture_set_trigger (
    0,
    0,
    to_name,
    0,
    rtems_capture_from_any,
    rtems_capture_switch
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 1; i < TASK_COUNT; i++) {
     to_name = rtems_build_name('T', 'A', '0', '0'+i);
     sc = rtems_capture_set_trigger (
      0,
      0,
      to_name,
      0,
      rtems_capture_from_any,
      rtems_capture_switch
    );
  }

  sc = rtems_capture_set_control (true);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test();

  sc = rtems_capture_set_control (false);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_capture_print_trace_records ( 22, false );
  rtems_capture_print_trace_records ( 22, false );
  rtems_capture_print_trace_records ( 22, false );

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MAXIMUM_PROCESSORS NUM_CPUS

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY       7
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_ALLOCATION_SIZE     (5)
#define CONFIGURE_MAXIMUM_TASKS  rtems_resource_unlimited(TASK_ALLOCATION_SIZE)
#define CONFIGURE_EXTRA_TASK_STACKS (75 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS (5)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
