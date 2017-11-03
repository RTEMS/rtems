/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

/*
 * This test program runs also on GNU/Linux.  Use
 *
 * cc init.c -pthread -Wall -Wextra -lrt && sudo ./a.out
 *
 * to run it.  It must run probably as root for the clock_settime().
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __rtems__

#include "tmacros.h"

#else /* __rtems__ */

#include <assert.h>

#define rtems_test_assert(x) assert(x)

#endif /* __rtems__ */

static void assert_eno(const char *hint, int eno, int expected_eno)
{
  const char *warn;

  if (eno != expected_eno) {
    warn = "WARNING: ";
  } else {
    warn = "";
  }

  printf(
    "%s%s: actual '%s', expected '%s'\n",
    warn,
    hint,
    strerror(eno),
    strerror(expected_eno)
  );
#ifdef __rtems__
  rtems_test_assert(eno == expected_eno);
#endif /* __rtems__ */
}

static void assert_rv(const char *hint, int rv, int expected_eno)
{
  int eno;

  if (rv != 0) {
    eno = errno;
  } else {
    eno = 0;
  }

  assert_eno(hint, eno, expected_eno);
}

typedef struct {
  char data;
} test_msg;

#define MSG_COUNT 1

#define MSG_SIZE sizeof(test_msg)

typedef struct {
  int value;
  pthread_mutex_t mtx;
  pthread_cond_t cnd;
} event;

typedef enum {
  ACTION_NONE,
  ACTION_MTX_LOCK,
  ACTION_CND_WAIT,
  ACTION_RW_RDLOCK,
  ACTION_RW_WRLOCK,
  ACTION_SEM_WAIT,
  ACTION_MQ_SEND,
  ACTION_MQ_RECV,
  ACTION_CLOCK_NANOSLEEP,
  ACTION_TERMINATE
} test_action;

typedef enum {
  MODE_TIMEOUT_FINITE,
  MODE_TIMEOUT_NEGATIVE_SEC,
  MODE_TIMEOUT_NEGATIVE_NSEC,
  MODE_TIMEOUT_NEGATIVE_SEC_NSEC,
  MODE_TIMEOUT_HUGE_NSEC,
#ifdef __rtems__
  MODE_TIMEOUT_NULL,
#endif /* __rtems__ */
  MODE_TIMEOUT_PRACTICALLY_INFINITE,
  MODE_COUNT
} test_mode;

typedef struct {
  test_mode mode;
  pthread_t worker;
  event action;
  event done;
  pthread_mutex_t mtx;
  pthread_mutex_t mtx2;
  pthread_cond_t cnd;
  pthread_rwlock_t rw;
  sem_t sem;
  mqd_t mq;
  int counter[ACTION_TERMINATE + 1];
} test_context;

static test_context test_instance;

static void event_init(event *e)
{
  int eno;

  eno = pthread_mutex_init(&e->mtx, NULL);
  rtems_test_assert(eno == 0);

  eno = pthread_cond_init(&e->cnd, NULL);
  rtems_test_assert(eno == 0);
}

static void event_destroy(event *e)
{
  int eno;

  eno = pthread_mutex_destroy(&e->mtx);
  rtems_test_assert(eno == 0);

  eno = pthread_cond_destroy(&e->cnd);
  rtems_test_assert(eno == 0);
}

static void event_post(event *e, int value)
{
  int eno;

  eno = pthread_mutex_lock(&e->mtx);
  rtems_test_assert(eno == 0);

  e->value = value;

  eno = pthread_cond_signal(&e->cnd);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_unlock(&e->mtx);
  rtems_test_assert(eno == 0);
}

static int event_get(event *e)
{
  int eno;
  int value;

  eno = pthread_mutex_lock(&e->mtx);
  rtems_test_assert(eno == 0);

  while (true) {
    value = e->value;

    if (value != 0) {
      e->value = 0;
      break;
    }

    eno = pthread_cond_wait(&e->cnd, &e->mtx);
    rtems_test_assert(eno == 0);
  }

  eno = pthread_mutex_unlock(&e->mtx);
  rtems_test_assert(eno == 0);

  return value;
}

