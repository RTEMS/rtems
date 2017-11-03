/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2013 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>

#include <rtems/score/threadimpl.h>
#include <rtems/rtems/eventimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 10";

#define GREEN RTEMS_EVENT_0

#define RED RTEMS_EVENT_1

#define EVENTS (GREEN | RED)

#define DEADBEEF 0xdeadbeef

typedef struct {
  rtems_id timer;
  Thread_Control *thread;
  bool hit;
} test_context;

static bool blocks_for_event(Thread_Wait_flags flags)
{
  return flags == (THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_INTEND_TO_BLOCK)
    || flags == (THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_BLOCKED);
}

static bool interrupts_blocking_op(Thread_Wait_flags flags)
{
  return
    flags == (THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_INTEND_TO_BLOCK);
}

static void any_satisfy_before_timeout(rtems_id timer, void *arg)
{
  rtems_status_code sc;
  test_context *ctx = arg;
  Thread_Control *thread = ctx->thread;
  Thread_Wait_flags flags = _Thread_Wait_flags_get(thread);

  if (blocks_for_event(flags)) {
    ctx->hit = interrupts_blocking_op(flags);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    sc = rtems_event_send(thread->Object.id, GREEN);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    sc = rtems_event_send(thread->Object.id, RED);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    _Thread_Timeout(&thread->Timer.Watchdog);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    if (ctx->hit) {
      rtems_test_assert(
        _Thread_Wait_flags_get(thread)
          == (THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_READY_AGAIN)
      );
    }

    rtems_test_assert(thread->Wait.count == EVENTS);
  }

  sc = rtems_timer_reset(timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static bool test_body_any_satisfy_before_timeout(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;
  rtems_event_set out;

  out = DEADBEEF;
  sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ANY | RTEMS_WAIT, 1, &out);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(out == GREEN);

  out = DEADBEEF;
  sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ANY | RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(out == RED);

  return ctx->hit;
}

static void test_any_satisfy_before_timeout(test_context *ctx)
{
  rtems_status_code sc;

  puts(
    "Init - Trying to generate any satisfied before timeout "
    "while blocking on event"
  );

  ctx->hit = false;

  sc = rtems_timer_fire_after(ctx->timer, 1, any_satisfy_before_timeout, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  interrupt_critical_section_test(
    test_body_any_satisfy_before_timeout,
    ctx,
    NULL
  );

  sc = rtems_timer_cancel(ctx->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->hit);
}

static void all_satisfy_before_timeout(rtems_id timer, void *arg)
{
  rtems_status_code sc;
  test_context *ctx = arg;
  Thread_Control *thread = ctx->thread;
  Thread_Wait_flags flags = _Thread_Wait_flags_get(thread);

  if (blocks_for_event(flags)) {
    ctx->hit = interrupts_blocking_op(flags);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    sc = rtems_event_send(thread->Object.id, GREEN);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    sc = rtems_event_send(thread->Object.id, RED);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == EVENTS
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    _Thread_Timeout(&thread->Timer.Watchdog);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == EVENTS
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    if (ctx->hit) {
      rtems_test_assert(
        _Thread_Wait_flags_get(thread)
          == (THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_READY_AGAIN)
      );
    }

    rtems_test_assert(thread->Wait.count == EVENTS);
  }

  sc = rtems_timer_reset(timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static bool test_body_all_satisfy_before_timeout(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;
  rtems_event_set out;

  out = DEADBEEF;
  sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ALL | RTEMS_WAIT, 1, &out);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(out == EVENTS);

  return ctx->hit;
}

static void test_all_satisfy_before_timeout(test_context *ctx)
{
  rtems_status_code sc;

  puts(
    "Init - Trying to generate all satisfied before timeout "
    "while blocking on event"
  );

  ctx->hit = false;

  sc = rtems_timer_fire_after(ctx->timer, 1, all_satisfy_before_timeout, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  interrupt_critical_section_test(
    test_body_all_satisfy_before_timeout,
    ctx,
    NULL
  );

  sc = rtems_timer_cancel(ctx->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->hit);
}

static void timeout_before_satisfied(rtems_id timer, void *arg)
{
  rtems_status_code sc;
  test_context *ctx = arg;
  Thread_Control *thread = ctx->thread;
  Thread_Wait_flags flags = _Thread_Wait_flags_get(thread);

  if (blocks_for_event(flags)) {
    ctx->hit = interrupts_blocking_op(flags);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_SUCCESSFUL);

    _Thread_Timeout(&thread->Timer.Watchdog);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_TIMEOUT);

    sc = rtems_event_send(thread->Object.id, EVENTS);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(_Thread_Wait_get_status(thread) == STATUS_TIMEOUT);

    if (ctx->hit) {
      rtems_test_assert(
        _Thread_Wait_flags_get(thread)
          == (THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_READY_AGAIN)
      );
    }

    rtems_test_assert(thread->Wait.count == EVENTS);
  }

  sc = rtems_timer_reset(timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static bool test_body_timeout_before_all_satisfy(void *arg)
{
  test_context *ctx = arg;
  rtems_event_set out;
  rtems_status_code sc;

  out = DEADBEEF;
  sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ALL | RTEMS_WAIT, 1, &out);
  rtems_test_assert(sc == RTEMS_TIMEOUT);
  rtems_test_assert(out == DEADBEEF);

  out = DEADBEEF;
  sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ALL | RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(out == EVENTS);

  return ctx->hit;
}

static void test_timeout_before_all_satisfy(test_context *ctx)
{
  rtems_status_code sc;

  puts(
    "Init - Trying to generate timeout before all satisfied "
    "while blocking on event"
  );

  ctx->hit = false;

  sc = rtems_timer_fire_after(ctx->timer, 1, timeout_before_satisfied, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  interrupt_critical_section_test(
    test_body_timeout_before_all_satisfy,
    ctx,
    NULL
  );

  sc = rtems_timer_cancel(ctx->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->hit);
}

static rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code sc;
  test_context ctx = {
    .thread = _Thread_Get_executing()
  };

  TEST_BEGIN();

  sc = rtems_timer_create(rtems_build_name('T', 'I', 'M', 'R'), &ctx.timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_any_satisfy_before_timeout(&ctx);
  test_all_satisfy_before_timeout(&ctx);
  test_timeout_before_all_satisfy(&ctx);

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
