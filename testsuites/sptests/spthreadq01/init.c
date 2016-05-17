/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#include <rtems.h>

#include <rtems/score/threadimpl.h>

#if defined(RTEMS_POSIX_API)
  #include <fcntl.h>
  #include <mqueue.h>
  #include <semaphore.h>
  #include <string.h>
  #include <pthread.h>
#endif

const char rtems_test_name[] = "SPTHREADQ 1";

static Thread_queue_Control queue = THREAD_QUEUE_INITIALIZER( "Queue" );

typedef struct {
  Thread_Control *master;
  rtems_id master_id;
  rtems_id worker_id;
  rtems_id sem;
  rtems_id mtx;
  rtems_id mq;
  rtems_id br;
#if defined(RTEMS_POSIX_API)
  sem_t psem;
  pthread_mutex_t pmtx;
  pthread_cond_t pcv;
  pthread_rwlock_t prw;
  mqd_t pmq;
#endif
} test_context;

static test_context test_instance;

static void wait_for_worker(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wake_up_master(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_event_transient_send(ctx->master_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static rtems_id get_wait_id(test_context *ctx)
{
  ISR_lock_Context lock_context;
  void *lock;
  rtems_id id;

  lock = _Thread_Lock_acquire(ctx->master, &lock_context);
  id = _Thread_Wait_get_id(ctx->master);
  _Thread_Lock_release(lock, &lock_context);

  return id;
}

static void classic_worker(test_context *ctx)
{
  rtems_status_code sc;
  char buf[1];

  wake_up_master(ctx);
  rtems_test_assert(get_wait_id(ctx) == ctx->sem);

  sc = rtems_semaphore_release(ctx->sem);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(ctx->mtx, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wake_up_master(ctx);
  rtems_test_assert(get_wait_id(ctx) == ctx->mtx);

  sc = rtems_semaphore_release(ctx->mtx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wake_up_master(ctx);
  rtems_test_assert(get_wait_id(ctx) == ctx->mq);

  buf[0] = 'X';
  sc = rtems_message_queue_send(ctx->mq, &buf[0], sizeof(buf));
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wake_up_master(ctx);
  rtems_test_assert(get_wait_id(ctx) == ctx->br);

  sc = rtems_barrier_wait(ctx->br, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void posix_worker(test_context *ctx)
{
#if defined(RTEMS_POSIX_API)
  int rv;
  int eno;
  char buf[1];

  wake_up_master(ctx);
  rtems_test_assert(get_wait_id(ctx) == ctx->psem);

  rv = sem_post(&ctx->psem);
  rtems_test_assert(rv == 0);

  eno = pthread_mutex_lock(&ctx->pmtx);
  rtems_test_assert(eno == 0);

  wake_up_master(ctx);
  rtems_test_assert(get_wait_id(ctx) == ctx->pmtx);

  eno = pthread_mutex_unlock(&ctx->pmtx);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_lock(&ctx->pmtx);
  rtems_test_assert(eno == 0);

  rtems_test_assert(get_wait_id(ctx) == ctx->pcv);

  eno = pthread_cond_signal(&ctx->pcv);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_unlock(&ctx->pmtx);
  rtems_test_assert(eno == 0);

  eno = pthread_rwlock_wrlock(&ctx->prw);
  rtems_test_assert(eno == 0);

  wake_up_master(ctx);
  rtems_test_assert(get_wait_id(ctx) == ctx->prw);

  eno = pthread_rwlock_unlock(&ctx->prw);
  rtems_test_assert(eno == 0);

  wake_up_master(ctx);
  rtems_test_assert(get_wait_id(ctx) == ctx->pmq);

  buf[0] = 'x';
  rv = mq_send(ctx->pmq, &buf[0], sizeof(buf), 0);
  rtems_test_assert(rv == 0);
#endif
}

static rtems_task worker(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  rtems_test_assert(get_wait_id(ctx) == 0);

  classic_worker(ctx);
  posix_worker(ctx);
}

static void test_classic_init(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', ' '),
    0,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &ctx->sem
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'T', 'X', ' '),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &ctx->mtx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_message_queue_create(
    rtems_build_name('M', 'Q', ' ', ' '),
    1,
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->mq
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_barrier_create(
    rtems_build_name('B', 'R', ' ', ' '),
    RTEMS_BARRIER_AUTOMATIC_RELEASE,
    2,
    &ctx->br
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_posix_init(test_context *ctx)
{
#if defined(RTEMS_POSIX_API)
  int rv;
  int eno;
  struct mq_attr attr;

  rv = sem_init(&ctx->psem, 0, 0);
  rtems_test_assert(rv == 0);

  eno = pthread_mutex_init(&ctx->pmtx, NULL);
  rtems_test_assert(eno == 0);

  eno = pthread_cond_init(&ctx->pcv, NULL);
  rtems_test_assert(eno == 0);

  eno = pthread_rwlock_init(&ctx->prw, NULL);
  rtems_test_assert(eno == 0);

  memset(&attr, 0, sizeof(attr));
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = sizeof(char);

  ctx->pmq = mq_open("mq", O_CREAT | O_RDWR, 0x777, &attr);
  rtems_test_assert(ctx->mq != -1);
#endif
}

static void test_context_init(test_context *ctx)
{
  rtems_status_code sc;

  ctx->master = _Thread_Get_executing();
  ctx->master_id = rtems_task_self();

  test_classic_init(ctx);
  test_posix_init(ctx);

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_id, worker, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_classic_obj(test_context *ctx)
{
  rtems_status_code sc;
  char buf[1];
  size_t n;

  wait_for_worker(ctx);

  sc = rtems_semaphore_obtain(ctx->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wait_for_worker(ctx);

  sc = rtems_semaphore_obtain(ctx->mtx, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wait_for_worker(ctx);

  buf[0] = 'Y';
  n = 123;
  sc = rtems_message_queue_receive(
    ctx->mq,
    &buf[0],
    &n,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(buf[0] == 'X');
  rtems_test_assert(n == sizeof(buf));

  wait_for_worker(ctx);

  sc = rtems_barrier_wait(ctx->br, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_posix_obj(test_context *ctx)
{
#if defined(RTEMS_POSIX_API)
  int rv;
  int eno;
  char buf[1];
  unsigned prio;
  ssize_t n;

  wait_for_worker(ctx);

  rv = sem_wait(&ctx->psem);
  rtems_test_assert(rv == 0);

  wait_for_worker(ctx);

  eno = pthread_mutex_lock(&ctx->pmtx);
  rtems_test_assert(eno == 0);

  eno = pthread_cond_wait(&ctx->pcv, &ctx->pmtx);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_unlock(&ctx->pmtx);
  rtems_test_assert(eno == 0);

  wait_for_worker(ctx);

  eno = pthread_rwlock_wrlock(&ctx->prw);
  rtems_test_assert(eno == 0);

  wait_for_worker(ctx);

  buf[0] = 'y';
  prio = 1;
  n = mq_receive(ctx->pmq, &buf[0], sizeof(buf), &prio);
  rtems_test_assert(n == (ssize_t) sizeof(buf));
  rtems_test_assert(buf[0] == 'x');
  rtems_test_assert(prio == 0);
#endif
}

static rtems_task Init(
  rtems_task_argument ignored
)
{
  test_context *ctx = &test_instance;

  TEST_BEGIN();

  puts( "Init - _Thread_queue_Extract - thread not blocked on a thread queue" );
  _Thread_queue_Extract( _Thread_Get_executing() );
  /* is there more to check? */

  test_context_init(ctx);
  test_classic_obj(ctx);
  test_posix_obj(ctx);

  rtems_test_assert( _Thread_queue_Is_empty( &queue.Queue ) );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS  2
#define CONFIGURE_MAXIMUM_SEMAPHORES  2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES  1
#define CONFIGURE_MAXIMUM_BARRIERS  1

#if defined(RTEMS_POSIX_API)
  #define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES 1
  #define CONFIGURE_MAXIMUM_POSIX_MUTEXES 1
  #define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES 1
  #define CONFIGURE_MAXIMUM_POSIX_RWLOCKS 1
  #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 1
  #define CONFIGURE_MESSAGE_BUFFER_MEMORY \
    (2 * CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(1, 1))
#else
  #define CONFIGURE_MESSAGE_BUFFER_MEMORY \
    CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(1, 1)
#endif

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
