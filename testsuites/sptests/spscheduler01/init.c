/*
 * Copyright (C) 2014, 2020 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libcsupport.h>

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPSCHEDULER 1";

#define BLUE rtems_build_name('b', 'l', 'u', 'e')

#define RED rtems_build_name('r', 'e', 'd', ' ')

#define BIG_NUM_CPUS (2 * CHAR_BIT * sizeof(cpu_set_t))

static const rtems_id invalid_id = 1;

static rtems_id master_id;

static rtems_id sema_id;

static void assert_eno(rtems_status_code sc, int eno)
{
  switch (sc) {
    case RTEMS_SUCCESSFUL:
      rtems_test_assert(eno == 0);
      break;
    case RTEMS_INVALID_ADDRESS:
      rtems_test_assert(eno == EFAULT);
      break;
    case RTEMS_INVALID_ID:
      rtems_test_assert(eno == ESRCH);
      break;
    case RTEMS_INVALID_NUMBER:
      rtems_test_assert(eno == EINVAL);
      break;
    default:
      rtems_test_assert(0);
  }
}

static rtems_status_code task_get_affinity(
  rtems_id id,
  size_t cpusetsize,
  cpu_set_t *cpuset
)
{
  rtems_status_code sc;
  int eno;
  cpu_set_t *cpusetbig;

  sc = rtems_task_get_affinity(id, cpusetsize, cpuset);

  if (cpuset != NULL) {
    cpusetbig = CPU_ALLOC(BIG_NUM_CPUS);
    rtems_test_assert(cpusetbig != NULL);

    memcpy(cpusetbig, cpuset, cpusetsize);
  } else {
    cpusetbig = NULL;
  }

  eno = pthread_getaffinity_np(id, cpusetsize, cpusetbig);

  if (cpuset != NULL) {
    rtems_test_assert(CPU_EQUAL_S(cpusetsize, cpusetbig, cpuset));
    CPU_FREE(cpusetbig);
  }

  assert_eno(sc, eno);

  return sc;
}

static rtems_status_code task_set_affinity(
  rtems_id id,
  size_t cpusetsize,
  const cpu_set_t *cpuset
)
{
  rtems_status_code sc;
  int eno;

  sc = rtems_task_set_affinity(id, cpusetsize, cpuset);
  eno = pthread_setaffinity_np(id, cpusetsize, cpuset);
  assert_eno(sc, eno);

  return sc;
}

static void test_task_get_set_affinity(void)
{
  rtems_id self_id = rtems_task_self();
  rtems_id task_id;
  rtems_status_code sc;
  cpu_set_t cpusetone;
  cpu_set_t cpusetall;
  cpu_set_t cpuset;
  size_t cpusetbigsize = CPU_ALLOC_SIZE(BIG_NUM_CPUS);
  cpu_set_t *cpusetbigone;
  cpu_set_t *cpusetbig;

  CPU_ZERO(&cpusetone);
  CPU_SET(0, &cpusetone);

  CPU_FILL(&cpusetall);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = task_get_affinity(RTEMS_SELF, sizeof(cpuset), NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = task_set_affinity(RTEMS_SELF, sizeof(cpuset), NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = task_get_affinity(RTEMS_SELF, 0, &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = task_set_affinity(RTEMS_SELF, 0, &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = task_get_affinity(invalid_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = task_set_affinity(invalid_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = task_get_affinity(RTEMS_SELF, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL(&cpuset, &cpusetone));

  sc = task_set_affinity(RTEMS_SELF, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = task_set_affinity(self_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = task_set_affinity(task_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = task_get_affinity(task_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL(&cpuset, &cpusetone));

  sc = task_set_affinity(RTEMS_SELF, sizeof(cpusetall), &cpusetall);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = task_get_affinity(task_id, sizeof(cpuset), &cpuset);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL(&cpuset, &cpusetone));

  cpusetbigone = CPU_ALLOC(BIG_NUM_CPUS);
  rtems_test_assert(cpusetbigone != NULL);

  cpusetbig = CPU_ALLOC(BIG_NUM_CPUS);
  rtems_test_assert(cpusetbig != NULL);

  CPU_ZERO_S(cpusetbigsize, cpusetbigone);
  CPU_SET_S(0, cpusetbigsize, cpusetbigone);

  sc = task_get_affinity(task_id, cpusetbigsize, cpusetbig);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL_S(cpusetbigsize, cpusetbig, cpusetbigone));

  sc = task_set_affinity(task_id, cpusetbigsize, cpusetbig);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  CPU_FREE(cpusetbig);
  CPU_FREE(cpusetbigone);
}

static rtems_task_priority set_prio(rtems_id id, rtems_task_priority prio)
{
  rtems_status_code sc;
  rtems_task_priority old_prio;

  old_prio = 0xffffffff;
  sc = rtems_task_set_priority(id, prio, &old_prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return old_prio;
}

static void forbidden_task(rtems_task_argument arg)
{
  (void) arg;

  rtems_test_assert(0);
}

static void restart_task(rtems_task_argument arg)
{
  rtems_status_code sc;

  if (arg == 0) {
    rtems_test_assert(set_prio(RTEMS_SELF, 3) == 2);

    rtems_task_restart(RTEMS_SELF, 1);
  } else if (arg == 1) {
    rtems_id scheduler_id;

    rtems_test_assert(set_prio(RTEMS_SELF, 3) == 2);

    sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(RTEMS_SELF, scheduler_id, 4);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(set_prio(RTEMS_SELF, 3) == 4);

    rtems_task_restart(RTEMS_SELF, 2);
  } else {
    rtems_test_assert(set_prio(RTEMS_SELF, 3) == 4);

    rtems_task_resume(master_id);
  }

  rtems_test_assert(0);
}

static void sema_task(rtems_task_argument arg)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(sema_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

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
  rtems_id mtx_id;

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

  sc = rtems_task_set_scheduler(invalid_id, scheduler_id, 1);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_task_set_scheduler(self_id, invalid_id, 1);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_task_set_scheduler(self_id, scheduler_id, UINT32_C(0x80000000));
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);

  sc = rtems_task_set_scheduler(self_id, scheduler_id, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name(' ', 'M', 'T', 'X'),
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &mtx_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(self_id, scheduler_id, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(mtx_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(set_prio(self_id, RTEMS_CURRENT_PRIORITY) == 1);

  sc = rtems_task_set_scheduler(self_id, scheduler_id, 2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(set_prio(self_id, RTEMS_CURRENT_PRIORITY) == 2);

  sc = rtems_task_set_scheduler(self_id, scheduler_id, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(set_prio(self_id, RTEMS_CURRENT_PRIORITY) == 1);

  sc = rtems_semaphore_delete(mtx_id);
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

  sc = rtems_task_set_scheduler(task_id, scheduler_id, 2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_id, forbidden_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(task_id, scheduler_id, 2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(task_id);
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

  sc = rtems_task_start(task_id, restart_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(self_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    0,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &sema_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_id, sema_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(task_id, scheduler_id, 1);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

  sc = rtems_semaphore_delete(sema_id);
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
  cpu_set_t s;

  sc = rtems_scheduler_ident(name, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = rtems_scheduler_ident(invalid_name, &scheduler_id);
  rtems_test_assert(sc == RTEMS_INVALID_NAME);

  scheduler_id = 0;
  sc = rtems_scheduler_ident(name, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(scheduler_id == expected_id);

  sc = rtems_scheduler_ident_by_processor(1, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = rtems_scheduler_ident_by_processor(1, &scheduler_id);
  rtems_test_assert(sc == RTEMS_INVALID_NAME);

  scheduler_id = 0;
  sc = rtems_scheduler_ident_by_processor(0, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(scheduler_id == expected_id);

  CPU_ZERO(&s);
  CPU_SET(1, &s);
  sc = rtems_scheduler_ident_by_processor_set(sizeof(s), &s, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  sc = rtems_scheduler_ident_by_processor_set(1, &s, &scheduler_id);
  rtems_test_assert(sc == RTEMS_INVALID_SIZE);

  sc = rtems_scheduler_ident_by_processor_set(sizeof(s), &s, &scheduler_id);
  rtems_test_assert(sc == RTEMS_INVALID_NAME);

  CPU_SET(0, &s);
  scheduler_id = 0;
  sc = rtems_scheduler_ident_by_processor_set(sizeof(s), &s, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(scheduler_id == expected_id);
}

static void test_scheduler_get_max_prio(void)
{
  rtems_status_code sc;
  rtems_task_priority priority;
  rtems_id scheduler_id;

  priority = 0;
  sc = rtems_scheduler_get_maximum_priority(invalid_id, &priority);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
  rtems_test_assert(priority == 0);

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_get_maximum_priority(scheduler_id, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  priority = 0;
  sc = rtems_scheduler_get_maximum_priority(scheduler_id, &priority);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(priority == 255);
}

static void test_scheduler_map_to_posix(void)
{
  rtems_status_code sc;
  int posix_priority;
  rtems_id scheduler_id;

  posix_priority = 123;
  sc = rtems_scheduler_map_priority_to_posix(invalid_id, 1, &posix_priority);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
  rtems_test_assert(posix_priority == 123);

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_map_priority_to_posix(scheduler_id, 1, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  posix_priority = 123;
  sc = rtems_scheduler_map_priority_to_posix(scheduler_id, 0, &posix_priority);
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);
  rtems_test_assert(posix_priority == 123);

  posix_priority = 123;
  sc = rtems_scheduler_map_priority_to_posix(scheduler_id, 255, &posix_priority);
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);
  rtems_test_assert(posix_priority == 123);

  posix_priority = 123;
  sc = rtems_scheduler_map_priority_to_posix(scheduler_id, 256, &posix_priority);
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);
  rtems_test_assert(posix_priority == 123);

  posix_priority = 123;
  sc = rtems_scheduler_map_priority_to_posix(scheduler_id, 1, &posix_priority);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(posix_priority == 254);

  posix_priority = 123;
  sc = rtems_scheduler_map_priority_to_posix(scheduler_id, 254, &posix_priority);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(posix_priority == 1);
}

static void test_scheduler_map_from_posix(void)
{
  rtems_status_code sc;
  rtems_task_priority priority;
  rtems_id scheduler_id;

  priority = 123;
  sc = rtems_scheduler_map_priority_from_posix(invalid_id, 1, &priority);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
  rtems_test_assert(priority == 123);

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_map_priority_from_posix(scheduler_id, 1, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  priority = 123;
  sc = rtems_scheduler_map_priority_from_posix(scheduler_id, -1, &priority);
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);
  rtems_test_assert(priority == 123);

  priority = 123;
  sc = rtems_scheduler_map_priority_from_posix(scheduler_id, 0, &priority);
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);
  rtems_test_assert(priority == 123);

  priority = 123;
  sc = rtems_scheduler_map_priority_from_posix(scheduler_id, 255, &priority);
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);
  rtems_test_assert(priority == 123);

  priority = 123;
  sc = rtems_scheduler_map_priority_from_posix(scheduler_id, 1, &priority);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(priority == 254);

  priority = 123;
  sc = rtems_scheduler_map_priority_from_posix(scheduler_id, 254, &priority);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(priority == 1);
}

static void test_scheduler_get_processors(void)
{
  rtems_status_code sc;
  rtems_name name = BLUE;
  rtems_id scheduler_id;
  cpu_set_t cpusetone;
  cpu_set_t cpuset;
  size_t cpusetbigsize = CPU_ALLOC_SIZE(BIG_NUM_CPUS);
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

  cpusetbigone = CPU_ALLOC(BIG_NUM_CPUS);
  rtems_test_assert(cpusetbigone != NULL);

  cpusetbig = CPU_ALLOC(BIG_NUM_CPUS);
  rtems_test_assert(cpusetbig != NULL);

  CPU_ZERO_S(cpusetbigsize, cpusetbigone);
  CPU_SET_S(0, cpusetbigsize, cpusetbigone);

  sc = rtems_scheduler_get_processor_set(scheduler_id, cpusetbigsize, cpusetbig);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(CPU_EQUAL_S(cpusetbigsize, cpusetbig, cpusetbigone));

  CPU_FREE(cpusetbig);
  CPU_FREE(cpusetbigone);
}

static void test_scheduler_add_remove_processors(void)
{
  rtems_status_code sc;
  rtems_id scheduler_id;

  sc = rtems_scheduler_ident(BLUE, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_add_processor(invalid_id, 0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_scheduler_remove_processor(invalid_id, 0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  sc = rtems_scheduler_add_processor(scheduler_id, 1);
  rtems_test_assert(sc == RTEMS_NOT_CONFIGURED);

  sc = rtems_scheduler_remove_processor(scheduler_id, 1);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_scheduler_add_processor(scheduler_id, 0);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

  sc = rtems_scheduler_remove_processor(scheduler_id, 0);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);
}

static void test_task_get_priority(void)
{
  rtems_status_code sc;
  rtems_id scheduler_id;
  rtems_task_priority priority;
  rtems_task_priority priority_2;

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_get_priority(RTEMS_SELF, scheduler_id, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  priority = 0;

  sc = rtems_task_get_priority(RTEMS_SELF, invalid_id, &priority);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
  rtems_test_assert(priority == 0);

  sc = rtems_task_get_priority(invalid_id, scheduler_id, &priority);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
  rtems_test_assert(priority == 0);

  sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(priority != 0);

  priority_2 = 0;
  sc = rtems_task_get_priority(RTEMS_SELF, scheduler_id, &priority_2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(priority_2 == priority);

  priority_2 = 0;
  sc = rtems_task_get_priority(rtems_task_self(), scheduler_id, &priority_2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(priority_2 == priority);
}

static void Init(rtems_task_argument arg)
{
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  master_id = rtems_task_self();

  test_task_get_set_affinity();
  test_task_get_set_scheduler();
  test_scheduler_ident();
  test_scheduler_get_max_prio();
  test_scheduler_map_to_posix();
  test_scheduler_map_from_posix();
  test_scheduler_get_processors();
  test_scheduler_add_remove_processors();
  test_task_get_priority();

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_SCHEDULER_NAME BLUE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
