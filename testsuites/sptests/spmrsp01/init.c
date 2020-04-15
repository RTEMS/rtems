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

const char rtems_test_name[] = "SPMRSP 1";

typedef struct {
  rtems_id semaphore_id;
  rtems_id task_id;
} test_mrsp_context;

static void create_not_defined(rtems_attribute attr)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    attr,
    0,
    &id
  );
  rtems_test_assert(sc == RTEMS_NOT_DEFINED);
}

static void test_mrsp_create_errors(void)
{
  rtems_status_code sc;
  rtems_id id;

  puts("test MrsP create errors");

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    UINT32_MAX,
    &id
  );
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);

  create_not_defined(
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_COUNTING_SEMAPHORE
  );

  create_not_defined(
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_SIMPLE_BINARY_SEMAPHORE
  );

  create_not_defined(
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE
      | RTEMS_PRIORITY
  );

  create_not_defined(
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_INHERIT_PRIORITY
      | RTEMS_BINARY_SEMAPHORE
  );

  create_not_defined(
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_PRIORITY_CEILING
      | RTEMS_BINARY_SEMAPHORE
  );

  create_not_defined(
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_INHERIT_PRIORITY
      | RTEMS_PRIORITY_CEILING
      | RTEMS_BINARY_SEMAPHORE
  );
}

static void assert_prio(rtems_task_priority expected_prio)
{
  rtems_status_code sc;
  rtems_task_priority prio;

  sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == expected_prio);
}

static void test_mrsp_obtain_release(void)
{
  rtems_status_code sc;
  rtems_id id;

  puts("test MrsP obtain and release");

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(2);

  sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(1);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

  sc = rtems_semaphore_release(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  assert_prio(2);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_set_priority_errors(void)
{
  rtems_status_code sc;
  rtems_id id;
  rtems_id scheduler_id;
  rtems_task_priority prio;

  puts("test MrsP set priority errors");

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('C', 'O', 'N', 'T'),
    0,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  prio = 1;
  sc = rtems_semaphore_set_priority(RTEMS_ID_NONE, scheduler_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  prio = 1;
  sc = rtems_semaphore_set_priority(id, RTEMS_ID_NONE, prio, &prio);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  prio = 0xffffffff;
  sc = rtems_semaphore_set_priority(id, scheduler_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);

  prio = 1;
  sc = rtems_semaphore_set_priority(id, scheduler_id, prio, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);

  prio = 1;
  sc = rtems_semaphore_set_priority(id, scheduler_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_NOT_DEFINED);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_set_priority(void)
{
  rtems_status_code sc;
  rtems_id id;
  rtems_id scheduler_id;
  rtems_task_priority prio;

  puts("test MrsP set priority");

  sc = rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  prio = RTEMS_CURRENT_PRIORITY;
  sc = rtems_semaphore_set_priority(id, scheduler_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 1);

  prio = 1;
  sc = rtems_semaphore_set_priority(id, scheduler_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 1);

  prio = 2;
  sc = rtems_semaphore_set_priority(id, scheduler_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 1);

  prio = RTEMS_CURRENT_PRIORITY;
  sc = rtems_semaphore_set_priority(id, scheduler_id, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == 2);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_mrsp_task(rtems_task_argument arg)
{
  test_mrsp_context *ctx = (test_mrsp_context *) arg;
  rtems_status_code sc;

  sc = rtems_semaphore_release(ctx->semaphore_id);
  rtems_test_assert(sc == RTEMS_NOT_OWNER_OF_RESOURCE);

  sc = rtems_semaphore_obtain(ctx->semaphore_id, RTEMS_NO_WAIT, 0);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  sc = rtems_semaphore_obtain(ctx->semaphore_id, RTEMS_WAIT, 1);
  rtems_test_assert(sc == RTEMS_TIMEOUT);

  sc = rtems_event_transient_send(ctx->task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_exit();
  rtems_test_assert(0);
}

static void test_mrsp_timeout_and_not_owner_of_resource(void)
{
  rtems_status_code sc;
  rtems_id id;
  rtems_id task_id;
  test_mrsp_context ctx;

  puts("test MrsP timeout and not owner of resource");

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  ctx.semaphore_id = id;
  ctx.task_id = rtems_task_self();

  sc = rtems_task_start(task_id, test_mrsp_task, (rtems_task_argument) &ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_release(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument arg)
{
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  test_mrsp_create_errors();
  test_mrsp_obtain_release();
  test_mrsp_set_priority_errors();
  test_mrsp_set_priority();
  test_mrsp_timeout_and_not_owner_of_resource();

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT_TASK_PRIORITY 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
