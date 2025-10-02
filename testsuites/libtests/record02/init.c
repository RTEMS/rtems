/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2018, 2024 embedded brains GmbH & Co. KG
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

#include <rtems/record.h>
#include <rtems/recordclient.h>
#include <rtems.h>

#include <string.h>

#include "tmacros.h"

const char rtems_test_name[] = "RECORD 2";

typedef struct {
  rtems_record_client_context client;
} test_context;

static test_context test_instance;

static rtems_record_client_status client_handler(
  uint64_t            bt,
  uint32_t            cpu,
  rtems_record_event  event,
  uint64_t            data,
  void               *arg
)
{
  uint32_t seconds;
  uint32_t nanoseconds;

  (void) arg;

  if (event == RTEMS_RECORD_USER_5) {
    return RTEMS_RECORD_CLIENT_SUCCESS;
  }

  rtems_record_client_bintime_to_seconds_and_nanoseconds(
    bt,
    &seconds,
    &nanoseconds
  );
  printf(
    "%" PRIu32 ".%09" PRIu32 ":"
    "%" PRIu32 ":%s:%" PRIx64 "\n",
    seconds,
    nanoseconds,
    cpu,
    rtems_record_event_text( event ),
    data
  );

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static void wait(void)
{
  int i;

  for (i = 0; i < 10; ++i) {
    rtems_task_wake_after(1);
  }
}

static void generate_events(void)
{
  uint32_t level;

  rtems_record_line();
  rtems_record_line_2(RTEMS_RECORD_USER_0, 0);
  rtems_record_line_3(RTEMS_RECORD_USER_1, 1, RTEMS_RECORD_USER_2, 2);
  rtems_record_line_arg(0);
  rtems_record_line_arg_2(0, 1);
  rtems_record_line_arg_3(0, 1, 2);
  rtems_record_line_arg_4(0, 1, 2, 3);
  rtems_record_line_arg_5(0, 1, 2, 3, 4);
  rtems_record_line_arg_6(0, 1, 2, 3, 4, 5);
  rtems_record_line_arg_7(0, 1, 2, 3, 4, 5, 6);
  rtems_record_line_arg_8(0, 1, 2, 3, 4, 5, 6, 7);
  rtems_record_line_arg_9(0, 1, 2, 3, 4, 5, 6, 7, 8);
  rtems_record_line_arg_10(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  rtems_record_caller();
  rtems_record_caller_3(RTEMS_RECORD_USER_0, 0);
  rtems_record_caller_4(RTEMS_RECORD_USER_1, 1, RTEMS_RECORD_USER_2, 2);
  rtems_record_caller_arg(0);
  rtems_record_caller_arg_2(0, 1);
  rtems_record_caller_arg_3(0, 1, 2);
  rtems_record_caller_arg_4(0, 1, 2, 3);
  rtems_record_caller_arg_5(0, 1, 2, 3, 4);
  rtems_record_caller_arg_6(0, 1, 2, 3, 4, 5);
  rtems_record_caller_arg_7(0, 1, 2, 3, 4, 5, 6);
  rtems_record_caller_arg_8(0, 1, 2, 3, 4, 5, 6, 7);
  rtems_record_caller_arg_9(0, 1, 2, 3, 4, 5, 6, 7, 8);
  rtems_record_caller_arg_10(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  rtems_record_entry(RTEMS_RECORD_USER_3);
  rtems_record_entry_1(RTEMS_RECORD_USER_3, 0);
  rtems_record_entry_2(RTEMS_RECORD_USER_3, 0, 1);
  rtems_record_entry_3(RTEMS_RECORD_USER_3, 0, 1, 2);
  rtems_record_entry_4(RTEMS_RECORD_USER_3, 0, 1, 2, 3);
  rtems_record_entry_5(RTEMS_RECORD_USER_3, 0, 1, 2, 3, 4);
  rtems_record_entry_6(RTEMS_RECORD_USER_3, 0, 1, 2, 3, 4, 5);
  rtems_record_entry_7(RTEMS_RECORD_USER_3, 0, 1, 2, 3, 4, 5, 6);
  rtems_record_entry_8(RTEMS_RECORD_USER_3, 0, 1, 2, 3, 4, 5, 6, 7);
  rtems_record_entry_9(RTEMS_RECORD_USER_3, 0, 1, 2, 3, 4, 5, 6, 7, 8);
  rtems_record_entry_10(RTEMS_RECORD_USER_3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  rtems_record_exit(RTEMS_RECORD_USER_4);
  rtems_record_exit_1(RTEMS_RECORD_USER_4, 0);
  rtems_record_exit_2(RTEMS_RECORD_USER_4, 0, 1);
  rtems_record_exit_3(RTEMS_RECORD_USER_4, 0, 1, 2);
  rtems_record_exit_4(RTEMS_RECORD_USER_4, 0, 1, 2, 3);
  rtems_record_exit_5(RTEMS_RECORD_USER_4, 0, 1, 2, 3, 4);
  rtems_record_exit_6(RTEMS_RECORD_USER_4, 0, 1, 2, 3, 4, 5);
  rtems_record_exit_7(RTEMS_RECORD_USER_4, 0, 1, 2, 3, 4, 5, 6);
  rtems_record_exit_8(RTEMS_RECORD_USER_4, 0, 1, 2, 3, 4, 5, 6, 7);
  rtems_record_exit_9(RTEMS_RECORD_USER_4, 0, 1, 2, 3, 4, 5, 6, 7, 8);
  rtems_record_exit_10(RTEMS_RECORD_USER_4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  level = rtems_record_interrupt_disable();
  rtems_record_interrupt_enable(level);
}

static void fetch(test_context *ctx)
{
  rtems_record_client_status cs;
  rtems_record_fetch_control control;
  rtems_record_item          items[256];
  rtems_record_fetch_status  fs;

  rtems_record_fetch_initialize(
    &control,
    &items[0],
    RTEMS_ARRAY_SIZE( items )
  );

  do {
    fs = rtems_record_fetch(&control);
    cs = rtems_record_client_run(
      &ctx->client,
      control.fetched_items,
      control.fetched_count * sizeof(*control.fetched_items)
    );
    rtems_test_assert(cs == RTEMS_RECORD_CLIENT_SUCCESS);
  } while (fs == RTEMS_RECORD_FETCH_CONTINUE);
}

static void overflow(void)
{
  int i;

  for (i = 0; i < 512; ++i) {
    rtems_record_produce(RTEMS_RECORD_USER_5, 0);
  }

  rtems_record_produce(RTEMS_RECORD_USER_6, 0);
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  test_context *ctx;
  Record_Stream_header header;
  size_t size;
  rtems_record_client_status cs;
  rtems_interrupt_level level;

  TEST_BEGIN();
  ctx = &test_instance;

  wait();
  generate_events();

  cs = rtems_record_client_init(&ctx->client, client_handler, NULL);
  rtems_test_assert(cs == RTEMS_RECORD_CLIENT_SUCCESS);
  size = _Record_Stream_header_initialize(&header);
  cs = rtems_record_client_run(&ctx->client, &header, size);
  rtems_test_assert(cs == RTEMS_RECORD_CLIENT_SUCCESS);
  fetch(ctx);

  rtems_interrupt_local_disable(level);
  overflow();
  fetch(ctx);
  rtems_interrupt_local_enable(level);

  rtems_record_client_destroy(&ctx->client);

  wait();
  generate_events();
  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, 0 );
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  generate_events();
  _Record_Fatal_dump_base64(source, always_set_to_false, code);
  TEST_END();
}

#define INITIAL_EXTENSION \
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, fatal_extension, NULL }

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS \
  INITIAL_EXTENSION, RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_RECORD_PER_PROCESSOR_ITEMS 512

#define CONFIGURE_RECORD_EXTENSIONS_ENABLED

#define CONFIGURE_RECORD_INTERRUPTS_ENABLED

#define CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
