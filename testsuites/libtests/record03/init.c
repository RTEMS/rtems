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

#include <rtems/test.h>
#include <rtems/test-info.h>

#define CHECK_ITEM(item, expected_event, expected_data)                        \
  do {                                                                         \
    T_eq_u32(RTEMS_RECORD_GET_EVENT((item)->event), expected_event);           \
    T_eq_ulong((item)->data, expected_data);                                   \
  } while (0)

#ifdef RTEMS_SMP
#define COUNT 17
#else
#define COUNT 18
#endif

static void
check_other_cpu(rtems_record_fetch_control *control) {
#ifdef RTEMS_SMP
  rtems_record_fetch_status status;
  rtems_record_item items_2[3];
  rtems_record_item *storage_items;
  size_t storage_item_count;

  memset(&items_2, 0xff, sizeof(items_2));
  storage_items = control->internal.storage_items;
  storage_item_count = control->internal.storage_item_count;
  control->internal.storage_items = items_2;
  control->internal.storage_item_count = RTEMS_ARRAY_SIZE(items_2);
  status = rtems_record_fetch(control);
  T_eq_int(status, RTEMS_RECORD_FETCH_CONTINUE);
  T_eq_ptr(control->fetched_items, &items_2[1]);
  T_eq_sz(control->fetched_count, 1);
  CHECK_ITEM(&items_2[1], RTEMS_RECORD_PROCESSOR, 0);
  control->internal.storage_items = storage_items;
  control->internal.storage_item_count = storage_item_count;
#else
  (void)control;
#endif
}

static uint32_t set_affinity(uint32_t cpu) {
#ifdef RTEMS_SMP
  rtems_status_code sc;
  cpu_set_t set;

  CPU_ZERO(&set);
  CPU_SET((int)cpu, &set);
  sc = rtems_task_set_affinity(RTEMS_SELF, sizeof(set), &set);
  T_rsc_success(sc);
#else
  cpu = 0;
#endif
  return cpu;
}

T_TEST_CASE(RecordFetch) {
  rtems_record_fetch_control control;
  rtems_record_fetch_status status;
  rtems_record_item items[COUNT];
  uint32_t i;
  uint32_t cpu;

  T_eq_sz(rtems_record_get_item_count_for_fetch(), RTEMS_ARRAY_SIZE(items));

  memset(&control, 0xff, sizeof(control));
  rtems_record_fetch_initialize(&control, items, RTEMS_ARRAY_SIZE(items));
#ifdef RTEMS_SMP
  T_eq_u32(control.internal.cpu_index, 0);
#endif
  T_eq_sz(control.internal.cpu_todo, 0);
  T_null(control.fetched_items);
  T_eq_sz(control.fetched_count, 0);

  control.internal.storage_item_count = 2;
  status = rtems_record_fetch(&control);
  control.internal.storage_item_count = RTEMS_ARRAY_SIZE(items);
  T_eq_int(status, RTEMS_RECORD_FETCH_INVALID_ITEM_COUNT);

  cpu = set_affinity(0);

  /*
   * Fetch the initial uptime events produced by
   * _Record_Initialize_watchdogs().
   */
  memset(&items, 0xff, sizeof(items));
  status = rtems_record_fetch(&control);
#ifdef RTEMS_SMP
  T_eq_int(status, RTEMS_RECORD_FETCH_CONTINUE);
#else
  T_eq_int(status, RTEMS_RECORD_FETCH_DONE);
#endif
  T_eq_ptr(control.fetched_items, &items[1]);
  T_eq_sz(control.fetched_count, 3);
  CHECK_ITEM(&items[1], RTEMS_RECORD_PROCESSOR, 0);
  CHECK_ITEM(&items[2], RTEMS_RECORD_UPTIME_LOW, 0);
  CHECK_ITEM(&items[3], RTEMS_RECORD_UPTIME_HIGH, 1);
#ifdef RTEMS_SMP
  status = rtems_record_fetch(&control);
  T_eq_int(status, RTEMS_RECORD_FETCH_DONE);
  T_eq_ptr(control.fetched_items, &items[1]);
  T_eq_sz(control.fetched_count, 1);
  CHECK_ITEM(&items[1], RTEMS_RECORD_PROCESSOR, 1);
#endif

  cpu = set_affinity(1);

  /* Fetch with no events available */
  memset(&items, 0xff, sizeof(items));
  check_other_cpu(&control);
  status = rtems_record_fetch(&control);
  T_eq_int(status, RTEMS_RECORD_FETCH_DONE);
  T_eq_ptr(control.fetched_items, &items[1]);
  T_eq_sz(control.fetched_count, 1);
  CHECK_ITEM(&items[1], RTEMS_RECORD_PROCESSOR, cpu);

  /* Fetch with overflow */

  for (i = 0; i < COUNT - 1; ++i) {
    rtems_record_produce(2 * i, 2 * i + 1);
  }

  memset(&items, 0xff, sizeof(items));
  check_other_cpu(&control);
  control.internal.storage_item_count = 4;
  status = rtems_record_fetch(&control);
  control.internal.storage_item_count = RTEMS_ARRAY_SIZE(items);
  T_eq_int(status, RTEMS_RECORD_FETCH_CONTINUE);
  T_eq_ptr(control.fetched_items, &items[0]);
  T_eq_sz(control.fetched_count, 4);
  CHECK_ITEM(&items[0], RTEMS_RECORD_PROCESSOR, cpu);
  CHECK_ITEM(&items[1], RTEMS_RECORD_PER_CPU_OVERFLOW, 1);
  CHECK_ITEM(&items[2], 2, 3);
  CHECK_ITEM(&items[3], 4, 5);

  status = rtems_record_fetch(&control);
  T_eq_int(status, RTEMS_RECORD_FETCH_DONE);
  T_eq_ptr(control.fetched_items, &items[1]);
  T_eq_sz(control.fetched_count, COUNT - 3);
  CHECK_ITEM(&items[1], RTEMS_RECORD_PROCESSOR, cpu);

  for (i = 0; i < COUNT - 4; ++i) {
    CHECK_ITEM(&items[i + 2], 2 * i + 6, 2 * i + 7);
  }
}

const char rtems_test_name[] = "RECORD 3";

static void Init(rtems_task_argument argument)
{
  rtems_test_run(argument, TEST_STATE);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_RECORD_PER_PROCESSOR_ITEMS 16

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