static void *worker(void *arg)
{
  test_context *ctx;
  test_action action;
  test_mode mode;
  int eno;
  int unused;

  ctx = arg;
  mode = ctx->mode;

  eno = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &unused);
  rtems_test_assert(eno == 0);

  eno = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &unused);
  rtems_test_assert(eno == 0);

  while ((action = event_get(&ctx->action)) != ACTION_TERMINATE) {
    int rv;
    struct timespec to_storage;
    const struct timespec *to;
    int expected_eno;
    test_msg msg;
    unsigned prio;

    switch (mode) {
      case MODE_TIMEOUT_FINITE:
        rv = clock_gettime(CLOCK_REALTIME, &to_storage);
        rtems_test_assert(rv == 0);

        to_storage.tv_sec += 3600;
        to = &to_storage;
        expected_eno = ETIMEDOUT;
        break;
      case MODE_TIMEOUT_NEGATIVE_SEC:
        to_storage.tv_sec = -1;
        to_storage.tv_nsec = 1;
        to = &to_storage;
        expected_eno = ETIMEDOUT;
        break;
      case MODE_TIMEOUT_NEGATIVE_NSEC:
        to_storage.tv_sec = 1;
        to_storage.tv_nsec = -1;
        to = &to_storage;
        expected_eno = EINVAL;
        break;
      case MODE_TIMEOUT_NEGATIVE_SEC_NSEC:
        to_storage.tv_sec = -1;
        to_storage.tv_nsec = -1;
        to = &to_storage;
        expected_eno = EINVAL;
        break;
      case MODE_TIMEOUT_HUGE_NSEC:
        to_storage.tv_sec = 1;
        to_storage.tv_nsec = LONG_MAX;
        to = &to_storage;
        expected_eno = EINVAL;
        break;
#ifdef __rtems__
      case MODE_TIMEOUT_NULL:
        to = NULL;
        expected_eno = EINVAL;
        break;
#endif /* __rtems__ */
      case MODE_TIMEOUT_PRACTICALLY_INFINITE:
        to_storage.tv_sec = INT64_MAX;
        to_storage.tv_nsec = 999999999;
        to = &to_storage;
        expected_eno = -1;
        break;
      default:
        to = NULL;
        expected_eno = -1;
        rtems_test_assert(0);
        break;
    }

    switch (action) {
      case ACTION_MTX_LOCK:
        eno = pthread_mutex_timedlock(&ctx->mtx, to);
        assert_eno("pthread_mutex_timedlock", eno, expected_eno);
        break;
      case ACTION_CND_WAIT:
        eno = pthread_mutex_lock(&ctx->mtx2);
        rtems_test_assert(eno == 0);

        eno = pthread_cond_timedwait(&ctx->cnd, &ctx->mtx2, to);
        assert_eno("pthread_cond_timedwait", eno, expected_eno);

        eno = pthread_mutex_unlock(&ctx->mtx2);
        rtems_test_assert(eno == 0);
        break;
      case ACTION_RW_RDLOCK:
        eno = pthread_rwlock_timedrdlock(&ctx->rw, to);
        assert_eno("pthread_rwlock_timedrdlock", eno, expected_eno);
        break;
      case ACTION_RW_WRLOCK:
        eno = pthread_rwlock_timedwrlock(&ctx->rw, to);
        assert_eno("pthread_rwlock_timedwrlock", eno, expected_eno);
        break;
      case ACTION_SEM_WAIT:
        errno = 0;
        rv = sem_timedwait(&ctx->sem, to);
        assert_rv("sem_timedwait", rv, expected_eno);
        break;
      case ACTION_MQ_SEND:
        msg.data = 13;
        rv = mq_send(ctx->mq, &msg.data, sizeof(msg), 7);
        rtems_test_assert(rv == 0);
        msg.data = 31;
        errno = 0;
        rv = mq_timedsend(ctx->mq, &msg.data, sizeof(msg), 5, to);
        assert_rv("mq_timedsend", rv, expected_eno);
        break;
      case ACTION_MQ_RECV:
        msg.data = 0;
        prio = 0;
        rv = mq_receive(ctx->mq, &msg.data, sizeof(msg), &prio);
        rtems_test_assert(rv == 1);
        rtems_test_assert(msg.data == 13);
        rtems_test_assert(prio == 7);
        msg.data = 0;
        prio = 0;
        errno = 0;
        rv = mq_timedreceive(ctx->mq, &msg.data, sizeof(msg), &prio, to);
        assert_rv("mq_timedreceive", rv, expected_eno);
        rtems_test_assert(msg.data == 0);
        rtems_test_assert(prio == 0);
        break;
      case ACTION_CLOCK_NANOSLEEP:
        rv = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, to, NULL);

        if (expected_eno == ETIMEDOUT) {
          assert_rv("clock_nanosleep", rv, 0);
        } else {
          assert_rv("clock_nanosleep", rv, expected_eno);
        }
        break;
      default:
        rtems_test_assert(0);
        break;
    }

    ++ctx->counter[action];

    event_post(&ctx->done, 1);
  }

  return ctx;
}

