/*
 * Copyright (C) 2013, 2020 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/test.h>
#include <rtems/test-info.h>

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
  volatile bool early;
  volatile bool late;
} test_context;

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
    T_quiet_true(sc == RTEMS_SUCCESSFUL || sc == RTEMS_TIMEOUT);
  }
}

static T_interrupt_test_state interrupt(void *arg)
{
  test_context *ctx = arg;
  T_interrupt_test_state state;
  rtems_status_code sc;

  state = T_interrupt_test_get_state();

  if (state != T_INTERRUPT_TEST_ACTION) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  sc = rtems_semaphore_release(ctx->semaphore_id);
  T_quiet_rsc_success(sc);

  if ( ctx->early ) {
    state = T_INTERRUPT_TEST_EARLY;
  } else if ( ctx->late ) {
    state = T_INTERRUPT_TEST_LATE;
  } else if (
    ctx->thread_queue_was_null
      && ctx->status_was_successful
      && ctx->status_was_timeout
  ) {
    state = T_INTERRUPT_TEST_DONE;
  } else {
    state = T_INTERRUPT_TEST_CONTINUE;
  }

  return state;
}

static void prepare(void *arg)
{
  test_context *ctx = arg;

  ctx->early = true;
  ctx->late = false;
}

static void action(void *arg)
{
  test_context *ctx = arg;
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  ctx->early = false;

  T_quiet_rsc_success(_Thread_Wait_get_status( ctx->semaphore_task_tcb ));

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
      T_unreachable();
      break;
  }

  ctx->late = true;
  _Thread_Dispatch_enable(cpu_self);

  T_interrupt_test_busy_wait_for_interrupt();
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE(SemaphoreObtainTimeoutInterrupt)
{
  test_context ctx;
  rtems_status_code sc;
  T_interrupt_test_state state;

  memset(&ctx, 0, sizeof(ctx));
  ctx.master_task = rtems_task_self();

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    1,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &ctx.semaphore_id
  );
  T_assert_rsc_success(sc);

  sc = rtems_task_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    PRIORITY_SEMAPHORE,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx.semaphore_task
  );
  T_assert_rsc_success(sc);

  sc = rtems_task_start(
    ctx.semaphore_task,
    semaphore_task,
    (rtems_task_argument) &ctx
  );
  T_assert_rsc_success(sc);

  state = T_interrupt_test(&config, &ctx);
  T_eq_int(state, T_INTERRUPT_TEST_DONE);

  T_true(ctx.thread_queue_was_null);
  T_true(ctx.status_was_successful);
  T_true(ctx.status_was_timeout);

  sc = rtems_task_delete(ctx.semaphore_task);
  T_rsc_success(sc);

  sc = rtems_semaphore_delete(ctx.semaphore_id);
  T_rsc_success(sc);
}

static rtems_task Init(rtems_task_argument argument)
{
  rtems_test_run(argument, TEST_STATE);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_SEMAPHORES 1
#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_MASTER
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
