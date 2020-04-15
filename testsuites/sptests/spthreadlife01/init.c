/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/threadimpl.h>

#include "tmacros.h"

#define PRIO_INIT 1
#define PRIO_HIGH 2
#define PRIO_MID 3
#define PRIO_LOW 4
#define PRIO_VERY_LOW 5

const char rtems_test_name[] = "SPTHREADLIFE 1";

typedef enum {
  INIT,
  SET_PRIO,
  SET_PRIO_DONE,
  DO_OBTAIN_0,
  OBTAIN_DONE_0,
  DO_RELEASE_0,
  RELEASE_DONE_0,
  DO_OBTAIN_1,
  OBTAIN_DONE_1,
  RESTART_0,
  RESTART_1,
  RESTART_2,
  RESTART_3,
  DO_RELEASE_1,
  RELEASE_DONE_1,
  DELETE_0,
  DELETE_1,
  DELETE_2,
  DELETE_3,
  SET_PROTECTION,
  SET_PROTECTION_DONE,
  CLEAR_PROTECTION,
  DELETE_4,
  DELETE_5,
  DELETE_6,
  DELETE_SELF,
  DELETE_7,
  DELETE_8,
  DELETE_9,
  EXIT_0,
  EXIT_1,
  EXIT_2,
  EXIT_3,
  INVALID
} test_state;

typedef struct {
  rtems_id main_task_id;
  rtems_id worker_task_id;
  rtems_id sema_id;
  test_state current;
  test_state next;
} test_context;

static test_context test_instance;