static void delay(void)
{
  int rv;

  rv = usleep(100000);
  rtems_test_assert(rv == 0);
}

static void run(
  test_context *ctx,
  const char *test,
  test_mode mode,
  void (*f)(void)
)
{
  test_action action;
  int eno;
  void *status;

  printf("*** %s ***\n", test);
  ctx->mode = mode;

  eno = pthread_create(&ctx->worker, NULL, worker, ctx);
  rtems_test_assert(eno == 0);

  for (action = ACTION_MTX_LOCK; action < ACTION_TERMINATE; ++action) {
    event_post(&ctx->action, action);
    delay();

    (*f)();

    event_get(&ctx->done);
  }

  event_post(&ctx->action, ACTION_TERMINATE);

  status = NULL;
  eno = pthread_join(ctx->worker, &status);
  rtems_test_assert(eno == 0);
  rtems_test_assert(status == ctx);
}

static void timeout_finite(void)
{
  struct timespec now;
  int rv;

  rv = clock_gettime(CLOCK_REALTIME, &now);
  rtems_test_assert(rv == 0);

  now.tv_sec += 7200;

  rv = clock_settime(CLOCK_REALTIME, &now);
  rtems_test_assert(rv == 0);
}

static void test_timeout_finite(test_context *ctx)
{
  run(ctx, "timeout finite", MODE_TIMEOUT_FINITE, timeout_finite);
}

static void do_nothing(void)
{
  /* Do nothing */
}

static void test_timeout_negative_sec(test_context *ctx)
{
  run(ctx, "timeout negative sec", MODE_TIMEOUT_NEGATIVE_SEC, do_nothing);
}

static void test_timeout_negative_nsec(test_context *ctx)
{
  run(ctx, "timout negative nsec", MODE_TIMEOUT_NEGATIVE_NSEC, do_nothing);
}

static void test_timeout_negative_sec_nsec(test_context *ctx)
{
  run(
    ctx,
    "timout negative sec and nsec",
    MODE_TIMEOUT_NEGATIVE_SEC_NSEC,
    do_nothing
  );
}

static void test_timeout_huge_nsec(test_context *ctx)
{
  run(ctx, "timout huge nsec", MODE_TIMEOUT_HUGE_NSEC, do_nothing);
}

#ifdef __rtems__
static void test_timeout_null(test_context *ctx)
{
  run(ctx, "timeout NULL", MODE_TIMEOUT_NULL, do_nothing);
}
#endif /* __rtems__ */

