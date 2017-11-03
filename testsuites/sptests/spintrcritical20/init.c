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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/rtems/semimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 20";

#define PRIORITY_MASTER 2

#define PRIORITY_SEMAPHORE 1

typedef struct {
  rtems_id master_task;
  rtems_id semaphore_task;
  rtems_id semaphore_id;
  Thread_Control *semaphore_task_tcb;
  bool thread_queue_was_null;
  bool status_was_successful;
  bool status_was_timeout;
} test_context;

static test_context ctx_instance;

static void semaphore_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  ctx->semaphore_task_tcb = _Thread_Get_executing();

  while (true) {
    rtems_status_code sc = rtems_semaphore_obtain(
      ctx->semaphore_id,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL || sc == RTEMS_TIMEOUT);
  }
}

static void release_semaphore(rtems_id timer, void *arg)
{
  /* The arg is NULL */
  test_context *ctx = &ctx_instance;
  rtems_status_code sc = rtems_semaphore_release(ctx->semaphore_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static bool test_body(void *arg)
{
  test_context *ctx = arg;
  int busy;
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();

  rtems_test_assert(
    _Thread_Wait_get_status( ctx->semaphore_task_tcb ) == STATUS_SUCCESSFUL
  );

  /*
   * Spend some time to make it more likely that we hit the test condition
   * below.
   */
  for (busy = 0; busy < 1000; ++busy) {
    __asm__ volatile ("");
  }

  if (ctx->semaphore_task_tcb->Wait.queue == NULL) {
    ctx->thread_queue_was_null = true;
  }

  _Thread_Timeout(&ctx->semaphore_task_tcb->Timer.Watchdog);

  switch (_Thread_Wait_get_status(ctx->semaphore_task_tcb)) {
    case STATUS_SUCCESSFUL:
      ctx->status_was_successful = true;
      break;
    case STATUS_TIMEOUT:
      ctx->status_was_timeout = true;
      break;
    default:
      rtems_test_assert(0);
      break;
  }

  _Thread_Dispatch_enable(cpu_self);

  return ctx->thread_queue_was_null
    && ctx->status_was_successful
    && ctx->status_was_timeout;
}

static void Init(rtems_task_argument ignored)
{
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;

  TEST_BEGIN();

  ctx->master_task = rtems_task_self();

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    1,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &ctx->semaphore_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    PRIORITY_SEMAPHORE,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->semaphore_task
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    ctx->semaphore_task,
    semaphore_task,
    (rtems_task_argument) ctx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  interrupt_critical_section_test(test_body, ctx, release_semaphore);

  rtems_test_assert(ctx->thread_queue_was_null);
  rtems_test_assert(ctx->status_was_successful);
  rtems_test_assert(ctx->status_was_timeout);

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_SEMAPHORES 1
#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_MASTER
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
