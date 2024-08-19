/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#include <rtems/record.h>

#include <rtems.h>
#include <string.h>

#include <rtems/test-info.h>
#include <rtems/test.h>

#define ITEM_CAPACITY 512

#define RECOMMENDED_ITEM_COUNT (ITEM_CAPACITY + 2)

typedef struct {
  bool early;
  bool late;
  bool interrupt;
  bool good;
  uint32_t how_many;
  rtems_record_item items[RECOMMENDED_ITEM_COUNT];
} RecordFetchConcurrentContext;

static void action(void *arg) {
  RecordFetchConcurrentContext *ctx;
  rtems_record_fetch_control control;

  ctx = arg;
  rtems_record_fetch_initialize(&control, &ctx->items[0],
                                RTEMS_ARRAY_SIZE(ctx->items));
  ctx->early = false;
  (void)rtems_record_fetch(&control);
  ctx->late = true;

  if (control.fetched_count < ITEM_CAPACITY) {
    size_t i;

    ctx->good = true;
    T_eq_u32(RTEMS_RECORD_GET_EVENT(control.fetched_items[0].event),
             RTEMS_RECORD_PROCESSOR);
    T_eq_ulong(control.fetched_items[0].data, 0);
    T_eq_u32(RTEMS_RECORD_GET_EVENT(control.fetched_items[1].event),
             RTEMS_RECORD_PER_CPU_OVERFLOW);

    for (i = 2; i < control.fetched_count; ++i) {
      rtems_record_event event;

      event = RTEMS_RECORD_GET_EVENT(control.fetched_items[i].event);
      T_true(event == RTEMS_RECORD_USER_0 || event == RTEMS_RECORD_UPTIME_LOW ||
             event == RTEMS_RECORD_UPTIME_HIGH);
    }
  }
}

static void prepare(void *arg) {
  RecordFetchConcurrentContext *ctx;
  uint32_t i;

  ctx = arg;
  ctx->early = true;
  ctx->late = false;
  ctx->interrupt = false;

  for (i = 0; i < RECOMMENDED_ITEM_COUNT - 2; ++i) {
    rtems_record_produce(RTEMS_RECORD_USER_0, 0);
  }
}

static T_interrupt_test_state interrupt(void *arg) {
  RecordFetchConcurrentContext *ctx;

  ctx = arg;

  if (ctx->good) {
    return T_INTERRUPT_TEST_DONE;
  }

  if (ctx->early) {
    return T_INTERRUPT_TEST_EARLY;
  }

  if (ctx->late) {
    return T_INTERRUPT_TEST_LATE;
  }

  if (!ctx->interrupt) {
    uint32_t i;

    for (i = 0; i < ctx->how_many; ++i) {
      rtems_record_produce(RTEMS_RECORD_USER_1, 0);
    }
  }

  ctx->interrupt = true;
  return T_INTERRUPT_TEST_CONTINUE;
}

static const T_interrupt_test_config config = {.prepare = prepare,
                                               .action = action,
                                               .interrupt = interrupt,
                                               .max_iteration_count = 10000};

T_TEST_CASE(RecordFetchConcurrent) {
  RecordFetchConcurrentContext ctx;

  memset(&ctx, 0, sizeof(ctx));
  ctx.how_many = ITEM_CAPACITY + 1;
  T_interrupt_test(&config, &ctx);
  T_true(ctx.good);

  memset(&ctx, 0, sizeof(ctx));
  ctx.how_many = ITEM_CAPACITY / 2;
  T_interrupt_test(&config, &ctx);
  T_true(ctx.good);
}

const char rtems_test_name[] = "RECORD 4";

static void Init(rtems_task_argument argument) {
  rtems_test_run(argument, TEST_STATE);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RECORD_PER_PROCESSOR_ITEMS 512

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
