/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sched.h>
#include <limits.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDULER 2";

#define SCHED_A rtems_build_name(' ', ' ', ' ', 'A')

#define SCHED_B rtems_build_name(' ', ' ', ' ', 'B')

#define SCHED_C rtems_build_name(' ', ' ', ' ', 'C')

static rtems_id main_task_id;

static rtems_id cmtx_id;

static rtems_id imtx_id;

static volatile bool ready;

static void task(rtems_task_argument arg)
{
  rtems_status_code sc;

  (void) arg;

  rtems_test_assert(rtems_scheduler_get_processor() == 1);
  rtems_test_assert(sched_get_priority_min(SCHED_RR) == 1);
  rtems_test_assert(sched_get_priority_max(SCHED_RR) == INT_MAX - 1);

  sc = rtems_semaphore_obtain(cmtx_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_NOT_DEFINED);

  sc = rtems_event_transient_send(main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(imtx_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(imtx_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_send(main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (1) {
    /* Do nothing */
  }
}

static void sticky_task(rtems_task_argument arg)
{
  rtems_status_code sc;
  rtems_id mtx_id;

  (void) arg;

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  sc = rtems_semaphore_create(
    rtems_build_name(' ', 'M', 'T', 'X'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    2,
    &mtx_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(mtx_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  ready = true;

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(mtx_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(mtx_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_send(main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (1) {
    /* Do nothing */
  }
}

static void test_scheduler_add_remove_processors(void)
{
  rtems_status_code sc;
  rtems_id scheduler_a_id;
  rtems_id scheduler_c_id;

  sc = rtems_scheduler_ident(SCHED_A, &scheduler_a_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_ident(SCHED_C, &scheduler_c_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_add_processor(scheduler_c_id, 62);
  rtems_test_assert(sc == RTEMS_NOT_CONFIGURED);

  sc = rtems_scheduler_add_processor(scheduler_c_id, 63);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);

  sc = rtems_scheduler_remove_processor(scheduler_c_id, 62);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_scheduler_remove_processor(scheduler_a_id, 0);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

  if (rtems_scheduler_get_processor_maximum() > 1) {
    rtems_id scheduler_id;
    rtems_id scheduler_b_id;
    rtems_id task_id;
    cpu_set_t first_cpu;

    sc = rtems_scheduler_ident(SCHED_B, &scheduler_b_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_scheduler_remove_processor(scheduler_b_id, 1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_scheduler_add_processor(scheduler_a_id, 1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(rtems_scheduler_get_processor() == 0);

    sc = rtems_scheduler_remove_processor(scheduler_a_id, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(rtems_scheduler_get_processor() == 1);

    CPU_ZERO(&first_cpu);
    CPU_SET(0, &first_cpu);
    sc = rtems_scheduler_ident_by_processor_set(
      sizeof(first_cpu),
      &first_cpu,
      &scheduler_id
    );
    rtems_test_assert(sc == RTEMS_INCORRECT_STATE);

    sc = rtems_scheduler_add_processor(scheduler_a_id, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(rtems_scheduler_get_processor() == 1);

    sc = rtems_task_create(
      rtems_build_name('T', 'A', 'S', 'K'),
      2,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(task_id, sticky_task, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    while (!ready) {
      /* Wait */
    }

    sc = rtems_scheduler_remove_processor(scheduler_a_id, 1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(rtems_scheduler_get_processor() == 0);

    sc = rtems_event_transient_send(task_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_delete(task_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_scheduler_add_processor(scheduler_b_id, 1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void test(void)
{
  rtems_status_code sc;
  rtems_id task_id;
  rtems_id scheduler_id;
  rtems_id scheduler_a_id;
  rtems_id scheduler_b_id;
  rtems_id scheduler_c_id;
  rtems_task_priority prio;
  cpu_set_t cpuset;
  cpu_set_t first_cpu;
  cpu_set_t second_cpu;
  cpu_set_t all_cpus;
  cpu_set_t online_cpus;
  uint32_t cpu_count;

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  cpu_count = rtems_scheduler_get_processor_maximum();
  main_task_id = rtems_task_self();

  CPU_ZERO(&first_cpu);
  CPU_SET(0, &first_cpu);

  CPU_ZERO(&second_cpu);
  CPU_SET(1, &second_cpu);

  CPU_FILL(&all_cpus);

  CPU_ZERO(&online_cpus);
  CPU_SET(0, &online_cpus);

  if (cpu_count > 1) {
    CPU_SET(1, &online_cpus);
  }

  sc = rtems_scheduler_ident(SCHED_A, &scheduler_a_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  if (cpu_count > 1) {
    sc = rtems_scheduler_ident(SCHED_B, &scheduler_b_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(scheduler_a_id != scheduler_b_id);
  }

  sc = rtems_scheduler_ident(SCHED_C, &scheduler_c_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('C', 'M', 'T', 'X'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    1,
    &cmtx_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('I', 'M', 'T', 'X'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    1,
    &imtx_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  prio = 2;
  sc = rtems_semaphore_set_priority(cmtx_id, scheduler_a_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 1);

  if (cpu_count > 1) {
    prio = 1;
    sc = rtems_semaphore_set_priority(cmtx_id, scheduler_b_id, prio, &prio);
    rtems_test_assert(sc == RTEMS_NOT_DEFINED);
    rtems_test_assert(prio == 2);
  }

  CPU_ZERO(&cpuset);
  sc = rtems_scheduler_get_processor_set(
    scheduler_a_id,
    sizeof(cpuset),
    &cpuset
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(CPU_EQUAL(&cpuset, &first_cpu));

  if (cpu_count > 1) {
    CPU_ZERO(&cpuset);
    sc = rtems_scheduler_get_processor_set(
      scheduler_b_id,
      sizeof(cpuset),
      &cpuset
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(CPU_EQUAL(&cpuset, &second_cpu));
  }

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_get_scheduler(task_id, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(scheduler_id == scheduler_a_id);

  CPU_ZERO(&cpuset);
  sc = rtems_task_get_affinity(task_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(CPU_EQUAL(&cpuset, &online_cpus));

  rtems_test_assert(sched_get_priority_min(SCHED_RR) == 1);
  rtems_test_assert(sched_get_priority_max(SCHED_RR) == 254);

  sc = rtems_task_set_scheduler(task_id, scheduler_c_id, 1);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  sc = rtems_task_set_scheduler(task_id, scheduler_c_id + 1, 1);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  if (cpu_count > 1) {
    sc = rtems_task_set_scheduler(task_id, scheduler_b_id, 1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_get_scheduler(task_id, &scheduler_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(scheduler_id == scheduler_b_id);

    CPU_ZERO(&cpuset);
    sc = rtems_task_get_affinity(task_id, sizeof(cpuset), &cpuset);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(CPU_EQUAL(&cpuset, &online_cpus));

    sc = rtems_task_set_affinity(task_id, sizeof(all_cpus), &all_cpus);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_affinity(task_id, sizeof(first_cpu), &first_cpu);
    rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

    sc = rtems_task_get_scheduler(task_id, &scheduler_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(scheduler_id == scheduler_b_id);

    sc = rtems_task_set_affinity(task_id, sizeof(online_cpus), &online_cpus);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_affinity(task_id, sizeof(second_cpu), &second_cpu);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(task_id, scheduler_a_id, 1);
    rtems_test_assert(sc == RTEMS_UNSATISFIED);

    sc = rtems_task_get_scheduler(task_id, &scheduler_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(scheduler_id == scheduler_b_id);

    sc = rtems_semaphore_obtain(imtx_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(task_id, scheduler_b_id, 1);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(task_id, task, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    /* Ensure that the other task waits for the mutex owned by us */
    sc = rtems_task_wake_after(2);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(RTEMS_SELF, scheduler_b_id, 1);
    rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

    sc = rtems_semaphore_release(imtx_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(cmtx_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(imtx_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_scheduler_add_remove_processors();
}

static void Init(rtems_task_argument arg)
{
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  test();

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_SEMAPHORES 2
#define CONFIGURE_MAXIMUM_MRSP_SEMAPHORES 1

/* Lets see when the first RTEMS system hits this limit */
#define CONFIGURE_MAXIMUM_PROCESSORS 64

#define CONFIGURE_SCHEDULER_EDF_SMP
#define CONFIGURE_SCHEDULER_PRIORITY_SMP
#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_PRIORITY_SMP(a, 256);

RTEMS_SCHEDULER_EDF_SMP(b, CONFIGURE_MAXIMUM_PROCESSORS);

RTEMS_SCHEDULER_SIMPLE_SMP(c);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_PRIORITY_SMP(a, SCHED_A), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(b, SCHED_B), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(c, SCHED_C)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL), \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER, \
  RTEMS_SCHEDULER_ASSIGN(2, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
