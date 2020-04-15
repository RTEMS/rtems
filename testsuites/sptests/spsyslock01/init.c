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

#include <sys/lock.h>
#include <errno.h>
#include <limits.h>
#include <setjmp.h>
#include <string.h>
#include <time.h>

const char rtems_test_name[] = "SPSYSLOCK 1";

#define US_PER_TICK 10000

#define EVENT_MTX_ACQUIRE RTEMS_EVENT_0

#define EVENT_MTX_RELEASE RTEMS_EVENT_1

#define EVENT_MTX_PRIO_INV RTEMS_EVENT_2

#define EVENT_REC_MTX_ACQUIRE RTEMS_EVENT_4

#define EVENT_REC_MTX_RELEASE RTEMS_EVENT_5

#define EVENT_REC_MTX_PRIO_INV RTEMS_EVENT_6

#define EVENT_SEM_WAIT RTEMS_EVENT_7

#define EVENT_FUTEX_WAIT RTEMS_EVENT_8

#define EVENT_CONDITION_WAIT RTEMS_EVENT_9

#define EVENT_CONDITION_WAIT_REC RTEMS_EVENT_10

typedef struct {
  rtems_id high[2];
  rtems_id mid;
  rtems_id low;
  struct _Mutex_Control mtx;
  struct _Mutex_recursive_Control rec_mtx;
  struct _Condition_Control cond;
  struct _Semaphore_Control sem;
  struct _Futex_Control futex;
  int val;
  int eno[2];
  int generation[2];
  int current_generation[2];
  jmp_buf deadlock_return_context;
} test_context;

static test_context test_instance;

static int generation(test_context *ctx, size_t idx)
{
  return ++ctx->current_generation[idx];
}

