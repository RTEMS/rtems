/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2013 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>

#include <rtems/rtems/eventimpl.h>

#define GREEN RTEMS_EVENT_0

#define RED RTEMS_EVENT_1

#define EVENTS (GREEN | RED)

#define DEADBEEF 0xdeadbeef

typedef struct {
  rtems_id timer;
  Thread_Control *thread;
  bool hit;
} test_context;

static void any_satisfy_before_timeout(rtems_id timer, void *arg)
{
  rtems_status_code sc;
  test_context *ctx = arg;
  const Thread_Control *thread = ctx->thread;

  if (thread->Wait.count != 0) {
    ctx->hit = _Event_Sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;

    rtems_test_assert(thread->Wait.count == EVENTS);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    sc = rtems_event_send(thread->Object.id, GREEN);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(thread->Wait.count == 0);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    sc = rtems_event_send(thread->Object.id, RED);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(thread->Wait.count == 0);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    _Event_Timeout(thread->Object.id, &_Event_Sync_state);

    rtems_test_assert(thread->Wait.count == 0);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    if (ctx->hit) {
      rtems_test_assert(
        _Event_Sync_state == THREAD_BLOCKING_OPERATION_SATISFIED
      );
    }
  }

  sc = rtems_timer_reset(timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_any_satisfy_before_timeout(test_context *ctx)
{
  rtems_status_code sc;
  int resets = 0;

  puts(
    "Init - Trying to generate any satisfied before timeout "
    "while blocking on event"
  );

  ctx->hit = false;

  interrupt_critical_section_test_support_initialize(NULL);

  sc = rtems_timer_fire_after(ctx->timer, 1, any_satisfy_before_timeout, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (!ctx->hit && resets < 2) {
    rtems_event_set out;

    if (interrupt_critical_section_test_support_delay())
      resets++;

    out = DEADBEEF;
    sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ANY | RTEMS_WAIT, 1, &out);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(out == GREEN);

    out = DEADBEEF;
    sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ANY | RTEMS_NO_WAIT, 0, &out);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(out == RED);
  }

  sc = rtems_timer_cancel(ctx->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->hit);
}

static void all_satisfy_before_timeout(rtems_id timer, void *arg)
{
  rtems_status_code sc;
  test_context *ctx = arg;
  const Thread_Control *thread = ctx->thread;

  if (thread->Wait.count != 0) {
    ctx->hit = _Event_Sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;

    rtems_test_assert(thread->Wait.count == EVENTS);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    sc = rtems_event_send(thread->Object.id, GREEN);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(thread->Wait.count == EVENTS);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    sc = rtems_event_send(thread->Object.id, RED);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(thread->Wait.count == 0);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == EVENTS
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    _Event_Timeout(thread->Object.id, &_Event_Sync_state);

    rtems_test_assert(thread->Wait.count == 0);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == EVENTS
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    if (ctx->hit) {
      rtems_test_assert(
        _Event_Sync_state == THREAD_BLOCKING_OPERATION_SATISFIED
      );
    }
  }

  sc = rtems_timer_reset(timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_all_satisfy_before_timeout(test_context *ctx)
{
  rtems_status_code sc;
  int resets = 0;

  puts(
    "Init - Trying to generate all satisfied before timeout "
    "while blocking on event"
  );

  ctx->hit = false;

  interrupt_critical_section_test_support_initialize(NULL);

  sc = rtems_timer_fire_after(ctx->timer, 1, all_satisfy_before_timeout, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (!ctx->hit && resets < 2) {
    rtems_event_set out;

    if (interrupt_critical_section_test_support_delay())
      resets++;

    out = DEADBEEF;
    sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ALL | RTEMS_WAIT, 1, &out);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(out == EVENTS);
  }

  sc = rtems_timer_cancel(ctx->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->hit);
}

static void timeout_before_satisfied(rtems_id timer, void *arg)
{
  rtems_status_code sc;
  test_context *ctx = arg;
  const Thread_Control *thread = ctx->thread;

  if (thread->Wait.count != 0) {
    ctx->hit =
      _Event_Sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;

    rtems_test_assert(thread->Wait.count == EVENTS);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_SUCCESSFUL);

    _Event_Timeout(thread->Object.id, &_Event_Sync_state);

    rtems_test_assert(thread->Wait.count == 0);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_TIMEOUT);

    sc = rtems_event_send(thread->Object.id, EVENTS);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rtems_test_assert(thread->Wait.count == 0);
    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(thread->Wait.return_code == RTEMS_TIMEOUT);

    if (ctx->hit) {
      rtems_test_assert(
        _Event_Sync_state == THREAD_BLOCKING_OPERATION_TIMEOUT
      );
    }
  }

  sc = rtems_timer_reset(timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_timeout_before_all_satisfy(test_context *ctx)
{
  rtems_status_code sc;
  int resets = 0;

  puts(
    "Init - Trying to generate timeout before all satisfied "
    "while blocking on event"
  );

  ctx->hit = false;

  interrupt_critical_section_test_support_initialize(NULL);

  sc = rtems_timer_fire_after(ctx->timer, 1, timeout_before_satisfied, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (!ctx->hit && resets < 2) {
    rtems_event_set out;

    if (interrupt_critical_section_test_support_delay())
      resets++;

    out = DEADBEEF;
    sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ALL | RTEMS_WAIT, 1, &out);
    rtems_test_assert(sc == RTEMS_TIMEOUT);
    rtems_test_assert(out == DEADBEEF);

    out = DEADBEEF;
    sc = rtems_event_receive(EVENTS, RTEMS_EVENT_ALL | RTEMS_NO_WAIT, 0, &out);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(out == EVENTS);
  }

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

  puts( "\n\n*** TEST INTERRUPT CRITICAL SECTION 10 ***" );

  sc = rtems_timer_create(rtems_build_name('T', 'I', 'M', 'R'), &ctx.timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_any_satisfy_before_timeout(&ctx);
  test_all_satisfy_before_timeout(&ctx);
  test_timeout_before_all_satisfy(&ctx);

  puts( "*** END OF TEST INTERRUPT CRITICAL SECTION 10 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
