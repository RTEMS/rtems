/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#include <rtems/profiling.h>
#include <rtems/bspIo.h>
#include <rtems.h>

#include <stdio.h>
#include <string.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPPROFILING 1";

typedef struct {
  rtems_interrupt_lock a;
  rtems_interrupt_lock b;
  rtems_interrupt_lock c;
  rtems_interrupt_lock d;
  enum {
    WAIT_FOR_A,
    EXPECT_B,
    EXPECT_D,
    DONE
  } state;
} visitor_context;

static bool is_equal(const rtems_profiling_smp_lock *psl, const char *name)
{
  return strcmp(psl->name, name) == 0;
}

static void visitor(void *arg, const rtems_profiling_data *data)
{
  visitor_context *ctx = arg;

  if (data->header.type == RTEMS_PROFILING_SMP_LOCK) {
    const rtems_profiling_smp_lock *psl = &data->smp_lock;

    switch (ctx->state) {
      case WAIT_FOR_A:
        rtems_test_assert(!is_equal(psl, "b"));
        rtems_test_assert(!is_equal(psl, "c"));
        rtems_test_assert(!is_equal(psl, "d"));

        if (is_equal(psl, "a")) {
          ctx->state = EXPECT_B;
        }
        break;
      case EXPECT_B:
        rtems_test_assert(is_equal(psl, "b"));
        rtems_interrupt_lock_destroy(&ctx->c);
        ctx->state = EXPECT_D;
        break;
      case EXPECT_D:
        rtems_test_assert(is_equal(psl, "d"));
        ctx->state = DONE;
        break;
      case DONE:
        rtems_test_assert(!is_equal(psl, "a"));
        rtems_test_assert(!is_equal(psl, "b"));
        rtems_test_assert(!is_equal(psl, "c"));
        rtems_test_assert(!is_equal(psl, "d"));
        break;
    }
  }
}

static void lock_init(rtems_interrupt_lock *lock, const char *name)
{
  (void) lock;

  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_initialize(lock, name);
  rtems_interrupt_lock_acquire(lock, &lock_context);
  rtems_interrupt_lock_release(lock, &lock_context);
}

static void test_iterate(void)
{
  visitor_context ctx_instance;
  visitor_context *ctx = &ctx_instance;

  ctx->state = WAIT_FOR_A;
  lock_init(&ctx->a, "a");
  lock_init(&ctx->b, "b");
  lock_init(&ctx->c, "c");
  lock_init(&ctx->d, "d");

  rtems_profiling_iterate(visitor, ctx);

  rtems_interrupt_lock_destroy(&ctx->a);
  rtems_interrupt_lock_destroy(&ctx->b);

  if (ctx->state != DONE) {
    rtems_interrupt_lock_destroy(&ctx->c);
  }

  rtems_interrupt_lock_destroy(&ctx->d);
}

static void test_report_xml(void)
{
  rtems_status_code sc;
  int rv;

  sc = rtems_task_wake_after(3);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rv = rtems_profiling_report_xml("X", &rtems_test_printer, 1, "  ");
  printf("characters produced by rtems_profiling_report_xml(): %i\n", rv);
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  test_iterate();
  test_report_xml();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
