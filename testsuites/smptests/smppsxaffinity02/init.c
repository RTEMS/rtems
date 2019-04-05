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

#define  _GNU_SOURCE

#include <tmacros.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

const char rtems_test_name[] = "SMPPSXAFFINITY 2";

pthread_t           Init_id;
pthread_t           Med_id[NUM_CPUS-1];
pthread_t           Low_id[NUM_CPUS];

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void Validate_setaffinity_errors(void);
void Validate_getaffinity_errors(void);
void Validate_affinity(void);
void *Thread_1(void *unused);

void *Thread_1(void *unused)
{
  while(1);
}

void Validate_setaffinity_errors(void)
{
  int                 sc;
  cpu_set_t           cpuset;

  /* Verify pthread_setaffinity_np checks that more cpu's don't hurt. */
  CPU_FILL(&cpuset);
  puts( "Init - pthread_setaffinity_np - Lots of cpus - SUCCESS" );
  sc = pthread_setaffinity_np( Init_id, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == 0 );

  /* Verify pthread_setaffinity_np checks that at least one cpu is set */
  CPU_ZERO(&cpuset);
  puts( "Init - pthread_setaffinity_np - no cpu - EINVAL" );
  sc = pthread_setaffinity_np( Init_id, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_setaffinity_np checks that at thread id is valid */
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  puts( "Init - pthread_setaffinity_np - Invalid thread - ESRCH" );
  sc = pthread_setaffinity_np( 999, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == ESRCH );

  /* Verify pthread_setaffinity_np validates cpusetsize */
  puts( "Init - pthread_setaffinity_np - Invalid cpusetsize - EINVAL" );
  sc = pthread_setaffinity_np( Init_id,  1, &cpuset );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_setaffinity_np validates cpuset */
  puts( "Init - pthread_setaffinity_np - Invalid cpuset - EFAULT" );
  sc = pthread_setaffinity_np( Init_id, sizeof(cpu_set_t), NULL );
  rtems_test_assert( sc == EFAULT );
}

void Validate_getaffinity_errors(void)
{
  int                 sc;
  cpu_set_t           cpuset;

  /* Verify pthread_getaffinity_np checks that at thread id is valid */
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  puts( "Init - pthread_getaffinity_np - Invalid thread - ESRCH" );
  sc = pthread_getaffinity_np( 999, sizeof(cpu_set_t), &cpuset );
  rtems_test_assert( sc == ESRCH );

  /* Verify pthread_getaffinity_np validates cpusetsize */
  puts( "Init - pthread_getaffinity_np - Invalid cpusetsize - EINVAL" );
  sc = pthread_getaffinity_np( Init_id,  1, &cpuset );
  rtems_test_assert( sc == EINVAL );

  /* Verify pthread_getaffinity_np validates cpuset */
  puts("Init - pthread_getaffinity_np - Invalid cpuset - EFAULT");
  sc = pthread_getaffinity_np( Init_id, sizeof(cpu_set_t), NULL );
  rtems_test_assert( sc == EFAULT );
}

void Validate_affinity(void )
{
  pthread_attr_t       attr;
  cpu_set_t            cpuset0;
  cpu_set_t            cpuset1;
  cpu_set_t            cpuset2;
  uint32_t             i;
  int                  sc;
  int                  cpu_count;
  struct sched_param   param;


  puts( "Init - Set Init priority to high");
  sc = pthread_getattr_np( Init_id, &attr );
  rtems_test_assert( sc == 0 );

  sc = pthread_attr_setstack( &attr, NULL, 0 );
  rtems_test_assert( sc == 0 );

  sc = pthread_attr_getschedparam( &attr, &param );
  rtems_test_assert( sc == 0 );
  param.sched_priority = sched_get_priority_max( SCHED_FIFO );
  sc = pthread_setschedparam( Init_id, SCHED_FIFO, &param );
  rtems_test_assert( !sc );

  sc = pthread_getaffinity_np( Init_id, sizeof(cpu_set_t), &cpuset0 );
  rtems_test_assert( !sc );

  /* Get the number of processors that we are using. */
  cpu_count = rtems_scheduler_get_processor_maximum();

  /* Fill the remaining cpus with med priority tasks */
  puts( "Init - Create Medium priority tasks");
  for (i=0; i<(cpu_count-1); i++){
    sc = pthread_create( &Med_id[i], &attr, Thread_1, NULL );
    rtems_test_assert( !sc );
  }

  puts( "Init - Verify Medium priority tasks");
  for (i=0; i<(cpu_count-1); i++){
    sc = pthread_getaffinity_np( Med_id[i], sizeof(cpu_set_t), &cpuset2 );
    rtems_test_assert( !sc );
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

    sc = pthread_attr_setaffinity_np( &attr, sizeof(cpu_set_t), &cpuset1 );
    rtems_test_assert( !sc );

    sc = pthread_create( &Low_id[i], &attr, Thread_1, NULL );
    rtems_test_assert( !sc );
  }

  /* Verify affinity on low priority tasks */
  puts( "Init - Verify Low priority tasks");
  for (i=0; i<(cpu_count-1); i++){
    CPU_ZERO(&cpuset1);
    CPU_SET(i, &cpuset1);

    sc = pthread_getaffinity_np( Low_id[i], sizeof(cpu_set_t), &cpuset2 );
    rtems_test_assert( !sc );
    rtems_test_assert( CPU_EQUAL(&cpuset1, &cpuset2) );
  }

  /* Change the affinity for each low priority task */
  puts("Init - Change affinity on Low priority tasks");
  CPU_COPY(&cpuset0, &cpuset1);
  for (i=0; i<cpu_count; i++){

    CPU_CLR(i, &cpuset1);
    sc = pthread_setaffinity_np( Low_id[i], sizeof(cpu_set_t), &cpuset1 );

    /* Verify no cpu's are now set in the cpuset */
    if (i== (cpu_count-1)) {
      rtems_test_assert( sc == EINVAL );
      sc = pthread_setaffinity_np( Low_id[i], sizeof(cpu_set_t), &cpuset0 );
    }
    rtems_test_assert( !sc );
  }

  puts("Init - Validate affinity on Low priority tasks");
  CPU_COPY(&cpuset0, &cpuset1);
  for (i=0; i<cpu_count; i++){
    CPU_CLR(i, &cpuset1);

    sc = pthread_getaffinity_np( Low_id[i], sizeof(cpu_set_t), &cpuset2 );
    rtems_test_assert( !sc );
    if (i== (cpu_count-1))
      rtems_test_assert( CPU_EQUAL(&cpuset0, &cpuset2) );
    else
      rtems_test_assert( CPU_EQUAL(&cpuset1, &cpuset2) );
  }
}

void *POSIX_Init(
  void *ignored
)
{
  TEST_BEGIN();

  /* Initialize thread id */
  Init_id = pthread_self();

  Validate_setaffinity_errors();
  Validate_getaffinity_errors();
  Validate_affinity();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_MAXIMUM_PROCESSORS NUM_CPUS

#define CONFIGURE_MAXIMUM_POSIX_THREADS (NUM_CPUS*2)

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
