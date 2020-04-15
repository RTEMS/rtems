/*
 * Copyright (c) 2015, 2016 embedded brains GmbH.  All rights reserved.
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

const char rtems_test_name[] = "SMPMUTEX 1";

#define SCHED_A rtems_build_name(' ', ' ', ' ', 'A')

#define SCHED_B rtems_build_name(' ', ' ', ' ', 'B')

#define PART_COUNT 2

#define TASK_COUNT 9

#define PRIO_NONE 0

/* Value chosen for Qemu, 2 would be sufficient for real targets */
#define TIMEOUT_IN_TICKS 10

typedef enum {
  REQ_WAKE_UP_MASTER = RTEMS_EVENT_0,
  REQ_WAKE_UP_HELPER = RTEMS_EVENT_1,
  REQ_MTX_OBTAIN = RTEMS_EVENT_2,
  REQ_MTX_OBTAIN_TIMEOUT = RTEMS_EVENT_3,
  REQ_MTX_OBTAIN_UNSATISFIED = RTEMS_EVENT_4,
  REQ_MTX_RELEASE = RTEMS_EVENT_5,
  REQ_MTX_2_OBTAIN = RTEMS_EVENT_6,
  REQ_MTX_2_RELEASE = RTEMS_EVENT_7,
  REQ_SEM_OBTAIN_RELEASE = RTEMS_EVENT_8,
  REQ_SEM_RELEASE = RTEMS_EVENT_9,
  REQ_SET_DONE = RTEMS_EVENT_10,
  REQ_WAIT_FOR_DONE = RTEMS_EVENT_11,
  REQ_SEND_EVENT_2 = RTEMS_EVENT_12,
  REQ_SEND_EVENT_3 = RTEMS_EVENT_13,
  REQ_CEIL_OBTAIN = RTEMS_EVENT_14,
  REQ_CEIL_RELEASE = RTEMS_EVENT_15
} request_id;

typedef enum {
  A_1,
  A_2_0,
  A_2_1,
  M,
  B_4,
  B_5_0,
  B_5_1,
  H_A,
  H_B,
  NONE
} task_id;

typedef struct {
  rtems_id mtx;
  rtems_id mtx_2;
  rtems_id sem;
  rtems_id ceil;
  rtems_id tasks[TASK_COUNT];
  Atomic_Uint done;
  task_id id_2;
  rtems_event_set events_2;
  task_id id_3;
  rtems_event_set events_3;
  int generation[TASK_COUNT];
  int expected_generation[TASK_COUNT];
} test_context;

static test_context test_instance;

static void assert_cpu(uint32_t expected_cpu)
{
  rtems_test_assert(rtems_scheduler_get_processor() == expected_cpu);
}