static void test_timeout_practically_infinite(test_context *ctx)
{
  test_action action;
  int eno;

  puts("*** timeout practically infinite ***");
  ctx->mode = MODE_TIMEOUT_PRACTICALLY_INFINITE;

  for (action = ACTION_MTX_LOCK; action < ACTION_TERMINATE; ++action) {
    void *status;

    eno = pthread_create(&ctx->worker, NULL, worker, ctx);
    rtems_test_assert(eno == 0);

    event_post(&ctx->action, action);
    delay();

    eno = pthread_cancel(ctx->worker);
    rtems_test_assert(eno == 0);

    status = NULL;
    eno = pthread_join(ctx->worker, &status);
    rtems_test_assert(eno == 0);
    rtems_test_assert(status == PTHREAD_CANCELED);
  }
}

static void test(test_context *ctx)
{
  test_action action;
  int eno;
  int rv;
  mode_t mode;
  struct mq_attr ma;

  event_init(&ctx->action);
  event_init(&ctx->done);

  eno = pthread_mutex_init(&ctx->mtx, NULL);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_lock(&ctx->mtx);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_init(&ctx->mtx2, NULL);
  rtems_test_assert(eno == 0);

  eno = pthread_cond_init(&ctx->cnd, NULL);
  rtems_test_assert(eno == 0);

  eno = pthread_rwlock_init(&ctx->rw, NULL);
  rtems_test_assert(eno == 0);

  eno = pthread_rwlock_wrlock(&ctx->rw);
  rtems_test_assert(eno == 0);

  rv = sem_init(&ctx->sem, 0, 0);
  rtems_test_assert(rv == 0);

  mode = S_IRWXU | S_IRWXG | S_IRWXO;
  memset(&ma, 0, sizeof(ma));
  ma.mq_maxmsg = MSG_COUNT;
  ma.mq_msgsize = MSG_SIZE;
  ctx->mq = mq_open("/mq", O_CREAT | O_RDWR, mode, &ma);
  rtems_test_assert(ctx->mq != (mqd_t) -1);

  test_timeout_finite(ctx);
  test_timeout_negative_sec(ctx);
  test_timeout_negative_nsec(ctx);
  test_timeout_negative_sec_nsec(ctx);
  test_timeout_huge_nsec(ctx);
#ifdef __rtems__
  test_timeout_null(ctx);
#endif /* __rtems__ */
  test_timeout_practically_infinite(ctx);

  event_destroy(&ctx->action);
  event_destroy(&ctx->done);

  eno = pthread_mutex_unlock(&ctx->mtx);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_destroy(&ctx->mtx);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_destroy(&ctx->mtx2);
#ifdef __rtems__
  rtems_test_assert(eno == 0);
#else /* __rtems__ */
  rtems_test_assert(eno == 0 || eno == EBUSY);
#endif /* __rtems__ */

  eno = pthread_cond_destroy(&ctx->cnd);
  rtems_test_assert(eno == 0);

  eno = pthread_rwlock_unlock(&ctx->rw);
  rtems_test_assert(eno == 0);

  eno = pthread_rwlock_destroy(&ctx->rw);
  rtems_test_assert(eno == 0);

  rv = sem_destroy(&ctx->sem);
  rtems_test_assert(rv == 0);

  rv = mq_close(ctx->mq);
  rtems_test_assert(rv == 0);

  rv = mq_unlink("/mq");
  rtems_test_assert(rv == 0);

  for (action = ACTION_MTX_LOCK; action < ACTION_TERMINATE; ++action) {
    rtems_test_assert(ctx->counter[action] == MODE_COUNT - 1);
  }
}

#ifdef __rtems__

const char rtems_test_name[] = "PSXCLOCKREALTIME 1";

static void *POSIX_Init(void *arg)
{
  TEST_BEGIN();
  test(&test_instance);
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#else /* __rtems__ */

int main(void)
{
  test(&test_instance);
  return 0;
}

#endif /* __rtems__ */
