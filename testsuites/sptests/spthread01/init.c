/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/thread.h>
#include <rtems.h>

#include <string.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPTHREAD 1";

#define WAKEUP_EVENT RTEMS_EVENT_0

typedef struct {
  rtems_mutex *mtx;
  rtems_recursive_mutex *rmtx;
  rtems_id calling_task;
} mutex_context;

static void mutex_task(rtems_task_argument arg)
{
  (void) arg;

  mutex_context *m;
  int rv;

  m = (mutex_context *) arg;

  rtems_test_assert(m->mtx != NULL || m->rmtx != NULL);

  if (m->mtx) {
    rv = rtems_mutex_try_lock(m->mtx);
    rtems_test_assert(rv == EBUSY);
  }

  if (m->rmtx) {
    rv = rtems_recursive_mutex_try_lock(m->rmtx);
    rtems_test_assert(rv == EBUSY);
  }

  rtems_event_send(m->calling_task, WAKEUP_EVENT);
}

static void test_try_lock_from_different_task(
  rtems_mutex *m,
  rtems_recursive_mutex *r
)
{
  mutex_context ctx;
  rtems_status_code sc;
  rtems_event_set e;
  rtems_id id;

  ctx.mtx = m;
  ctx.rmtx = r;
  ctx.calling_task = rtems_task_self();

  sc = rtems_task_create(
    rtems_build_name('M', 'T', 'X', ' '),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, mutex_task, (rtems_task_argument) &ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_receive(WAKEUP_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, 10, &e);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_delete(id);
}

static void test_mutex(void)
{
  rtems_mutex a = RTEMS_MUTEX_INITIALIZER("a");
  const char *name;
  int rv;

  name = rtems_mutex_get_name(&a);
  rtems_test_assert(strcmp(name, "a") == 0);

  rtems_mutex_set_name(&a, "b");

  name = rtems_mutex_get_name(&a);
  rtems_test_assert(strcmp(name, "b") == 0);

  rtems_mutex_destroy(&a);

  rtems_mutex_init(&a, "c");

  name = rtems_mutex_get_name(&a);
  rtems_test_assert(strcmp(name, "c") == 0);

  rtems_mutex_lock(&a);

  test_try_lock_from_different_task(&a, NULL);

  rtems_mutex_unlock(&a);

  rv = rtems_mutex_try_lock(&a);
  rtems_test_assert(rv == 0);

  rtems_mutex_unlock(&a);

  rtems_mutex_destroy(&a);
}

static void test_recursive_mutex(void)
{
  rtems_recursive_mutex a = RTEMS_RECURSIVE_MUTEX_INITIALIZER("a");
  const char *name;
  int rv;

  name = rtems_recursive_mutex_get_name(&a);
  rtems_test_assert(strcmp(name, "a") == 0);

  rtems_recursive_mutex_set_name(&a, "b");

  name = rtems_recursive_mutex_get_name(&a);
  rtems_test_assert(strcmp(name, "b") == 0);

  rtems_recursive_mutex_destroy(&a);

  rtems_recursive_mutex_init(&a, "c");

  name = rtems_recursive_mutex_get_name(&a);
  rtems_test_assert(strcmp(name, "c") == 0);

  rtems_recursive_mutex_lock(&a);

  rtems_recursive_mutex_lock(&a);

  rv = rtems_recursive_mutex_try_lock(&a);
  rtems_test_assert(rv == 0);

  test_try_lock_from_different_task(NULL, &a);

  rtems_recursive_mutex_unlock(&a);

  rtems_recursive_mutex_unlock(&a);

  rtems_recursive_mutex_unlock(&a);

  rtems_recursive_mutex_destroy(&a);
}

typedef struct {
  rtems_mutex mtx;
  rtems_condition_variable cnd;
} signal_context;

static void signal_task(rtems_task_argument arg)
{
  (void) arg;

  signal_context *s;

  s = (signal_context *) arg;
  rtems_mutex_lock(&s->mtx);
  rtems_condition_variable_signal(&s->cnd);
  rtems_mutex_unlock(&s->mtx);
}

static void test_condition_variable(void)
{
  rtems_condition_variable a = RTEMS_CONDITION_VARIABLE_INITIALIZER("a");
  signal_context s;
  const char *name;
  rtems_status_code sc;
  rtems_id id;

  name = rtems_condition_variable_get_name(&a);
  rtems_test_assert(strcmp(name, "a") == 0);

  rtems_condition_variable_set_name(&a, "b");

  name = rtems_condition_variable_get_name(&a);
  rtems_test_assert(strcmp(name, "b") == 0);

  rtems_condition_variable_destroy(&a);

  rtems_mutex_init(&s.mtx, "d");
  rtems_condition_variable_init(&s.cnd, "c");

  name = rtems_condition_variable_get_name(&s.cnd);
  rtems_test_assert(strcmp(name, "c") == 0);

  rtems_condition_variable_signal(&s.cnd);

  rtems_condition_variable_broadcast(&s.cnd);

  rtems_mutex_lock(&s.mtx);

  sc = rtems_task_create(
    rtems_build_name('C', 'O', 'N', 'D'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, signal_task, (rtems_task_argument) &s);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_condition_variable_wait(&s.cnd, &s.mtx);

  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_mutex_unlock(&s.mtx);

  rtems_condition_variable_destroy(&s.cnd);
  rtems_mutex_destroy(&s.mtx);
}

static void test_counting_semaphore(void)
{
  rtems_counting_semaphore a = RTEMS_COUNTING_SEMAPHORE_INITIALIZER("a", 1);
  const char *name;
  int eno;

  name = rtems_counting_semaphore_get_name(&a);
  rtems_test_assert(strcmp(name, "a") == 0);

  rtems_counting_semaphore_set_name(&a, "b");

  name = rtems_counting_semaphore_get_name(&a);
  rtems_test_assert(strcmp(name, "b") == 0);

  rtems_counting_semaphore_destroy(&a);

  rtems_counting_semaphore_init(&a, "c", 0);

  name = rtems_counting_semaphore_get_name(&a);
  rtems_test_assert(strcmp(name, "c") == 0);

  eno = rtems_counting_semaphore_try_wait(&a);
  rtems_test_assert(eno == EAGAIN);

  eno = rtems_counting_semaphore_wait_timed_ticks(&a, 1);
  rtems_test_assert(eno == ETIMEDOUT);

  rtems_counting_semaphore_post(&a);

  rtems_counting_semaphore_wait(&a);

  rtems_counting_semaphore_post(&a);

  eno = rtems_counting_semaphore_try_wait(&a);
  rtems_test_assert(eno == 0);

  rtems_counting_semaphore_post(&a);

  eno = rtems_counting_semaphore_wait_timed_ticks(&a, 1);
  rtems_test_assert(eno == 0);

  rtems_counting_semaphore_destroy(&a);
}

static void test_binary_semaphore(void)
{
  rtems_binary_semaphore a = RTEMS_BINARY_SEMAPHORE_INITIALIZER("a");
  const char *name;
  int eno;

  name = rtems_binary_semaphore_get_name(&a);
  rtems_test_assert(strcmp(name, "a") == 0);

  rtems_binary_semaphore_set_name(&a, "b");

  name = rtems_binary_semaphore_get_name(&a);
  rtems_test_assert(strcmp(name, "b") == 0);

  rtems_binary_semaphore_destroy(&a);

  rtems_binary_semaphore_init(&a, "c");

  name = rtems_binary_semaphore_get_name(&a);
  rtems_test_assert(strcmp(name, "c") == 0);

  eno = rtems_binary_semaphore_try_wait(&a);
  rtems_test_assert(eno == EAGAIN);

  eno = rtems_binary_semaphore_wait_timed_ticks(&a, 1);
  rtems_test_assert(eno == ETIMEDOUT);

  rtems_binary_semaphore_post(&a);

  rtems_binary_semaphore_wait(&a);

  rtems_binary_semaphore_post(&a);

  eno = rtems_binary_semaphore_try_wait(&a);
  rtems_test_assert(eno == 0);

  rtems_binary_semaphore_post(&a);

  eno = rtems_binary_semaphore_wait_timed_ticks(&a, 1);
  rtems_test_assert(eno == 0);

  rtems_binary_semaphore_destroy(&a);
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  test_mutex();
  test_recursive_mutex();
  test_condition_variable();
  test_counting_semaphore();
  test_binary_semaphore();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