static void test_task_get_priority_not_defined(test_context *ctx)
{
  rtems_status_code sc;
  rtems_id scheduler_id;
  rtems_task_priority priority;

  sc = rtems_scheduler_ident(SCHED_B, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  priority = 0;
  sc = rtems_task_get_priority(RTEMS_SELF, scheduler_id, &priority);
  rtems_test_assert(sc == RTEMS_NOT_DEFINED);
  rtems_test_assert(priority == 0);
}

static void start_task(
  test_context *ctx,
  task_id id,
  rtems_task_entry entry,
  rtems_task_priority prio,
  rtems_name scheduler
)
{
  rtems_status_code sc;
  rtems_id scheduler_id;

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    prio,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->tasks[id]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_scheduler_ident(scheduler, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(ctx->tasks[id], scheduler_id, prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->tasks[id], entry, id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void send_event(test_context *ctx, task_id id, rtems_event_set events)
{
  rtems_status_code sc;

  sc = rtems_event_send(ctx->tasks[id], events);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void set_event_2(
  test_context *ctx,
  task_id id_2,
  rtems_event_set events_2
)
{
  ctx->id_2 = id_2;
  ctx->events_2 = events_2;
}

static void set_event_3(
  test_context *ctx,
  task_id id_3,
  rtems_event_set events_3
)
{
  ctx->id_3 = id_3;
  ctx->events_3 = events_3;
}

static void clear_done(test_context *ctx)
{
  _Atomic_Store_uint(&ctx->done, 0, ATOMIC_ORDER_RELAXED);
}

static void set_done(test_context *ctx)
{
  _Atomic_Store_uint(&ctx->done, 1, ATOMIC_ORDER_RELEASE);
}

static bool is_done(test_context *ctx)
{
  return _Atomic_Load_uint(&ctx->done, ATOMIC_ORDER_ACQUIRE) != 0;
}

static void wait_for_done(test_context *ctx)
{
  while (!is_done(ctx)) {
    /* Wait */
  }
}

static void request_pre_emption(test_context *ctx, task_id id)
{
  clear_done(ctx);
  send_event(ctx, id, REQ_SET_DONE);
  wait_for_done(ctx);
}

static rtems_event_set wait_for_events(void)
{
  rtems_event_set events;
  rtems_status_code sc;

  sc = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return events;
}

static void sync_with_helper_by_id(test_context *ctx, task_id id)
{
  rtems_event_set events;

  send_event(ctx, id, REQ_WAKE_UP_HELPER);
  events = wait_for_events();
  rtems_test_assert(events == REQ_WAKE_UP_MASTER);
}

static void sync_with_helper(test_context *ctx)
{
  sync_with_helper_by_id(ctx, H_A);
  sync_with_helper_by_id(ctx, H_B);
}

static void request(test_context *ctx, task_id id, request_id req)
{
  send_event(ctx, id, req);
  clear_done(ctx);

  if (rtems_scheduler_get_processor() == 0) {
    id = H_B;
  } else {
    id = H_A;
  }

  send_event(ctx, id, REQ_SET_DONE);
  wait_for_done(ctx);
}

static void obtain(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mtx, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void obtain_timeout(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mtx, RTEMS_WAIT, TIMEOUT_IN_TICKS);
  rtems_test_assert(sc == RTEMS_TIMEOUT);
}

static void obtain_unsatisfied(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mtx, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void release(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_release(ctx->mtx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void flush(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_flush(ctx->mtx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void obtain_2(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mtx_2, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void release_2(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_release(ctx->mtx_2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void sem_obtain(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void sem_release(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_release(ctx->sem);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void ceil_obtain(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->ceil, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void ceil_release(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_release(ctx->ceil);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wait(void)
{
  rtems_status_code sc;

  sc = rtems_task_wake_after(TIMEOUT_IN_TICKS + 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void check_generations(test_context *ctx, task_id a, task_id b)
{
  size_t i;

  if (a != NONE) {
    ++ctx->expected_generation[a];
  }

  if (b != NONE) {
    ++ctx->expected_generation[b];
  }

  for (i = 0; i < TASK_COUNT; ++i) {
    rtems_test_assert(ctx->generation[i] == ctx->expected_generation[i]);
  }
}

static void yield(void)
{
  rtems_status_code sc;

  sc = rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void set_prio(test_context *ctx, task_id id, rtems_task_priority prio)
{
  rtems_status_code sc;

  sc = rtems_task_set_priority(ctx->tasks[id], prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void assert_prio(
  test_context *ctx,
  task_id id,
  rtems_task_priority expected
)
{
  rtems_task_priority actual;
  rtems_status_code sc;

  sc = rtems_task_set_priority(
    ctx->tasks[id],
    RTEMS_CURRENT_PRIORITY,
    &actual
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(expected == actual);
}

static void assert_prio_by_scheduler(
  test_context *ctx,
  task_id id,
  rtems_name scheduler,
  rtems_task_priority expected
)
{
  rtems_task_priority actual;
  rtems_status_code sc;
  rtems_id scheduler_id;

  sc = rtems_scheduler_ident(scheduler, &scheduler_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  actual = PRIO_NONE;
  sc = rtems_task_get_priority(
    ctx->tasks[id],
    scheduler_id,
    &actual
  );

  if (expected == PRIO_NONE) {
    rtems_test_assert(sc == RTEMS_NOT_DEFINED);
  } else {
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  rtems_test_assert(actual == expected);
}

static void helper(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  while (true) {
    rtems_event_set events = wait_for_events();

    if ((events & REQ_WAKE_UP_HELPER) != 0) {
      send_event(ctx, M, REQ_WAKE_UP_MASTER);
    }

    if ((events & REQ_SEM_RELEASE) != 0) {
      sem_release(ctx);
    }

    if ((events & REQ_SET_DONE) != 0) {
      set_done(ctx);
    }
  }
}

static void worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  task_id id = arg;

  while (true) {
    rtems_event_set events = wait_for_events();

    if ((events & REQ_MTX_OBTAIN) != 0) {
      obtain(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_OBTAIN_TIMEOUT) != 0) {
      obtain_timeout(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_OBTAIN_UNSATISFIED) != 0) {
      obtain_unsatisfied(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_RELEASE) != 0) {
      release(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_2_OBTAIN) != 0) {
      obtain_2(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_2_RELEASE) != 0) {
      release_2(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_SEM_OBTAIN_RELEASE) != 0) {
      sem_obtain(ctx);
      ++ctx->generation[id];
      sem_release(ctx);
    }

    if ((events & REQ_SEND_EVENT_2) != 0) {
      send_event(ctx, ctx->id_2, ctx->events_2);
    }

    if ((events & REQ_SEND_EVENT_3) != 0) {
      send_event(ctx, ctx->id_3, ctx->events_3);
    }

    if ((events & REQ_CEIL_OBTAIN) != 0) {
      ceil_obtain(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_CEIL_RELEASE) != 0) {
      ceil_release(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_SET_DONE) != 0) {
      set_done(ctx);
    }

    if ((events & REQ_WAIT_FOR_DONE) != 0) {
      wait_for_done(ctx);
    }
  }
}

static void test_init(test_context *ctx)
{
  rtems_status_code sc;

  ctx->tasks[M] = rtems_task_self();
  start_task(ctx, A_1, worker, 1, SCHED_A);
  start_task(ctx, A_2_0, worker, 2, SCHED_A);
  start_task(ctx, A_2_1, worker, 2, SCHED_A);
  start_task(ctx, H_A, helper, 3, SCHED_A);

  if (rtems_scheduler_get_processor_maximum() >= PART_COUNT) {
    start_task(ctx, B_4, worker, 4, SCHED_B);
    start_task(ctx, B_5_0, worker, 5, SCHED_B);
    start_task(ctx, B_5_1, worker, 5, SCHED_B);
    start_task(ctx, H_B, helper, 6, SCHED_B);
  }

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'T', 'X', '1'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &ctx->mtx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'T', 'X', '2'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &ctx->mtx_2
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name(' ', 'S', 'E', 'M'),
    0,
    RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY,
    0,
    &ctx->sem
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('C', 'E', 'I', 'L'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    1,
    &ctx->ceil
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_simple_inheritance(test_context *ctx)
{
  obtain(ctx);
  request(ctx, A_1, REQ_MTX_OBTAIN);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 1);
  release(ctx);
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  request(ctx, A_1, REQ_MTX_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_flush_inheritance(test_context *ctx)
{
  assert_prio(ctx, M, 3);
  obtain(ctx);
  send_event(ctx, A_1, REQ_MTX_OBTAIN_UNSATISFIED);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 1);
  flush(ctx);
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  release(ctx);
}

static void test_ceiling_mutex(test_context *ctx)
{
  assert_prio(ctx, M, 3);
  ceil_obtain(ctx);
  assert_prio(ctx, M, 1);
  send_event(ctx, A_1, REQ_CEIL_OBTAIN);
  yield();
  check_generations(ctx, NONE, NONE);
  ceil_release(ctx);
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  send_event(ctx, A_1, REQ_CEIL_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_dequeue_order_one_scheduler_instance(test_context *ctx)
{
  obtain(ctx);
  request(ctx, A_2_0, REQ_MTX_OBTAIN);
  request(ctx, A_1, REQ_MTX_OBTAIN);
  request(ctx, A_2_1, REQ_MTX_OBTAIN);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 1);
  release(ctx);
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  assert_prio(ctx, A_1, 1);
  request(ctx, A_1, REQ_MTX_RELEASE);
  check_generations(ctx, A_1, A_2_0);
  request(ctx, A_2_0, REQ_MTX_RELEASE);
  check_generations(ctx, A_2_0, A_2_1);
  request(ctx, A_2_1, REQ_MTX_RELEASE);
  check_generations(ctx, A_2_1, NONE);
}

static void test_mixed_queue_two_scheduler_instances(test_context *ctx)
{
  obtain(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);

  request(ctx, B_4, REQ_MTX_OBTAIN_TIMEOUT);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);
  check_generations(ctx, NONE, NONE);
  wait();
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  check_generations(ctx, B_4, NONE);

  request(ctx, B_4, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);
  check_generations(ctx, NONE, NONE);

  request(ctx, B_5_0, REQ_SEM_OBTAIN_RELEASE);
  send_event(ctx, H_A, REQ_SEM_RELEASE);
  check_generations(ctx, NONE, NONE);

  /*
   * We are in scheduler instance A.  Task B_5_0 of scheduler instance B issued
   * the counting semaphore obtain before us.  However, we inherited the
   * priority of B_4, so we get the semaphore before B_5_0 (priority order
   * within scheduler instance B).
   */
  sem_obtain(ctx);
  check_generations(ctx, NONE, NONE);

  release(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  sync_with_helper(ctx);
  check_generations(ctx, B_4, NONE);

  request(ctx, B_4, REQ_MTX_RELEASE);
  check_generations(ctx, B_4, NONE);

  sem_release(ctx);
  sync_with_helper(ctx);
  check_generations(ctx, B_5_0, NONE);

  sem_obtain(ctx);
}

static void test_mixed_queue_two_scheduler_instances_sem_only(test_context *ctx)
{
  request(ctx, B_5_0, REQ_SEM_OBTAIN_RELEASE);
  send_event(ctx, H_A, REQ_SEM_RELEASE);
  check_generations(ctx, NONE, NONE);

  /*
   * We are in scheduler instance A.  Task B_5_0 of scheduler instance B issued
   * the counting semaphore obtain before us.  No priority inheritance is
   * involved, so task B_5_0 gets the counting semaphore first.
   */
  sem_obtain(ctx);
  check_generations(ctx, B_5_0, NONE);

  sem_release(ctx);
}

static void test_two_scheduler_instances_sem_with_inheritance(test_context *ctx)
{
  sem_obtain(ctx);

  request(ctx, B_4, REQ_MTX_OBTAIN);
  check_generations(ctx, B_4, NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);

  request(ctx, B_4, REQ_SEM_OBTAIN_RELEASE);
  check_generations(ctx, NONE, NONE);

  request(ctx, A_1, REQ_MTX_OBTAIN);
  check_generations(ctx, NONE, NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, 1);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);

  sem_release(ctx);
  sync_with_helper(ctx);
  check_generations(ctx, B_4, NONE);

  request(ctx, B_4, REQ_MTX_RELEASE);
  check_generations(ctx, B_4, A_1);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);

  request(ctx, A_1, REQ_MTX_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_two_scheduler_instances_sem_with_inheritance_timeout(test_context *ctx)
{
  sem_obtain(ctx);

  request(ctx, B_4, REQ_MTX_OBTAIN);
  check_generations(ctx, B_4, NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);

  request(ctx, B_4, REQ_SEM_OBTAIN_RELEASE);
  check_generations(ctx, NONE, NONE);

  request(ctx, A_1, REQ_MTX_OBTAIN_TIMEOUT);
  check_generations(ctx, NONE, NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, 1);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);
  wait();
  check_generations(ctx, A_1, NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);

  sem_release(ctx);
  sync_with_helper(ctx);
  check_generations(ctx, B_4, NONE);

  request(ctx, B_4, REQ_MTX_RELEASE);
  check_generations(ctx, B_4, NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);
}

static void test_simple_inheritance_two_scheduler_instances(test_context *ctx)
{
  obtain(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);

  request(ctx, B_5_0, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);

  request(ctx, B_4, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);

  request(ctx, B_5_1, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);
  check_generations(ctx, NONE, NONE);

  release(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  sync_with_helper(ctx);
  check_generations(ctx, B_4, NONE);

  request(ctx, B_4, REQ_MTX_RELEASE);
  check_generations(ctx, B_4, B_5_0);

  request(ctx, B_5_0, REQ_MTX_RELEASE);
  check_generations(ctx, B_5_0, B_5_1);

  request(ctx, B_5_1, REQ_MTX_RELEASE);
  check_generations(ctx, B_5_1, NONE);
}

static void test_nested_inheritance_two_scheduler_instances(test_context *ctx)
{
  obtain_2(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);

  request(ctx, B_5_0, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);
  check_generations(ctx, B_5_0, NONE);

  request(ctx, B_5_0, REQ_MTX_2_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);

  request(ctx, B_4, REQ_MTX_OBTAIN_TIMEOUT);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 4);
  wait();
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);
  check_generations(ctx, B_4, NONE);

  request(ctx, B_4, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 4);

  request(ctx, B_5_1, REQ_MTX_2_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 4);
  check_generations(ctx, NONE, NONE);

  release_2(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 4);
  sync_with_helper(ctx);
  check_generations(ctx, B_5_0, NONE);

  request(ctx, B_5_0, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);
  check_generations(ctx, B_4, B_5_0);

  request(ctx, B_4, REQ_MTX_RELEASE);
  check_generations(ctx, B_4, NONE);

  request(ctx, B_5_0, REQ_MTX_2_RELEASE);
  check_generations(ctx, B_5_0, B_5_1);

  request(ctx, B_5_1, REQ_MTX_2_RELEASE);
  check_generations(ctx, B_5_1, NONE);
}

static void test_dequeue_order_two_scheduler_instances(test_context *ctx)
{
  obtain(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);

  request(ctx, A_2_0, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  check_generations(ctx, NONE, NONE);

  request(ctx, B_5_0, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);
  check_generations(ctx, NONE, NONE);

  request(ctx, B_5_1, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);

  request(ctx, B_4, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);

  request(ctx, A_2_1, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);

  request(ctx, A_1, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 1);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 4);
  check_generations(ctx, NONE, NONE);

  release(ctx);
  sync_with_helper(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  assert_prio_by_scheduler(ctx, A_1, SCHED_A, 1);
  assert_prio_by_scheduler(ctx, A_1, SCHED_B, 4);
  check_generations(ctx, A_1, NONE);

  request(ctx, A_1, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, A_1, SCHED_A, 1);
  assert_prio_by_scheduler(ctx, A_1, SCHED_B, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);
  check_generations(ctx, A_1, B_4);

  request(ctx, B_4, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_4, SCHED_B, 4);
  assert_prio_by_scheduler(ctx, A_2_0, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, A_2_0, SCHED_B, 5);
  check_generations(ctx, B_4, A_2_0);

  request(ctx, A_2_0, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, A_2_0, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, A_2_0, SCHED_B, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);
  check_generations(ctx, A_2_0, B_5_0);

  request(ctx, B_5_0, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);
  assert_prio_by_scheduler(ctx, A_2_1, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, A_2_1, SCHED_B, 5);
  check_generations(ctx, B_5_0, A_2_1);

  request(ctx, A_2_1, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, A_2_1, SCHED_A, 2);
  assert_prio_by_scheduler(ctx, A_2_1, SCHED_B, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_1, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_1, SCHED_B, 5);
  check_generations(ctx, A_2_1, B_5_1);

  request(ctx, B_5_1, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, B_5_1, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_1, SCHED_B, 5);
  check_generations(ctx, B_5_1, NONE);
}

static void test_omip_pre_emption(test_context *ctx)
{
  assert_cpu(0);
  obtain(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);

  request(ctx, B_5_0, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);
  check_generations(ctx, NONE, NONE);

  request_pre_emption(ctx, A_1);
  assert_cpu(1);

  request_pre_emption(ctx, B_4);
  assert_cpu(0);

  request_pre_emption(ctx, A_1);
  assert_cpu(1);

  release(ctx);
  assert_cpu(0);
  sync_with_helper(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  check_generations(ctx, B_5_0, NONE);

  request(ctx, B_5_0, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);
  check_generations(ctx, B_5_0, NONE);
}

static void test_omip_rescue(test_context *ctx)
{
  assert_cpu(0);
  obtain(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);

  clear_done(ctx);
  set_event_3(ctx, H_B, REQ_SET_DONE);
  set_event_2(ctx, B_5_0, REQ_SEND_EVENT_3 | REQ_MTX_OBTAIN);
  send_event(ctx, A_1, REQ_SEND_EVENT_2 | REQ_WAIT_FOR_DONE);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);
  assert_cpu(1);

  release(ctx);
  assert_cpu(0);
  sync_with_helper(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  check_generations(ctx, B_5_0, NONE);

  request(ctx, B_5_0, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);
  check_generations(ctx, B_5_0, NONE);
}

static void test_omip_timeout(test_context *ctx)
{
  assert_cpu(0);
  obtain(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);

  clear_done(ctx);
  set_event_3(ctx, H_B, REQ_SET_DONE);
  set_event_2(ctx, B_5_0, REQ_SEND_EVENT_3 | REQ_MTX_OBTAIN_TIMEOUT);
  send_event(ctx, A_1, REQ_SEND_EVENT_2 | REQ_WAIT_FOR_DONE);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);
  assert_cpu(1);

  wait();
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  check_generations(ctx, B_5_0, NONE);
  assert_cpu(0);

  release(ctx);
}

static void test_omip_yield(test_context *ctx)
{
  assert_cpu(0);
  obtain(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);

  request(ctx, B_5_0, REQ_MTX_OBTAIN);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, 5);
  check_generations(ctx, NONE, NONE);

  clear_done(ctx);
  send_event(ctx, H_A, REQ_SET_DONE);
  yield();
  assert_cpu(1);
  wait_for_done(ctx);

  clear_done(ctx);
  send_event(ctx, H_B, REQ_SET_DONE);
  set_prio(ctx, H_B, 5);
  yield();
  assert_cpu(1);
  rtems_test_assert(!is_done(ctx));

  set_prio(ctx, H_B, 4);
  assert_cpu(0);

  wait_for_done(ctx);
  set_prio(ctx, H_B, 6);

  release(ctx);
  sync_with_helper(ctx);
  assert_prio_by_scheduler(ctx, M, SCHED_A, 3);
  assert_prio_by_scheduler(ctx, M, SCHED_B, PRIO_NONE);
  check_generations(ctx, B_5_0, NONE);

  request(ctx, B_5_0, REQ_MTX_RELEASE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_A, PRIO_NONE);
  assert_prio_by_scheduler(ctx, B_5_0, SCHED_B, 5);
  check_generations(ctx, B_5_0, NONE);
}

static void test(test_context *ctx)
{
  test_init(ctx);

  if (rtems_scheduler_get_processor_maximum() >= PART_COUNT) {
    test_task_get_priority_not_defined(ctx);
    test_simple_inheritance(ctx);
    test_dequeue_order_one_scheduler_instance(ctx);
    test_mixed_queue_two_scheduler_instances(ctx);
    test_mixed_queue_two_scheduler_instances_sem_only(ctx);
    test_two_scheduler_instances_sem_with_inheritance(ctx);
    test_two_scheduler_instances_sem_with_inheritance_timeout(ctx);
    test_simple_inheritance_two_scheduler_instances(ctx);
    test_nested_inheritance_two_scheduler_instances(ctx);
    test_dequeue_order_two_scheduler_instances(ctx);
    test_omip_pre_emption(ctx);
    test_omip_rescue(ctx);
    test_omip_timeout(ctx);
    test_omip_yield(ctx);
  }

  test_flush_inheritance(ctx);
  test_ceiling_mutex(ctx);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();
  test(&test_instance);
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS PART_COUNT

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_SIMPLE_SMP(a);

RTEMS_SCHEDULER_SIMPLE_SMP(b);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(a, SCHED_A), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(b, SCHED_B)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_MAXIMUM_TASKS TASK_COUNT

#define CONFIGURE_MAXIMUM_SEMAPHORES 4

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY 3

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
