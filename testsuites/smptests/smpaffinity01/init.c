/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define NUM_CPUS   4

#include "tmacros.h"

const char rtems_test_name[] = "SMPAFFINITY 1";

rtems_id           Init_id;
rtems_id           Med_id[NUM_CPUS-1];
rtems_id           Low_id[NUM_CPUS];

/* forward declarations to avoid warnings */
void Task_1(rtems_task_argument arg);
void Validate_setaffinity_errors(void);
void Validate_getaffinity_errors(void);
void Validate_affinity(void);

void Task_1(rtems_task_argument arg)
{
  while(1);
}

void Validate_setaffinity_errors(void)
{
  int                 sc;
  cpu_set_t           cpuset;

  /* Verify rtems_task_set_affinity checks that all cpu's exist. */
  /* Note this check assumes you are running with less than 32 CPUs */
  CPU_FILL(&cpuset);
  puts( "Init - rtems_task_set_affinity - Lots of cpus - SUCCESS" );
  sc = rtems_task_set_affinity( Init_id, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  /* Verify rtems_task_set_affinity checks that at least one cpu is set */
  CPU_ZERO(&cpuset);
  puts( "Init - rtems_task_set_affinity - no cpu - RTEMS_INVALID_NUMBER" );
  sc = rtems_task_set_affinity( Init_id, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == RTEMS_INVALID_NUMBER );

  /* Verify rtems_task_set_affinity checks that at thread id is valid */
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  puts( "Init - rtems_task_set_affinity - Invalid thread - RTEMS_INVALID_ID" );
  sc = rtems_task_set_affinity( 999, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == RTEMS_INVALID_ID );

  /* Verify rtems_task_set_affinity validates cpusetsize */
  puts( "Init - rtems_task_set_affinity - Invalid cpusetsize - RTEMS_INVALID_NUMBER" );
  sc = rtems_task_set_affinity( Init_id,  1, &cpuset );
  rtems_test_assert( sc == RTEMS_INVALID_NUMBER );

  /* Verifyrtems_task_set_affinity validates cpuset */
  puts( "Init - rtems_task_set_affinity - Invalid cpuset - RTEMS_INVALID_ADDRESS" );
  sc = rtems_task_set_affinity( Init_id, sizeof(cpu_set_t), NULL );
  rtems_test_assert( sc == RTEMS_INVALID_ADDRESS );
}

void Validate_getaffinity_errors(void)
{
  int                 sc;
  cpu_set_t           cpuset;

  /* Verify rtems_task_get_affinity checks that at thread id is valid */
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  puts( "Init - rtems_task_get_affinity - Invalid thread - RTEMS_INVALID_ID" );
  sc = rtems_task_get_affinity( 999, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == RTEMS_INVALID_ID );

  /* Verify rtems_task_get_affinity validates cpusetsize */
  puts(
    "Init - rtems_task_get_affinity - Invalid cpusetsize - RTEMS_INVALID_NUMBER"
  );
  sc = rtems_task_get_affinity( Init_id,  1, &cpuset );
  rtems_test_assert( sc == RTEMS_INVALID_NUMBER );

  /* Verify rtems_task_get_affinity validates cpuset */
  puts("Init - rtems_task_get_affinity - Invalid cpuset - RTEMS_INVALID_ADDRESS");
  sc = rtems_task_get_affinity( Init_id, sizeof(cpu_set_t), NULL );
  rtems_test_assert( sc == RTEMS_INVALID_ADDRESS );
}

void Validate_affinity(void )
{
  cpu_set_t            cpuset0;
  cpu_set_t            cpuset1;
  cpu_set_t            cpuset2;
  uint32_t             i;
  int                  sc;
  int                  cpu_count;
  rtems_task_priority  priority;
  char                 ch[2];

  puts( "Init - Set Init priority to high");
  sc = rtems_task_set_priority( Init_id, 1, &priority );
  directive_failed( sc, "Set Init Priority" );

  sc = rtems_task_get_affinity( Init_id, sizeof(cpu_set_t), &cpuset0 );
  directive_failed( sc, "Get Affinity of Init Task" );

  /* Get the number of processors that we are using. */
  cpu_count = rtems_scheduler_get_processor_maximum();

  /* Fill the remaining cpus with med priority tasks */
  puts( "Init - Create Medium priority tasks");
  for (i=0; i<(cpu_count-1); i++){
    sprintf(ch, "%01" PRId32, i+1 );
    sc = rtems_task_create(
      rtems_build_name( 'C', 'P', 'U', ch[0] ),
      2,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Med_id[i]
    );
    directive_failed( sc, "task create" );

    sc = rtems_task_start( Med_id[i], Task_1, i+1 );
    directive_failed( sc, "task start" );

    sc = rtems_task_get_affinity( Med_id[i], sizeof(cpu_set_t), &cpuset2 );
    directive_failed( sc, "Get Affinity of Medium Priority Task" );
    rtems_test_assert( CPU_EQUAL(&cpuset0, &cpuset2) );
  }

  /*
   * Create low priority thread for each remaining cpu with the affinity
   * set to only run on one cpu.
   */
  puts( "Init - Create  Low priority tasks");
  for (i=0; i<cpu_count; i++){
    CPU_ZERO(&cpuset1);
    CPU_SET(i, &cpuset1);

    sprintf(ch, "%01" PRId32, (uint32_t) 0 );
    sc = rtems_task_create(
      rtems_build_name( 'X', 'T', 'R', ch[0] ),
      10,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Low_id[i]
    );
    directive_failed( sc, "task create" );

    sc = rtems_task_set_affinity( Low_id[i], sizeof(cpu_set_t), &cpuset1 );
    directive_failed( sc, "Low priority task set affinity" );

    sc = rtems_task_start( Low_id[i], Task_1, i+1 );
    directive_failed( sc, "task start" );
  }


  /* Verify affinity on low priority tasks */
  puts("Init - Verify affinity on Low priority tasks");
  for (i=0; i<cpu_count; i++){
    CPU_ZERO(&cpuset1);
    CPU_SET(i, &cpuset1);

    sc = rtems_task_get_affinity( Low_id[i], sizeof(cpu_set_t), &cpuset2 );
    directive_failed( sc, "Low priority task get affinity" );
    rtems_test_assert( CPU_EQUAL(&cpuset1, &cpuset2) );
  }

  /* Change the affinity for each low priority task */
  puts("Init - Change affinity on Low priority tasks");
  CPU_COPY(&cpuset0, &cpuset1);
  for (i=0; i<cpu_count; i++){

    CPU_CLR(i, &cpuset1);
    sc = rtems_task_set_affinity( Low_id[i], sizeof(cpu_set_t), &cpuset1 );

    /* Verify no cpu's are now set in the cpuset */
    if (i== (cpu_count-1)) {
      rtems_test_assert( sc == RTEMS_INVALID_NUMBER );
      sc = rtems_task_set_affinity( Low_id[i], sizeof(cpu_set_t), &cpuset0 );
    }

    directive_failed( sc, "Low priority task set affinity" );
  }

  puts("Init - Validate affinity on Low priority tasks");
  CPU_COPY(&cpuset0, &cpuset1);
  for (i=0; i<cpu_count; i++){
    CPU_CLR(i, &cpuset1);

    sc = rtems_task_get_affinity( Low_id[i], sizeof(cpu_set_t), &cpuset2 );
    directive_failed( sc, "Low priority task get affinity" );
    if (i== (cpu_count-1))
      rtems_test_assert( CPU_EQUAL(&cpuset0, &cpuset2) );
    else
      rtems_test_assert( CPU_EQUAL(&cpuset1, &cpuset2) );
  }
}

static void Init(rtems_task_argument arg)
{
  int                  sc;

  TEST_BEGIN();

  /* Initialize thread id */
  sc = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &Init_id );
  directive_failed( sc, "Identify Init Task" );

  Validate_setaffinity_errors();
  Validate_getaffinity_errors();
  Validate_affinity();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_MAXIMUM_PROCESSORS NUM_CPUS

#define CONFIGURE_MAXIMUM_TASKS         (NUM_CPUS*2)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
