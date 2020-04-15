/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 * Test to walk the affinity of the init task across all the cores.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDAFFINITY 3";

#define NUM_CPUS   4
#define TASK_COUNT NUM_CPUS

static void test_delay(int ticks)
{ 
  rtems_interval start, stop;
  start = rtems_clock_get_ticks_since_boot();
  do {
    stop = rtems_clock_get_ticks_since_boot();
  } while ( (stop - start) < ticks );
}

static void test(void)
{
  rtems_status_code   sc;
  rtems_id            id;
  uint32_t            cpu_count;
  int                 cpu;
  int                 i;
  cpu_set_t           cpuset;

  /* Get the number of processors that we are using. */
  cpu_count = rtems_scheduler_get_processor_maximum();
 
  id = rtems_task_self();

  /* 
   * The Init task comes up on the maximum core so start at
   * that core and walk the affinity down to core 0.
   */
  for( i=cpu_count-1; i >= 0; i--) {
    
    /* Move the affinity to the current core - 1 */
    CPU_ZERO(&cpuset);
    CPU_SET(i, &cpuset);
    printf("Set Affinity for cpu %d\n", i);
    sc = rtems_task_set_affinity( id, sizeof(cpuset), &cpuset );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    /* Wait 100 clock ticks */
    test_delay(100);

    /* Check the cpu the Init task is running on */
    cpu = rtems_scheduler_get_processor();
    printf("On cpu %d\n", cpu);
    rtems_test_assert(cpu == i);
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
