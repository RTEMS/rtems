/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <errno.h>
#include <pthread.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "SMPPSXMUTEX 1";

#define SCHED_A rtems_build_name(' ', ' ', ' ', 'A')

#define SCHED_B rtems_build_name(' ', ' ', ' ', 'B')

typedef struct {
  pthread_t thread_b;
  pthread_mutexattr_t mtx_attr;
  pthread_mutex_t mtx_a;
  pthread_mutex_t mtx_b;
} test_context;

static test_context test_instance;

static void *thread_b(void *arg)
{
  test_context *ctx;
  rtems_id scheduler_b_id;
  rtems_status_code sc;
  rtems_task_priority prio;
  int prio_ceiling;
  int eno;

  ctx = arg;

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  sc = rtems_scheduler_ident(SCHED_B, &scheduler_b_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_priority(pthread_self(), RTEMS_CURRENT_PRIORITY, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_set_scheduler(pthread_self(), scheduler_b_id, prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(rtems_scheduler_get_processor() == 1);

  eno = pthread_mutex_init(&ctx->mtx_b, &ctx->mtx_attr);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_getprioceiling(&ctx->mtx_b, &prio_ceiling);
  rtems_test_assert(eno == 0);
  rtems_test_assert(prio_ceiling == 254);

  eno = pthread_mutex_lock(&ctx->mtx_b);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_unlock(&ctx->mtx_b);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_destroy(&ctx->mtx_b);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_getprioceiling(&ctx->mtx_a, &prio_ceiling);
  rtems_test_assert(eno == 0);
  rtems_test_assert(prio_ceiling == 126);

  eno = pthread_mutex_lock(&ctx->mtx_a);
  rtems_test_assert(eno == EINVAL);

  return ctx;
}

static void test(test_context *ctx)
{
  uint32_t cpu_count;
  int prio_ceiling;
  int eno;

  cpu_count = rtems_scheduler_get_processor_maximum();

  rtems_test_assert(rtems_scheduler_get_processor() == 0);

  eno = pthread_mutexattr_init(&ctx->mtx_attr);
  rtems_test_assert(eno == 0);

  eno = pthread_mutexattr_setprotocol(&ctx->mtx_attr, PTHREAD_PRIO_PROTECT);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_init(&ctx->mtx_a, &ctx->mtx_attr);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_getprioceiling(&ctx->mtx_a, &prio_ceiling);
  rtems_test_assert(eno == 0);
  rtems_test_assert(prio_ceiling == 126);

  eno = pthread_mutex_lock(&ctx->mtx_a);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_unlock(&ctx->mtx_a);
  rtems_test_assert(eno == 0);

  if (cpu_count > 1) {
    void *exit_code;

    eno = pthread_create(&ctx->thread_b, NULL, thread_b, ctx);
    rtems_test_assert(eno == 0);

    exit_code = NULL;
    eno = pthread_join(ctx->thread_b, &exit_code);
    rtems_test_assert(eno == 0);
    rtems_test_assert(exit_code == ctx);
  }

  eno = pthread_mutex_destroy(&ctx->mtx_a);
  rtems_test_assert(eno == 0);

  eno = pthread_mutexattr_destroy(&ctx->mtx_attr);
  rtems_test_assert(eno == 0);
}

static void *POSIX_Init(void *arg)
{
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();

  rtems_resource_snapshot_take(&snapshot);

  test(&test_instance);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_SCHEDULER_PRIORITY_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_PRIORITY_SMP(a, 128);

RTEMS_SCHEDULER_PRIORITY_SMP(b, 256);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_PRIORITY_SMP(a, SCHED_A), \
  RTEMS_SCHEDULER_TABLE_PRIORITY_SMP(b, SCHED_B)  \

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
