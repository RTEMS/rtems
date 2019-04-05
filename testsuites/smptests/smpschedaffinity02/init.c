/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 * Test designed for 2 cores: Init task and TA1 task.
 * of equal priorities.  
 *
 *  - Set TA1 affinity to core 0 verify
 *  - Set TA1 affinity to core 1 verify it does not run because
 *    the Init task never blocks
 *  - Set Init affinity to core 0 verify both tasks are on the correct cores.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDAFFINITY 2";

#define NUM_CPUS   2

struct task_data_t {
  rtems_id   id;
  int        expected_cpu;
  cpu_set_t  cpuset;
  bool       ran;
  int        actual_cpu;
};

struct task_data_t task_data = {
  0x0, 0, {{0x3}}, false, -1
};

rtems_id           task_sem;

static void task(rtems_task_argument arg);
static void task_verify( bool ran, bool change_affinity, int cpu );
static void init_verify( int expect ); 

static void test_delay(int ticks)
{ 
  rtems_interval start, stop;
  start = rtems_clock_get_ticks_since_boot();
  do {
    stop = rtems_clock_get_ticks_since_boot();
  } while ( (stop - start) < ticks );
}

static void task_verify( bool ran, bool change_affinity, int cpu )
{
  rtems_status_code   sc;
  size_t              size = sizeof(cpu_set_t);

  /* Obtain the semaphore without blocking */
  while( rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0) != RTEMS_SUCCESSFUL );

  /* print the expected and actual values */
  printf( "TA01: expected=%d actual=%d ran=%d\n", 
   task_data.expected_cpu,
   task_data.actual_cpu,
   task_data.ran
   );

  /* Verify expected results */
  rtems_test_assert( task_data.ran == ran );
  if (ran)
    rtems_test_assert( task_data.expected_cpu == task_data.actual_cpu );

  if (change_affinity) {
    printf("Set TA01 to cpu %d\n", cpu);
    CPU_ZERO(&task_data.cpuset);
    CPU_SET(cpu, &task_data.cpuset);
    sc = rtems_task_set_affinity( task_data.id, size, &task_data.cpuset );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  /* Reset the states */
  task_data.ran = false;
  task_data.expected_cpu = cpu;

  /* Release the semaphore */
  rtems_semaphore_release(task_sem);
} 

static void init_verify( int expect ) 
{
  int cpu;


  test_delay(20);

  cpu = rtems_scheduler_get_processor();
  printf( "Init: expected=%d actual=%d\n", expect, cpu);
  rtems_test_assert( expect == cpu );
}

static void task(rtems_task_argument arg)
{
  rtems_status_code   sc;

  /* Never block and continually get core id  */
  while (true) {
    sc = rtems_semaphore_obtain (task_sem, RTEMS_NO_WAIT, 0);
    if (sc == RTEMS_SUCCESSFUL) {
      task_data.actual_cpu = rtems_scheduler_get_processor();
      task_data.ran = true;
      test_delay(25);
      rtems_semaphore_release(task_sem);
    }
  }
}

static void test(void)
{
  rtems_status_code   sc;
  uint32_t            cpu_count;
  rtems_id            id_self;
  cpu_set_t           cpuset;
 
  /* Get the number of processors that we are using. */
  cpu_count = rtems_scheduler_get_processor_maximum();
  if (cpu_count < NUM_CPUS) {
    printf("Error: Test requires at least 2 cpus\n");
    return;
  }

  id_self = rtems_task_self();
 
  printf("Create Semaphore\n");
  sc = rtems_semaphore_create(  
    rtems_build_name('S', 'E', 'M', '0'),
    1,
    RTEMS_BINARY_SEMAPHORE |
    RTEMS_PRIORITY | 
    RTEMS_PRIORITY_CEILING,
    0,
    &task_sem
  );  
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  printf("Create TA1\n");
  sc = rtems_task_create(
    rtems_build_name('T', 'A', '0', '1'),
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_data.id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* TA1 should start on cpu  0, since init starts on core 1 */
  sc = rtems_task_start( task_data.id, task, 1 );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Verify Init task is on cpu 1  */
  init_verify(1);

  /* Verify TA1 on cpu 0 and set the affinity to cpu 0 */
  task_verify( true, true, 0 );
   
  /* Verify Init task is on cpu 1  */
  init_verify(1);

  /* Verify TA1 on cpu 0 and change the affinity to cpu 1 */
  task_verify( true, true, 1 );

  /* Verify Init task is on cpu 1  */
  init_verify(1);

  /* Verify TA1 did not run */
  task_verify( false, false, 1 );

  /* Set affinity of Init to cpu 0 */
  printf("Set Affinity of init task to cpu 0\n");
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  sc = rtems_task_set_affinity( id_self, sizeof(cpuset), &cpuset );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* Verify init task went to cpu 0 */
  test_delay(50);
  init_verify(0);

  /* Verfiy TA1 is now running on cpu 1 */
  task_verify(true, false, 1);
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

#define CONFIGURE_MAXIMUM_TASKS          NUM_CPUS

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT_TASK_PRIORITY      4

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