static void send_event(test_context *ctx, size_t idx, rtems_event_set events)
{
  rtems_status_code sc;

  sc = rtems_event_send(ctx->high[idx], events);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void get_abs_timeout(struct timespec *to)
{
  int rv;

  rv = clock_gettime(CLOCK_REALTIME, to);
  rtems_test_assert(rv == 0);

  to->tv_nsec += 2 * US_PER_TICK * 1000;
  if (to->tv_nsec >= 1000000000) {
    ++to->tv_sec;
    to->tv_nsec -= 1000000000;
  }
}

static bool eq_tq(
  const struct _Thread_queue_Queue *a,
  const struct _Thread_queue_Queue *b
)
{
  return a->_Lock._next_ticket == b->_Lock._next_ticket
    && a->_Lock._now_serving == b->_Lock._now_serving
    && a->_heads == b->_heads
    && a->_owner == b->_owner
    && a->_name == b->_name;
}

static bool eq_mtx(
  const struct _Mutex_Control *a,
  const struct _Mutex_Control *b
)
{
  return eq_tq(&a->_Queue, &b->_Queue);
}

static bool eq_rec_mtx(
  const struct _Mutex_recursive_Control *a,
  const struct _Mutex_recursive_Control *b
)
{
  return eq_mtx(&a->_Mutex, &b->_Mutex)
    && a->_nest_level == b->_nest_level;
}

static bool eq_cond(
  const struct _Condition_Control *a,
  const struct _Condition_Control *b
)
{
  return eq_tq(&a->_Queue, &b->_Queue);
}

static bool eq_sem(
  const struct _Semaphore_Control *a,
  const struct _Semaphore_Control *b
)
{
  return eq_tq(&a->_Queue, &b->_Queue)
    && a->_count == b->_count;
}

static bool eq_futex(
  const struct _Futex_Control *a,
  const struct _Futex_Control *b
)
{
  return eq_tq(&a->_Queue, &b->_Queue);
}

static void test_initialization(test_context *ctx)
{
  struct _Mutex_Control mtx = _MUTEX_INITIALIZER;
  struct _Mutex_recursive_Control rec_mtx = _MUTEX_RECURSIVE_INITIALIZER;
  struct _Condition_Control cond = _CONDITION_INITIALIZER;
  struct _Semaphore_Control sem = _SEMAPHORE_INITIALIZER(1);
  struct _Futex_Control futex = _FUTEX_INITIALIZER;

  _Mutex_Initialize(&ctx->mtx);
  _Mutex_recursive_Initialize(&ctx->rec_mtx);
  _Condition_Initialize(&ctx->cond);
  _Semaphore_Initialize(&ctx->sem, 1);
  _Futex_Initialize(&ctx->futex);

  rtems_test_assert(eq_mtx(&mtx, &ctx->mtx));
  rtems_test_assert(eq_rec_mtx(&rec_mtx, &ctx->rec_mtx));
  rtems_test_assert(eq_cond(&cond, &ctx->cond));
  rtems_test_assert(eq_sem(&sem, &ctx->sem));
  rtems_test_assert(eq_futex(&futex, &ctx->futex));

  _Mutex_Destroy(&mtx);
  _Mutex_recursive_Destroy(&rec_mtx);
  _Condition_Destroy(&cond);
  _Semaphore_Destroy(&sem);
  _Futex_Destroy(&futex);
}

static void test_recursive_acquire_normal(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;
  size_t idx = 0;
  int eno;

  eno = _Mutex_Try_acquire(mtx);
  rtems_test_assert(eno == 0);

  eno = _Mutex_Try_acquire(mtx);
  rtems_test_assert(eno == EBUSY);

  _Mutex_Release(mtx);

  eno = _Mutex_Try_acquire(mtx);
  rtems_test_assert(eno == 0);

  _Mutex_Release(mtx);

  _Mutex_Acquire(mtx);

  eno = _Mutex_Try_acquire(mtx);
  rtems_test_assert(eno == EBUSY);

  _Mutex_Release(mtx);

  send_event(ctx, idx, EVENT_MTX_ACQUIRE);

  eno = _Mutex_Try_acquire(mtx);
  rtems_test_assert(eno == EBUSY);

  send_event(ctx, idx, EVENT_MTX_RELEASE);
}

static void test_recursive_acquire_recursive(test_context *ctx)
{
  struct _Mutex_recursive_Control *mtx = &ctx->rec_mtx;
  size_t idx = 0;
  int eno;

  eno = _Mutex_recursive_Try_acquire(mtx);
  rtems_test_assert(eno == 0);

  _Mutex_recursive_Acquire(mtx);

  eno = _Mutex_recursive_Try_acquire(mtx);
  rtems_test_assert(eno == 0);

  _Mutex_recursive_Release(mtx);
  _Mutex_recursive_Release(mtx);
  _Mutex_recursive_Release(mtx);

  send_event(ctx, idx, EVENT_REC_MTX_ACQUIRE);

  eno = _Mutex_recursive_Try_acquire(mtx);
  rtems_test_assert(eno == EBUSY);

  send_event(ctx, idx, EVENT_REC_MTX_RELEASE);
}

static void test_prio_acquire_order(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;
  size_t a = 0;
  size_t b = 1;
  int gen_a;
  int gen_b;

  _Mutex_Acquire(mtx);

  gen_a = ctx->generation[a];
  gen_b = ctx->generation[b];

  send_event(ctx, b, EVENT_MTX_ACQUIRE);
  send_event(ctx, a, EVENT_MTX_ACQUIRE);

  rtems_test_assert(ctx->generation[a] == gen_a);
  rtems_test_assert(ctx->generation[b] == gen_b);

  _Mutex_Release(mtx);

  rtems_test_assert(ctx->generation[a] == gen_a + 1);
  rtems_test_assert(ctx->generation[b] == gen_b);

  send_event(ctx, a, EVENT_MTX_RELEASE);

  rtems_test_assert(ctx->generation[a] == gen_a + 1);
  rtems_test_assert(ctx->generation[b] == gen_b + 1);

  send_event(ctx, b, EVENT_MTX_RELEASE);
}

static void test_prio_inv_normal(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;
  size_t idx = 0;
  int gen;

  _Mutex_Acquire(mtx);
  gen = ctx->generation[idx];
  send_event(ctx, idx, EVENT_MTX_PRIO_INV);
  rtems_test_assert(ctx->generation[idx] == gen);
  _Mutex_Release(mtx);
  rtems_test_assert(ctx->generation[idx] == gen + 1);
}

static void test_prio_inv_recursive(test_context *ctx)
{
  struct _Mutex_recursive_Control *mtx = &ctx->rec_mtx;
  size_t idx = 0;
  int gen;

  _Mutex_recursive_Acquire(mtx);
  gen = ctx->generation[idx];
  send_event(ctx, idx, EVENT_REC_MTX_PRIO_INV);
  rtems_test_assert(ctx->generation[idx] == gen);
  _Mutex_recursive_Release(mtx);
  rtems_test_assert(ctx->generation[idx] == gen + 1);
}

static void test_mtx_timeout_normal(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;
  size_t idx = 0;
  struct timespec to;
  int eno;

  eno = _Mutex_Acquire_timed(mtx, NULL);
  rtems_test_assert(eno == 0);
  _Mutex_Release(mtx);

  send_event(ctx, idx, EVENT_MTX_ACQUIRE);

  memset(&to, 0x00, sizeof(to));
  eno = _Mutex_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);
  memset(&to, 0xff, sizeof(to));
  eno = _Mutex_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == EINVAL);
  get_abs_timeout(&to);
  eno = _Mutex_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);

  send_event(ctx, idx, EVENT_MTX_RELEASE);
}

