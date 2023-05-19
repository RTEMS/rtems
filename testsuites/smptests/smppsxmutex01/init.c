/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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
  int prio_ceiling;
  int eno;

  ctx = arg;

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
    rtems_id scheduler_a_id;
    rtems_id scheduler_b_id;
    rtems_status_code sc;
    rtems_task_priority prio;
    void *exit_code;

    sc = rtems_scheduler_ident(SCHED_A, &scheduler_a_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_scheduler_ident(SCHED_B, &scheduler_b_id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_priority(pthread_self(), RTEMS_CURRENT_PRIORITY, &prio);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_set_scheduler(pthread_self(), scheduler_b_id, prio);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    eno = pthread_create(&ctx->thread_b, NULL, thread_b, ctx);
    rtems_test_assert(eno == 0);

    exit_code = NULL;
    eno = pthread_join(ctx->thread_b, &exit_code);
    rtems_test_assert(eno == 0);
    rtems_test_assert(exit_code == ctx);

    sc = rtems_task_set_scheduler(pthread_self(), scheduler_a_id, prio);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
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
