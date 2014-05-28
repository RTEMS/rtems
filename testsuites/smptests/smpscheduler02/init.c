/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>
#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPSCHEDULER 2";

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)

#define SCHED_A rtems_build_name(' ', ' ', ' ', 'A')

#define SCHED_B rtems_build_name(' ', ' ', ' ', 'B')

#define SCHED_C rtems_build_name(' ', ' ', ' ', 'C')

static rtems_id main_task_id;

static void task(rtems_task_argument arg)
{
  rtems_status_code sc;

  (void) arg;

  rtems_test_assert(rtems_get_current_processor() == 1);

  sc = rtems_event_transient_send(main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (1) {
    /* Do nothing */
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
  cpu_set_t cpuset;
  cpu_set_t first_cpu;
  cpu_set_t second_cpu;
  cpu_set_t all_cpus;

  main_task_id = rtems_task_self();

  CPU_ZERO(&first_cpu);
  CPU_SET(0, &first_cpu);

  CPU_ZERO(&second_cpu);
  CPU_SET(1, &second_cpu);

  CPU_ZERO(&all_cpus);
  CPU_SET(0, &all_cpus);
  CPU_SET(1, &all_cpus);

  rtems_test_assert(rtems_get_current_processor() == 0);

  sc = rtems_scheduler_ident(SCHED_A, &scheduler_a_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_ident(SCHED_B, &scheduler_b_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(scheduler_a_id != scheduler_b_id);

  sc = rtems_scheduler_ident(SCHED_C, &scheduler_c_id);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  CPU_ZERO(&cpuset);
  sc = rtems_scheduler_get_processor_set(
    scheduler_a_id,
    sizeof(cpuset),
    &cpuset
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(CPU_EQUAL(&cpuset, &first_cpu));

  CPU_ZERO(&cpuset);
  sc = rtems_scheduler_get_processor_set(
    scheduler_b_id,
    sizeof(cpuset),
    &cpuset
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(CPU_EQUAL(&cpuset, &second_cpu));

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
  rtems_test_assert(CPU_EQUAL(&cpuset, &first_cpu));

  sc = rtems_task_set_scheduler(task_id, scheduler_b_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(task_id, scheduler_b_id + 1);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_task_get_scheduler(task_id, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(scheduler_id == scheduler_b_id);

  CPU_ZERO(&cpuset);
  sc = rtems_task_get_affinity(task_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(CPU_EQUAL(&cpuset, &second_cpu));

  sc = rtems_task_set_affinity(task_id, sizeof(all_cpus), &all_cpus);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_affinity(task_id, sizeof(first_cpu), &first_cpu);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_task_get_scheduler(task_id, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(scheduler_id == scheduler_b_id);

  sc = rtems_task_set_affinity(task_id, sizeof(second_cpu), &second_cpu);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_get_scheduler(task_id, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(scheduler_id == scheduler_b_id);

  sc = rtems_task_start(task_id, task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(task_id, scheduler_b_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

#else /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */

static void test(void)
{
  /* Nothing to do */
}

#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */

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
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

/* Lets see when the first RTEMS system hits this limit */
#define CONFIGURE_SMP_MAXIMUM_PROCESSORS 64

#define CONFIGURE_MAXIMUM_PRIORITY 255

#define CONFIGURE_SCHEDULER_PRIORITY_SMP
#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP(a, CONFIGURE_MAXIMUM_PRIORITY + 1);

RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP(b, CONFIGURE_MAXIMUM_PRIORITY + 1);

RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(c);

#define CONFIGURE_SCHEDULER_CONTROLS \
  RTEMS_SCHEDULER_CONTROL_PRIORITY_SMP(a, SCHED_A), \
  RTEMS_SCHEDULER_CONTROL_PRIORITY_SMP(b, SCHED_B), \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(c, SCHED_C)

#define CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
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

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
