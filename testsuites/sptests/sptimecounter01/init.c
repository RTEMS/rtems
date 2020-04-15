/*
 * Copyright (c) 2015, 2019 embedded brains GmbH.  All rights reserved.
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

#include <assert.h>

#include <bsp/bootcard.h>

#include <rtems/score/timecounterimpl.h>
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

  ctx = &test_instance;
  tc = &ctx->tc;

  TEST_BEGIN();

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

  /* Ensure that the fraction overflows and the second remains constant */
  ctx->counter = (0xf0000000 | 1) + TEST_FREQ;
  rtems_bsd_binuptime(&bt);
  assert(ctx->counter == (0xf0000000 | 2) + TEST_FREQ);
  assert(bt.sec == 1);
  assert(bt.frac == 18446742522092);

  test_install(ctx);

  TEST_END();

  _Terminate(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_SCHEDULER_USER

#define CONFIGURE_SCHEDULER

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES { }

#define CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER 0

#define CONFIGURE_TASK_STACK_ALLOCATOR NULL

#define CONFIGURE_TASK_STACK_DEALLOCATOR NULL

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_IDLE_TASK_BODY NULL

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
