/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

typedef struct {
  struct timecounter tc_soft;
  u_int tc_soft_counter;
} test_context;

static test_context test_instance;

static uint32_t test_get_timecount_soft(struct timecounter *tc)
{
  test_context *ctx = tc->tc_priv;

  ++ctx->tc_soft_counter;

  return ctx->tc_soft_counter;
}

void boot_card(const char *cmdline)
{
  test_context *ctx = &test_instance;
  struct timecounter *tc_soft = &ctx->tc_soft;
  uint64_t soft_freq = 1000000;
  struct bintime bt;
  struct timeval tv;
  struct timespec ts;

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

  ctx->tc_soft_counter = 0;
  tc_soft->tc_get_timecount = test_get_timecount_soft;
  tc_soft->tc_counter_mask = 0x0fffffff;
  tc_soft->tc_frequency = soft_freq;
  tc_soft->tc_quality = 1234;
  tc_soft->tc_priv = ctx;
  _Timecounter_Install(tc_soft);
  assert(ctx->tc_soft_counter == 3);

  rtems_bsd_binuptime(&bt);
  assert(ctx->tc_soft_counter == 4);

  assert(bt.sec == 1);
  assert(bt.frac == 18446744073708);

  ctx->tc_soft_counter = 0xf0000000 | 3;
  rtems_bsd_binuptime(&bt);
  assert(ctx->tc_soft_counter == (0xf0000000 | 4));

  assert(bt.sec == 1);
  assert(bt.frac == 18446744073708);

  /* Ensure that the fraction overflows and the second remains constant */
  ctx->tc_soft_counter = (0xf0000000 | 3) + soft_freq;
  rtems_bsd_binuptime(&bt);
  assert(ctx->tc_soft_counter == (0xf0000000 | 4) + soft_freq);
  assert(bt.sec == 1);
  assert(bt.frac == 18446742522092);

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
