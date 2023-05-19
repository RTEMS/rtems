/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2015, 2019 embedded brains GmbH & Co. KG
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

#include "tmacros.h"

#include <assert.h>

#include <bsp/bootcard.h>

#include <rtems/score/timecounterimpl.h>
#include <rtems/score/percpu.h>
#include <rtems/score/todimpl.h>
#include <rtems/timecounter.h>
#include <rtems/bsd.h>

const char rtems_test_name[] = "SPTIMECOUNTER 1";

#define TEST_QUAL 1234

#define TEST_FREQ 1000000

typedef struct {
  struct timecounter tc;
  uint32_t counter;
  struct timecounter tc_2;
  uint32_t counter_2;
} test_context;

static test_context test_instance;

static Thread_Control executing;

static uint32_t test_get_timecount(struct timecounter *tc)
{
  test_context *ctx;

  ctx = RTEMS_CONTAINER_OF(tc, test_context, tc);
  ++ctx->counter;

  return ctx->counter;
}

static uint32_t test_get_timecount_2(struct timecounter *tc)
{
  test_context *ctx;

  ctx = RTEMS_CONTAINER_OF(tc, test_context, tc_2);
  ++ctx->counter_2;

  return ctx->counter_2;
}

static void test_install(test_context *ctx)
{
  struct timecounter *tc;
  struct timecounter *tc_2;
  uint32_t c;
  uint32_t c_2;

  tc = &ctx->tc;
  tc_2 = &ctx->tc_2;
  c = ctx->counter;
  c_2 = ctx->counter_2;

  tc_2->tc_get_timecount = test_get_timecount_2;
  tc_2->tc_counter_mask = 0x0fffffff;
  tc_2->tc_frequency = TEST_FREQ - 1;
  tc_2->tc_quality = TEST_QUAL;
  _Timecounter_Install(tc_2);
  assert(ctx->counter == c);
  assert(ctx->counter_2 == c_2);

  tc_2->tc_get_timecount = test_get_timecount_2;
  tc_2->tc_counter_mask = 0x0fffffff;
  tc_2->tc_frequency = TEST_FREQ - 1;
  tc_2->tc_quality = TEST_QUAL + 1;
  _Timecounter_Install(tc_2);
  assert(ctx->counter == c + 1);
  assert(ctx->counter_2 == c_2 + 1);

  tc->tc_get_timecount = test_get_timecount;
  tc->tc_counter_mask = 0x0fffffff;
  tc->tc_frequency = TEST_FREQ;
  tc->tc_quality = TEST_QUAL + 1;
  _Timecounter_Install(tc);
  assert(ctx->counter == c + 2);
  assert(ctx->counter_2 == c_2 + 2);
}

void boot_card(const char *cmdline)
{
  test_context *ctx;
  struct timecounter *tc;
  struct bintime bt;
  struct timeval tv;
  struct timespec ts;
  Per_CPU_Control *cpu_self;

  ctx = &test_instance;
  tc = &ctx->tc;

  TEST_BEGIN();

  cpu_self = _Per_CPU_Get();
  cpu_self->executing = &executing;
  cpu_self->heir = &executing;

  assert(time(NULL) == TOD_SECONDS_1970_THROUGH_1988);

  rtems_bsd_bintime(&bt);
  assert(bt.sec == TOD_SECONDS_1970_THROUGH_1988);
  assert(bt.frac == 0);

  rtems_bsd_getbintime(&bt);
  assert(bt.sec == TOD_SECONDS_1970_THROUGH_1988);
  assert(bt.frac == 0);

  rtems_bsd_microtime(&tv);
  assert(tv.tv_sec == TOD_SECONDS_1970_THROUGH_1988);
  assert(tv.tv_usec == 0);

  rtems_bsd_getmicrotime(&tv);
  assert(tv.tv_sec == TOD_SECONDS_1970_THROUGH_1988);
  assert(tv.tv_usec == 0);

  rtems_bsd_nanotime(&ts);
  assert(ts.tv_sec == TOD_SECONDS_1970_THROUGH_1988);
  assert(ts.tv_nsec == 0);

  rtems_bsd_getnanotime(&ts);
  assert(ts.tv_sec == TOD_SECONDS_1970_THROUGH_1988);
  assert(ts.tv_nsec == 0);

  assert(rtems_clock_get_uptime_seconds() == 0);
  assert(rtems_clock_get_uptime_nanoseconds() == 0);

  rtems_bsd_binuptime(&bt);
  assert(bt.sec == 1);
  assert(bt.frac == 0);

  rtems_bsd_getbinuptime(&bt);
  assert(bt.sec == 1);
  assert(bt.frac == 0);

  rtems_bsd_microuptime(&tv);
  assert(tv.tv_sec == 1);
  assert(tv.tv_usec == 0);

  rtems_bsd_getmicrouptime(&tv);
  assert(tv.tv_sec == 1);
  assert(tv.tv_usec == 0);

  rtems_bsd_nanouptime(&ts);
  assert(ts.tv_sec == 1);
  assert(ts.tv_nsec == 0);

  rtems_bsd_getnanouptime(&ts);
  assert(ts.tv_sec == 1);
  assert(ts.tv_nsec == 0);

  /* On RTEMS time does not advance using the dummy timecounter */
  rtems_bsd_binuptime(&bt);
  assert(bt.sec == 1);
  assert(bt.frac == 0);

  rtems_timecounter_tick();
  rtems_bsd_binuptime(&bt);
  assert(bt.sec == 1);
  assert(bt.frac == 0);

  ctx->counter = 0;
  tc->tc_get_timecount = test_get_timecount;
  tc->tc_counter_mask = 0x0fffffff;
  tc->tc_frequency = TEST_FREQ;
  tc->tc_quality = TEST_QUAL;
  _Timecounter_Install(tc);
  assert(ctx->counter == 1);

  rtems_bsd_binuptime(&bt);
  assert(ctx->counter == 2);

  assert(bt.sec == 1);
  assert(bt.frac == 18446744073708);

  ctx->counter = 0xf0000000 | 1;
  rtems_bsd_binuptime(&bt);
  assert(ctx->counter == (0xf0000000 | 2));

  assert(bt.sec == 1);
  assert(bt.frac == 18446744073708);

  /* Check that a large delta yields a correct time */
  ctx->counter = (0xf0000000 | 1) + TEST_FREQ;
  rtems_bsd_binuptime(&bt);
  assert(ctx->counter == (0xf0000000 | 2) + TEST_FREQ);
  assert(bt.sec == 2);
  assert(bt.frac == 18446742522092);

  test_install(ctx);

  TEST_END();

  _Terminate(RTEMS_FATAL_SOURCE_EXIT, 0);
}

static void *stack_allocate(size_t size)
{
  (void) size;
  return NULL;
}

static void stack_free(void *ptr)
{
  (void) ptr;
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_SCHEDULER_USER

#define CONFIGURE_SCHEDULER

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES { }

#define CONFIGURE_TASK_STACK_ALLOCATOR stack_allocate

#define CONFIGURE_TASK_STACK_DEALLOCATOR stack_free

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_IDLE_TASK_BODY NULL

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
