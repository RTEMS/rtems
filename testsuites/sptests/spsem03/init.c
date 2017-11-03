/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
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

#include "tmacros.h"

const char rtems_test_name[] = "SPSEM 3";

typedef struct {
  rtems_id low;
  rtems_id mid;
  rtems_id high;
  rtems_id inversion;
  rtems_id sem_a;
  rtems_id sem_b;
} test_context;

static test_context test_instance;

static void assert_prio(rtems_id task_id, rtems_task_priority expected_prio)
{
  rtems_status_code sc;
  rtems_task_priority prio;

  sc = rtems_task_set_priority(task_id, RTEMS_CURRENT_PRIORITY, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(prio == expected_prio);
}

static void create_task(rtems_id *id, rtems_task_priority prio)
{
  rtems_status_code sc;

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    prio,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void start_task(rtems_id id, rtems_task_entry entry)
{
  rtems_status_code sc;

  sc = rtems_task_start(id, entry, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void create_sema(rtems_id *id)
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
    0,
    id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void obtain_sema(rtems_id id)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void inversion_task(rtems_task_argument arg)
{
  rtems_test_assert(0);
}

static void mid_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  obtain_sema(ctx->sem_b);
  obtain_sema(ctx->sem_a);
}

static void high_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  start_task(ctx->inversion, inversion_task);
  obtain_sema(ctx->sem_b);
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  TEST_BEGIN();

  ctx->low = rtems_task_self();

  create_task(&ctx->mid, 3);
  create_task(&ctx->high, 1);
  create_task(&ctx->inversion, 2);
  create_sema(&ctx->sem_a);
  create_sema(&ctx->sem_b);

  obtain_sema(ctx->sem_a);
  start_task(ctx->mid, mid_task);
  start_task(ctx->high, high_task);

  /*
   * Here we see that the priority of the high priority task blocked on
   * semaphore B propagated to the low priority task owning semaphore A
   * on which the owner of semaphore B depends.
   */
  assert_prio(ctx->low, 1);
  assert_prio(ctx->mid, 1);
  assert_prio(ctx->high, 1);
  assert_prio(ctx->inversion, 2);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 4
#define CONFIGURE_MAXIMUM_SEMAPHORES 2

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