static void test_mtx_timeout_recursive(test_context *ctx)
{
  struct _Mutex_recursive_Control *mtx = &ctx->rec_mtx;
  size_t idx = 0;
  struct timespec to;
  int eno;

  eno = _Mutex_recursive_Acquire_timed(mtx, NULL);
  rtems_test_assert(eno == 0);
  eno = _Mutex_recursive_Acquire_timed(mtx, NULL);
  rtems_test_assert(eno == 0);
  _Mutex_recursive_Release(mtx);
  _Mutex_recursive_Release(mtx);

  send_event(ctx, idx, EVENT_REC_MTX_ACQUIRE);

  memset(&to, 0x00, sizeof(to));
  eno = _Mutex_recursive_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);
  memset(&to, 0xff, sizeof(to));
  eno = _Mutex_recursive_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == EINVAL);
  get_abs_timeout(&to);
  eno = _Mutex_recursive_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);

  send_event(ctx, idx, EVENT_REC_MTX_RELEASE);
}

static void test_mtx_deadlock(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;

  _Mutex_Acquire(mtx);

  if (setjmp(ctx->deadlock_return_context) == 0) {
    _Mutex_Acquire(mtx);
  }

  _Mutex_Release(mtx);
}

static void test_condition(test_context *ctx)
{
  struct _Condition_Control *cond = &ctx->cond;
  size_t a = 0;
  size_t b = 1;
  int gen_a;
  int gen_b;

  gen_a = ctx->generation[a];
  gen_b = ctx->generation[b];

  _Condition_Signal(cond);
  _Condition_Broadcast(cond);

  send_event(ctx, a, EVENT_CONDITION_WAIT);
  send_event(ctx, b, EVENT_CONDITION_WAIT_REC);

  rtems_test_assert(ctx->generation[a] == gen_a + 1);
  rtems_test_assert(ctx->generation[b] == gen_b + 1);

  _Condition_Signal(cond);

  rtems_test_assert(ctx->generation[a] == gen_a + 2);
  rtems_test_assert(ctx->generation[b] == gen_b + 1);

  _Condition_Signal(cond);

  rtems_test_assert(ctx->generation[a] == gen_a + 2);
  rtems_test_assert(ctx->generation[b] == gen_b + 2);

  send_event(ctx, a, EVENT_CONDITION_WAIT);
  send_event(ctx, b, EVENT_CONDITION_WAIT_REC);

  rtems_test_assert(ctx->generation[a] == gen_a + 3);
  rtems_test_assert(ctx->generation[b] == gen_b + 3);

  _Condition_Broadcast(cond);

  rtems_test_assert(ctx->generation[a] == gen_a + 4);
  rtems_test_assert(ctx->generation[b] == gen_b + 4);
}

static void test_condition_timeout(test_context *ctx)
{
  struct timespec to;
  int eno;

  _Mutex_Acquire(&ctx->mtx);
  memset(&to, 0x00, sizeof(to));
  eno = _Condition_Wait_timed(&ctx->cond, &ctx->mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);
  memset(&to, 0xff, sizeof(to));
  eno = _Condition_Wait_timed(&ctx->cond, &ctx->mtx, &to);
  rtems_test_assert(eno == EINVAL);
  get_abs_timeout(&to);
  eno = _Condition_Wait_timed(&ctx->cond, &ctx->mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);
  _Mutex_Release(&ctx->mtx);

  _Mutex_recursive_Acquire(&ctx->rec_mtx);
  _Mutex_recursive_Acquire(&ctx->rec_mtx);
  memset(&to, 0x00, sizeof(to));
  eno = _Condition_Wait_recursive_timed(&ctx->cond, &ctx->rec_mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);
  memset(&to, 0xff, sizeof(to));
  eno = _Condition_Wait_recursive_timed(&ctx->cond, &ctx->rec_mtx, &to);
  rtems_test_assert(eno == EINVAL);
  get_abs_timeout(&to);
  eno = _Condition_Wait_recursive_timed(&ctx->cond, &ctx->rec_mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);
  _Mutex_recursive_Release(&ctx->rec_mtx);
  _Mutex_recursive_Release(&ctx->rec_mtx);
}

