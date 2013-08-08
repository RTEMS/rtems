/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>
#include <rtems/score/statesimpl.h>

#define TEST_NAME "19"

#define PRIORITY_RED 1

#define PRIORITY_GREEN 2

#define PRIORITY_RESUMER 3

typedef struct {
  rtems_id master_task;
  rtems_id resumer_task;
  Thread_Control *master_task_tcb;
  bool test_case_hit;
} test_context;

static test_context ctx_instance;

static void resumer_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  while (true) {
    rtems_status_code sc = rtems_task_resume(ctx->master_task);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void suspend_master_task(rtems_id timer, void *arg)
{
  /* The arg is NULL */
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;

  if (_States_Is_transient(ctx->master_task_tcb->current_state)) {
    ctx->test_case_hit = true;
  }

  sc = rtems_task_suspend(ctx->master_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument ignored)
{
  test_context *ctx = &ctx_instance;
  rtems_task_priority priority = PRIORITY_RED;
  int resets = 0;
  rtems_status_code sc;

  puts("\n\n*** TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***\n");

  ctx->master_task = rtems_task_self();
  ctx->master_task_tcb = _Thread_Get_executing();

  sc = rtems_task_create(
    rtems_build_name('R', 'E', 'S', 'U'),
    PRIORITY_RESUMER,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->resumer_task
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    ctx->resumer_task,
    resumer_task,
    (rtems_task_argument) ctx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  interrupt_critical_section_test_support_initialize(
    suspend_master_task
  );

  while (resets < 3 && !ctx->test_case_hit) {
    if (interrupt_critical_section_test_support_delay()) {
      ++resets;
    }

    sc = rtems_task_set_priority(RTEMS_SELF, priority, &priority);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(_States_Is_ready(ctx->master_task_tcb->current_state));
  }

  rtems_test_assert(ctx->test_case_hit);

  sc = rtems_task_delete(ctx->resumer_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  puts("*** END OF TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_GREEN
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
