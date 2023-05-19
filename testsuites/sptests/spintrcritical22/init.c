/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2014, 2020 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems/test.h>

#include <rtems/rtems/semimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 22";

typedef struct {
  rtems_id semaphore_id;
  Semaphore_Control *semaphore_control;
  Thread_Control *main_task_control;
} test_context;

static Semaphore_Control *get_semaphore_control(rtems_id id)
{
  Thread_queue_Context queue_context;
  Semaphore_Control *sem;

  sem = _Semaphore_Get(id, &queue_context);
  rtems_test_assert(sem != NULL);
  _ISR_lock_ISR_enable(&queue_context.Lock_context.Lock_context);

  return sem;
}

static T_interrupt_test_state release_semaphore(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;
  Thread_Wait_flags flags;
  T_interrupt_test_state state;

  flags = _Thread_Wait_flags_get(ctx->main_task_control);

  if (
    flags == (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_INTEND_TO_BLOCK)
  ) {
    CORE_semaphore_Control *sem;

    state = T_INTERRUPT_TEST_DONE;

    sc = rtems_semaphore_release(ctx->semaphore_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(
      _Thread_Wait_flags_get(ctx->main_task_control)
        == THREAD_WAIT_STATE_READY
    );
    sem = &ctx->semaphore_control->Core_control.Semaphore;
    rtems_test_assert(sem->count == 0);
  } else {
    if (flags == (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_BLOCKED)) {
      state = T_INTERRUPT_TEST_LATE;
    } else {
      state = T_INTERRUPT_TEST_EARLY;
    }

    sc = rtems_semaphore_release(ctx->semaphore_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  return state;
}

static void action(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(
    ctx->semaphore_id,
    RTEMS_WAIT,
    2
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void prepare(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(
    ctx->semaphore_id,
    RTEMS_NO_WAIT,
    0
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL || sc == RTEMS_UNSATISFIED);
}

static void blocked(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  T_interrupt_test_change_state(
    T_INTERRUPT_TEST_ACTION,
    T_INTERRUPT_TEST_LATE
  );

  sc = rtems_semaphore_release(ctx->semaphore_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = release_semaphore,
  .blocked = blocked,
  .max_iteration_count = 10000
};

T_TEST_CASE(InterruptSemaphoreObtain)
{
  test_context ctx;
  rtems_status_code sc;
  T_interrupt_test_state state;

  ctx.main_task_control = _Thread_Get_executing();

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    1,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &ctx.semaphore_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  ctx.semaphore_control = get_semaphore_control(ctx.semaphore_id);

  state = T_interrupt_test(&config, &ctx);
  T_eq_int(state, T_INTERRUPT_TEST_DONE);

  sc = rtems_semaphore_delete(ctx.semaphore_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument argument)
{
  rtems_test_run(argument, TEST_STATE);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_SEMAPHORES 1
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
