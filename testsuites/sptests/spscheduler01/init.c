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

#include <limits.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPSCHEDULER 1";

#define BLUE rtems_build_name('b', 'l', 'u', 'e')

#define RED rtems_build_name('r', 'e', 'd', ' ')

static const rtems_id invalid_id = 1;

static void test_task_get_set_affinity(void)
{
#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)
  rtems_id self_id = rtems_task_self();
  rtems_id task_id;
  rtems_status_code sc;
  cpu_set_t cpusetone;
  cpu_set_t cpuset;
  size_t big = 2 * CHAR_BIT * sizeof(cpu_set_t);
  size_t cpusetbigsize = CPU_ALLOC_SIZE(big);
  cpu_set_t *cpusetbigone;
  cpu_set_t *cpusetbig;

  CPU_ZERO(&cpusetone);
  CPU_SET(0, &cpusetone);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_get_affinity(RTEMS_SELF, sizeof(cpuset), NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(cpuset), NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = rtems_task_get_affinity(RTEMS_SELF, 0, &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_task_set_affinity(RTEMS_SELF, 0, &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_task_get_affinity(invalid_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_task_set_affinity(invalid_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_task_get_affinity(RTEMS_SELF, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL(&cpuset, &cpusetone));

  sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_affinity(self_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_affinity(task_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_get_affinity(task_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL(&cpuset, &cpusetone));

  cpusetbigone = CPU_ALLOC(big);
  rtems_test_assert(cpusetbigone != NULL);

  cpusetbig = CPU_ALLOC(big);
  rtems_test_assert(cpusetbig != NULL);

  CPU_ZERO_S(cpusetbigsize, cpusetbigone);
  CPU_SET_S(0, cpusetbigsize, cpusetbigone);

  sc = rtems_task_get_affinity(task_id, cpusetbigsize, cpusetbig);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL_S(cpusetbigsize, cpusetbig, cpusetbigone));

  sc = rtems_task_set_affinity(task_id, cpusetbigsize, cpusetbig);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  CPU_FREE(cpusetbig);
  CPU_FREE(cpusetbigone);
#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */
}

static void task(rtems_task_argument arg)
{
  (void) arg;

  rtems_test_assert(0);
}

static void test_task_get_set_scheduler(void)
{
  rtems_status_code sc;
  rtems_id self_id = rtems_task_self();
  rtems_name name = BLUE;
  rtems_id scheduler_id;
  rtems_id scheduler_by_name;
  rtems_id task_id;

  sc = rtems_scheduler_ident(name, &scheduler_by_name);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_get_scheduler(RTEMS_SELF, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = rtems_task_get_scheduler(invalid_id, &scheduler_id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  scheduler_id = 0;
  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(scheduler_id == scheduler_by_name);

  scheduler_id = 0;
  sc = rtems_task_get_scheduler(self_id, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(scheduler_id == scheduler_by_name);

  sc = rtems_task_set_scheduler(invalid_id, scheduler_id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_task_set_scheduler(self_id, invalid_id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_task_set_scheduler(self_id, scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  scheduler_id = 0;
  sc = rtems_task_get_scheduler(task_id, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(scheduler_id == scheduler_by_name);

  sc = rtems_task_set_scheduler(task_id, scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_id, task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(task_id, scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_scheduler_ident(void)
{
  rtems_status_code sc;
  rtems_id expected_id = rtems_build_id(7, 1, 1, 1);
  rtems_id scheduler_id;
  rtems_name name = BLUE;
  rtems_name invalid_name = RED;

  sc = rtems_scheduler_ident(name, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = rtems_scheduler_ident(invalid_name, &scheduler_id);
  rtems_test_assert(sc == RTEMS_INVALID_NAME);

  scheduler_id = 0;
  sc = rtems_scheduler_ident(name, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(scheduler_id == expected_id);
}

static void test_scheduler_get_processors(void)
{
#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)
  rtems_status_code sc;
  rtems_name name = BLUE;
  rtems_id scheduler_id;
  cpu_set_t cpusetone;
  cpu_set_t cpuset;
  size_t big = 2 * CHAR_BIT * sizeof(cpu_set_t);
  size_t cpusetbigsize = CPU_ALLOC_SIZE(big);
  cpu_set_t *cpusetbigone;
  cpu_set_t *cpusetbig;

  CPU_ZERO(&cpusetone);
  CPU_SET(0, &cpusetone);

  sc = rtems_scheduler_ident(name, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_get_processor_set(scheduler_id, sizeof(cpuset), NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = rtems_scheduler_get_processor_set(invalid_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_scheduler_get_processor_set(scheduler_id, 0, &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_scheduler_get_processor_set(scheduler_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL(&cpuset, &cpusetone));

  cpusetbigone = CPU_ALLOC(big);
  rtems_test_assert(cpusetbigone != NULL);

  cpusetbig = CPU_ALLOC(big);
  rtems_test_assert(cpusetbig != NULL);

  CPU_ZERO_S(cpusetbigsize, cpusetbigone);
  CPU_SET_S(0, cpusetbigsize, cpusetbigone);

  sc = rtems_scheduler_get_processor_set(scheduler_id, cpusetbigsize, cpusetbig);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL_S(cpusetbigsize, cpusetbig, cpusetbigone));

  CPU_FREE(cpusetbig);
  CPU_FREE(cpusetbigone);
#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */
}

static void Init(rtems_task_argument arg)
{
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  test_task_get_set_affinity();
  test_task_get_set_scheduler();
  test_scheduler_ident();
  test_scheduler_get_processors();

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_SCHEDULER_NAME BLUE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