static void wake_up_main(const test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_event_transient_send(ctx->main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wait(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void change_state(
  test_context *ctx,
  test_state expected,
  test_state current,
  test_state next
)
{
  rtems_test_assert(ctx->current == expected);
  ctx->current = current;
  ctx->next = next;
}

static void change_state_and_wait(
  test_context *ctx,
  test_state expected,
  test_state current,
  test_state next
)
{
  change_state(ctx, expected, current, next);
  wait();
}

static void set_priority(rtems_task_priority prio)
{
  rtems_status_code sc;

  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void assert_priority(rtems_task_priority expected)
{
  rtems_status_code sc;
  rtems_task_priority prio;

  sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(prio == expected);
}

static void restart_extension(
  Thread_Control *executing,
  Thread_Control *restarted
)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  rtems_test_assert(executing == restarted);

  switch (ctx->current) {
    case RESTART_0:
      rtems_test_assert(ctx->worker_task_id == rtems_task_self());
      ctx->current = RESTART_1;
      sc = rtems_task_restart(RTEMS_SELF, 0);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
      break;
    case RESTART_1:
      rtems_test_assert(ctx->worker_task_id == rtems_task_self());
      ctx->current = RESTART_2;
      break;
    default:
      rtems_test_assert(0);
      break;
  }
}

static void delete_extension(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  test_context *ctx = &test_instance;

  rtems_test_assert(executing != deleted);
  rtems_test_assert(ctx->main_task_id == rtems_task_self());

  switch (ctx->current) {
    case DELETE_2:
      assert_priority(PRIO_INIT);
      ctx->current = DELETE_3;
      break;
    case DELETE_5:
      assert_priority(PRIO_INIT);
      ctx->current = DELETE_6;
      break;
    case DELETE_8:
      assert_priority(PRIO_VERY_LOW);
      ctx->current = DELETE_9;
      break;
    case EXIT_2:
      assert_priority(PRIO_VERY_LOW);
      ctx->current = EXIT_3;
      break;
    default:
      rtems_test_assert(0);
      break;
  }
}

static void terminate_extension(Thread_Control *executing)
{
  test_context *ctx = &test_instance;

  rtems_test_assert(ctx->worker_task_id == rtems_task_self());

  switch (ctx->current) {
    case DELETE_0:
      assert_priority(PRIO_INIT);
      ctx->current = DELETE_1;
      rtems_task_delete(RTEMS_SELF);
      rtems_test_assert(0);
      break;
    case DELETE_1:
      assert_priority(PRIO_INIT);
      ctx->current = DELETE_2;
      break;
    case DELETE_4:
      assert_priority(PRIO_INIT);
      ctx->current = DELETE_5;
      break;
    case DELETE_7:
      assert_priority(PRIO_LOW);
      ctx->current = DELETE_8;
      break;
    case EXIT_1:
      assert_priority(PRIO_LOW);
      ctx->current = EXIT_2;
      break;
    default:
      rtems_test_assert(0);
      break;
  }
}

static void worker_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  while (true) {
    test_state state = ctx->current;
    rtems_status_code sc;
    Thread_Life_state previous_thread_life_state;
    Per_CPU_Control *cpu_self;

    switch (state) {
      case SET_PRIO:
        assert_priority(PRIO_LOW);
        set_priority(PRIO_MID);
        break;
      case DO_OBTAIN_0:
      case DO_OBTAIN_1:
        assert_priority(PRIO_MID);
        sc = rtems_semaphore_obtain(
          ctx->sema_id,
          RTEMS_WAIT,
          RTEMS_NO_TIMEOUT
        );
        rtems_test_assert(sc == RTEMS_SUCCESSFUL);
        assert_priority(PRIO_HIGH);
        break;
      case DO_RELEASE_0:
      case DO_RELEASE_1:
        assert_priority(PRIO_HIGH);
        sc = rtems_semaphore_release(ctx->sema_id);
        rtems_test_assert(sc == RTEMS_SUCCESSFUL);

        switch (state) {
          case DO_RELEASE_0:
            assert_priority(PRIO_MID);
            break;
          case DO_RELEASE_1:
            assert_priority(PRIO_LOW);
            break;
          default:
            rtems_test_assert(0);
            break;
        }

        break;
      case RESTART_2:
        assert_priority(PRIO_HIGH);
        break;
      case SET_PROTECTION:
        cpu_self = _Thread_Dispatch_disable();
        previous_thread_life_state =
          _Thread_Set_life_protection(THREAD_LIFE_PROTECTED);
        rtems_test_assert(
          (previous_thread_life_state & THREAD_LIFE_PROTECTED) == 0
        );
        _Thread_Dispatch_enable(cpu_self);
        break;
      case CLEAR_PROTECTION:
        cpu_self = _Thread_Dispatch_disable();
        previous_thread_life_state = _Thread_Set_life_protection(0);
        rtems_test_assert(
          (previous_thread_life_state & THREAD_LIFE_PROTECTED) != 0
        );
        ctx->current = DELETE_4;
        _Thread_Dispatch_enable(cpu_self);
        break;
      case DELETE_SELF:
        ctx->current = DELETE_7;
        rtems_task_delete(RTEMS_SELF);
        rtems_test_assert(0);
        break;
      case EXIT_0:
        ctx->current = EXIT_1;
        rtems_task_exit();
        rtems_test_assert(0);
        break;
      default:
        rtems_test_assert(0);
        break;
    }

    ctx->current = ctx->next;
    wake_up_main(ctx);
  }
}

static void create_sema(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    PRIO_HIGH,
    &ctx->sema_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void create_and_start_worker(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    PRIO_LOW,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_task_id, worker_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_resource_snapshot snapshot;

  ctx->main_task_id = rtems_task_self();

  rtems_resource_snapshot_take(&snapshot);

  create_sema(ctx);
  create_and_start_worker(ctx);

  change_state_and_wait(ctx, INIT, SET_PRIO, SET_PRIO_DONE);
  change_state_and_wait(ctx, SET_PRIO_DONE, DO_OBTAIN_0, OBTAIN_DONE_0);

  sc = rtems_semaphore_delete(ctx->sema_id);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

  change_state_and_wait(ctx, OBTAIN_DONE_0, DO_RELEASE_0, RELEASE_DONE_0);

  sc = rtems_semaphore_delete(ctx->sema_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  create_sema(ctx);

  change_state_and_wait(ctx, RELEASE_DONE_0, DO_OBTAIN_1, OBTAIN_DONE_1);

  sc = rtems_semaphore_delete(ctx->sema_id);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

  sc = rtems_task_restart(ctx->worker_task_id, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  change_state_and_wait(ctx, OBTAIN_DONE_1, RESTART_0, RESTART_3);
  change_state_and_wait(ctx, RESTART_3, DO_RELEASE_1, RELEASE_DONE_1);

  sc = rtems_semaphore_delete(ctx->sema_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  change_state(ctx, RELEASE_DONE_1, DELETE_0, INVALID);

  sc = rtems_task_delete(ctx->worker_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(ctx->current == DELETE_2);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  create_and_start_worker(ctx);

  change_state_and_wait(ctx, DELETE_3, SET_PROTECTION, SET_PROTECTION_DONE);
  change_state(ctx, SET_PROTECTION_DONE, CLEAR_PROTECTION, INVALID);

  sc = rtems_task_delete(ctx->worker_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  create_and_start_worker(ctx);

  change_state(ctx, DELETE_6, DELETE_SELF, INVALID);
  set_priority(PRIO_VERY_LOW);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
  set_priority(PRIO_INIT);

  create_and_start_worker(ctx);

  change_state(ctx, DELETE_9, EXIT_0, INVALID);
  set_priority(PRIO_VERY_LOW);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
  set_priority(PRIO_INIT);

  rtems_test_assert(ctx->current == EXIT_3);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS \
  { \
    .thread_restart = restart_extension, \
    .thread_delete = delete_extension, \
    .thread_terminate = terminate_extension \
  }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