static void test_sem(test_context *ctx)
{
  struct _Semaphore_Control *sem = &ctx->sem;
  size_t idx = 0;
  int gen;

  _Semaphore_Wait(sem);
  gen = ctx->generation[idx];
  send_event(ctx, idx, EVENT_SEM_WAIT);
  rtems_test_assert(ctx->generation[idx] == gen);
  _Semaphore_Post(sem);
  rtems_test_assert(ctx->generation[idx] == gen + 1);
  _Semaphore_Post(sem);
}

static void test_sem_prio_wait_order(test_context *ctx)
{
  struct _Semaphore_Control *sem = &ctx->sem;
  size_t a = 0;
  size_t b = 1;
  int gen_a;
  int gen_b;

  _Semaphore_Wait(sem);

  gen_a = ctx->generation[a];
  gen_b = ctx->generation[b];

  send_event(ctx, b, EVENT_SEM_WAIT);
  send_event(ctx, a, EVENT_SEM_WAIT);

  rtems_test_assert(ctx->generation[a] == gen_a);
  rtems_test_assert(ctx->generation[b] == gen_b);

  _Semaphore_Post(sem);

  rtems_test_assert(ctx->generation[a] == gen_a + 1);
  rtems_test_assert(ctx->generation[b] == gen_b);

  _Semaphore_Post(sem);

  rtems_test_assert(ctx->generation[a] == gen_a + 1);
  rtems_test_assert(ctx->generation[b] == gen_b + 1);
}

static void test_futex(test_context *ctx)
{
  struct _Futex_Control *futex = &ctx->futex;
  size_t a = 0;
  size_t b = 1;
  int eno;
  int woken;

  eno = _Futex_Wait(futex, &ctx->val, 1);
  rtems_test_assert(eno == EWOULDBLOCK);

  woken = _Futex_Wake(futex, 0);
  rtems_test_assert(woken == 0);

  woken = _Futex_Wake(futex, 1);
  rtems_test_assert(woken == 0);

  ctx->val = 1;

  ctx->eno[a] = -1;
  send_event(ctx, a, EVENT_FUTEX_WAIT);
  rtems_test_assert(ctx->eno[a] == -1);

  woken = _Futex_Wake(futex, INT_MAX);
  rtems_test_assert(woken == 1);
  rtems_test_assert(ctx->eno[a] == 0);

  ctx->eno[a] = -1;
  ctx->eno[b] = -1;
  send_event(ctx, a, EVENT_FUTEX_WAIT);
  send_event(ctx, b, EVENT_FUTEX_WAIT);
  rtems_test_assert(ctx->eno[a] == -1);
  rtems_test_assert(ctx->eno[b] == -1);

  woken = _Futex_Wake(futex, 1);
  rtems_test_assert(woken == 1);
  rtems_test_assert(ctx->eno[a] == 0);
  rtems_test_assert(ctx->eno[b] == -1);

  woken = _Futex_Wake(futex, 1);
  rtems_test_assert(woken == 1);
  rtems_test_assert(ctx->eno[a] == 0);
  rtems_test_assert(ctx->eno[b] == 0);

  ctx->eno[a] = -1;
  ctx->eno[b] = -1;
  send_event(ctx, a, EVENT_FUTEX_WAIT);
  send_event(ctx, b, EVENT_FUTEX_WAIT);
  rtems_test_assert(ctx->eno[a] == -1);
  rtems_test_assert(ctx->eno[b] == -1);

  woken = _Futex_Wake(futex, 2);
  rtems_test_assert(woken == 2);
  rtems_test_assert(ctx->eno[a] == 0);
  rtems_test_assert(ctx->eno[b] == 0);
}

static void test_sched(void)
{
  rtems_test_assert(_Sched_Index() == 0);
  rtems_test_assert(_Sched_Name_to_index("", 0) == -1);
  rtems_test_assert(_Sched_Name_to_index("b", 1) == -1);
  rtems_test_assert(_Sched_Name_to_index("bl", 2) == -1);
  rtems_test_assert(_Sched_Name_to_index("blu", 3) == -1);
  rtems_test_assert(_Sched_Name_to_index("blue", 4) == 0);
  rtems_test_assert(_Sched_Name_to_index("blueX", 5) == 0);
  rtems_test_assert(_Sched_Processor_count(-1) == 0);
  rtems_test_assert(_Sched_Processor_count(0) == 1);
  rtems_test_assert(_Sched_Processor_count(1) == 0);
}

static void mid_task(rtems_task_argument arg)
{
  rtems_test_assert(0);
}

