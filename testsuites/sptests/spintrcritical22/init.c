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

#include <tmacros.h>
#include <intrcritical.h>
#include <rtems/rtems/semimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 22";

typedef struct {
  rtems_id semaphore_id;
  Semaphore_Control *semaphore_control;
  Thread_Control *main_task_control;
  volatile bool done;
} test_context;

static test_context ctx_instance;

static Semaphore_Control *get_semaphore_control(rtems_id id)
{
  Thread_queue_Context queue_context;
  Semaphore_Control *sem;

  sem = _Semaphore_Get(id, &queue_context);
  rtems_test_assert(sem != NULL);
  _ISR_lock_ISR_enable(&queue_context.Lock_context.Lock_context);

  return sem;
}

static void release_semaphore(rtems_id timer, void *arg)
{
  /* The arg is NULL */
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;

  if (
    _Thread_Wait_flags_get(ctx->main_task_control)
      == (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_INTEND_TO_BLOCK)
  ) {
    CORE_semaphore_Control *sem;

    ctx->done = true;

    sc = rtems_semaphore_release(ctx->semaphore_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(
      _Thread_Wait_flags_get(ctx->main_task_control)
        == (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_READY_AGAIN)
    );
    sem = &ctx->semaphore_control->Core_control.Semaphore;
    rtems_test_assert(sem->count == 0);
  } else {
    sc = rtems_semaphore_release(ctx->semaphore_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static bool test_body(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(
    ctx->semaphore_id,
    RTEMS_NO_WAIT,
    0
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL || sc == RTEMS_UNSATISFIED);

  sc = rtems_semaphore_obtain(
    ctx->semaphore_id,
    RTEMS_WAIT,
    2
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL || sc == RTEMS_TIMEOUT);

  return ctx->done;
}

static void Init(rtems_task_argument ignored)
{
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;

  TEST_BEGIN();

  ctx->main_task_control = _Thread_Get_executing();

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    1,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &ctx->semaphore_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  ctx->semaphore_control = get_semaphore_control(ctx->semaphore_id);

  interrupt_critical_section_test(test_body, ctx, release_semaphore);
  rtems_test_assert(ctx->done);

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_SEMAPHORES 1
#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