static void high_task(rtems_task_argument idx)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  if (idx == 0) {
    sc = rtems_task_start(ctx->mid, mid_task, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_suspend(ctx->mid);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  while (true) {
    rtems_event_set events;

    sc = rtems_event_receive(
      RTEMS_ALL_EVENTS,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    if ((events & EVENT_MTX_ACQUIRE) != 0) {
      _Mutex_Acquire(&ctx->mtx);
      ctx->generation[idx] = generation(ctx, idx);
    }

    if ((events & EVENT_MTX_RELEASE) != 0) {
      _Mutex_Release(&ctx->mtx);
    }

    if ((events & EVENT_MTX_PRIO_INV) != 0) {
      sc = rtems_task_resume(ctx->mid);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      _Mutex_Acquire(&ctx->mtx);
      ctx->generation[idx] = generation(ctx, idx);
      _Mutex_Release(&ctx->mtx);

      sc = rtems_task_suspend(ctx->mid);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }

    if ((events & EVENT_REC_MTX_ACQUIRE) != 0) {
      _Mutex_recursive_Acquire(&ctx->rec_mtx);
    }

    if ((events & EVENT_REC_MTX_RELEASE) != 0) {
      _Mutex_recursive_Release(&ctx->rec_mtx);
    }

    if ((events & EVENT_REC_MTX_PRIO_INV) != 0) {
      sc = rtems_task_resume(ctx->mid);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      _Mutex_recursive_Acquire(&ctx->rec_mtx);
      ctx->generation[idx] = generation(ctx, idx);
      _Mutex_recursive_Release(&ctx->rec_mtx);

      sc = rtems_task_suspend(ctx->mid);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }

    if ((events & EVENT_SEM_WAIT) != 0) {
      _Semaphore_Wait(&ctx->sem);
      ctx->generation[idx] = generation(ctx, idx);
    }

    if ((events & EVENT_FUTEX_WAIT) != 0) {
      ctx->eno[idx] = _Futex_Wait(&ctx->futex, &ctx->val, 1);
    }

    if ((events & EVENT_CONDITION_WAIT) != 0) {
      _Mutex_Acquire(&ctx->mtx);
      ctx->generation[idx] = generation(ctx, idx);
      _Condition_Wait(&ctx->cond, &ctx->mtx);
      ctx->generation[idx] = generation(ctx, idx);
      _Mutex_Release(&ctx->mtx);
    }

    if ((events & EVENT_CONDITION_WAIT_REC) != 0) {
      _Mutex_recursive_Acquire(&ctx->rec_mtx);
      _Mutex_recursive_Acquire(&ctx->rec_mtx);
      ctx->generation[idx] = generation(ctx, idx);
      _Condition_Wait_recursive(&ctx->cond, &ctx->rec_mtx);
      ctx->generation[idx] = generation(ctx, idx);
      _Mutex_recursive_Release(&ctx->rec_mtx);
      _Mutex_recursive_Release(&ctx->rec_mtx);
    }
  }
}

static void test(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  test_initialization(ctx);

  ctx->low = rtems_task_self();

  sc = rtems_task_create(
    rtems_build_name('M', 'I', 'D', ' '),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->mid
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', '0'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->high[0], high_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', '1'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high[1]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->high[1], high_task, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_recursive_acquire_normal(ctx);
  test_recursive_acquire_recursive(ctx);
  test_prio_acquire_order(ctx);
  test_prio_inv_normal(ctx);
  test_prio_inv_recursive(ctx);
  test_mtx_timeout_normal(ctx);
  test_mtx_timeout_recursive(ctx);
  test_mtx_deadlock(ctx);
  test_condition(ctx);
  test_condition_timeout(ctx);
  test_sem(ctx);
  test_sem_prio_wait_order(ctx);
  test_futex(ctx);
  test_sched();

  sc = rtems_task_delete(ctx->mid);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->high[0]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(ctx->high[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  _Mutex_Destroy(&ctx->mtx);
  _Mutex_recursive_Destroy(&ctx->rec_mtx);
  _Condition_Destroy(&ctx->cond);
  _Semaphore_Destroy(&ctx->sem);
  _Futex_Destroy(&ctx->futex);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{

  if (
    source == INTERNAL_ERROR_CORE
      && !always_set_to_false
      && error == INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK
  ) {
    test_context *ctx = &test_instance;

    longjmp(ctx->deadlock_return_context, 1);
  }
}

#define CONFIGURE_MICROSECONDS_PER_TICK US_PER_TICK

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_SCHEDULER_NAME rtems_build_name('b', 'l', 'u', 'e')

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
