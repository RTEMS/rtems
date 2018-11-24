/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/score/objectimpl.h>
#include <rtems/sysinit.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPCONFIG 2";

static const rtems_name name = rtems_build_name('N', 'A', 'M', 'E');

static void test_barrier_create(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_barrier_create(name, RTEMS_DEFAULT_ATTRIBUTES, 1, &id);
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_barrier_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_barrier_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_BARRIERS, 1, 0);
  sc = rtems_barrier_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_barrier_get(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_barrier_wait(0, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_BARRIERS, 1, 0);
  sc = rtems_barrier_wait(id, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_message_queue_create(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_message_queue_create(
    name,
    1,
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_message_queue_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_message_queue_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_MESSAGE_QUEUES, 1, 0);
  sc = rtems_message_queue_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_message_queue_get(void)
{
  rtems_status_code sc;
  rtems_id id;
  uint32_t count;

  sc = rtems_message_queue_flush(0, &count);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_MESSAGE_QUEUES, 1, 0);
  sc = rtems_message_queue_flush(id, &count);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_partition_create(void)
{
  rtems_status_code sc;
  rtems_id id;
  long buf[32];

  sc = rtems_partition_create(
    name,
    buf,
    sizeof(buf),
    sizeof(buf),
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_partition_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_partition_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_PARTITIONS, 1, 0);
  sc = rtems_partition_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_partition_get(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_partition_return_buffer(0, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_PARTITIONS, 1, 0);
  sc = rtems_partition_return_buffer(id, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_rate_monotonic_create(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_rate_monotonic_create(name, &id);
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_rate_monotonic_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_rate_monotonic_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_PERIODS, 1, 0);
  sc = rtems_rate_monotonic_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_rate_monotonic_get(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_rate_monotonic_cancel(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_PERIODS, 1, 0);
  sc = rtems_rate_monotonic_cancel(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_region_create(void)
{
  rtems_status_code sc;
  rtems_id id;
  long buf[32];

  sc = rtems_region_create(
    name,
    buf,
    sizeof(buf),
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_region_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_region_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_REGIONS, 1, 0);
  sc = rtems_region_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_region_get(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_region_return_segment(0, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_REGIONS, 1, 0);
  sc = rtems_region_return_segment(id, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_semaphore_create(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_semaphore_create(
    name,
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    0,
    &id
  );
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_semaphore_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_semaphore_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_SEMAPHORES, 1, 0);
  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_semaphore_get(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_semaphore_release(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_SEMAPHORES, 1, 0);
  sc = rtems_semaphore_release(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_task_create(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_task_create(
    name,
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_task_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, 1, 0);
  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_task_get(void)
{
  rtems_status_code sc;
  rtems_id id;

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, 1, 0);
  sc = rtems_task_resume(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_timer_create(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_timer_create(name, &id);
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_timer_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_timer_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TIMERS, 1, 0);
  sc = rtems_timer_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_timer_get(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_timer_reset(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TIMERS, 1, 0);
  sc = rtems_timer_reset(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_user_extensions_create(void)
{
  rtems_status_code sc;
  rtems_id id;
  rtems_extensions_table table;

  memset(&table, 0, sizeof(table));
  sc = rtems_extension_create(name, &table, &id);
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_user_extensions_delete(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_extension_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_EXTENSIONS, 1, 0);
  sc = rtems_extension_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_id_to_name(rtems_id api, rtems_id cls, rtems_id idx, bool *found)
{
  rtems_status_code sc;
  rtems_id id;
  rtems_name name_of_id;

  id = rtems_build_id(api, cls, 1, idx);
  sc = rtems_object_get_classic_name(id, &name_of_id);

  if (sc == RTEMS_SUCCESSFUL) {
    if (name_of_id == rtems_build_name('U', 'I', '1', ' ')) {
      rtems_test_assert(id == rtems_task_self());
      rtems_test_assert(!found[0]);
      found[0] = true;
    } else {
      rtems_test_assert(name_of_id == rtems_build_name('I', 'D', 'L', 'E'));
      rtems_test_assert(!found[1]);
      found[1] = true;
    }
  } else {
    rtems_test_assert(sc == RTEMS_INVALID_ID);
  }
}

static void test_some_id_to_name(void)
{
  rtems_id api;
  bool found[2];

  found[0] = false;
  found[1] = false;

  for (api = 0; api < 8; ++api) {
    rtems_id cls;

    for (cls = 0; cls < 32; ++cls) {
      test_id_to_name(api, cls, 0, found);
      test_id_to_name(api, cls, 1, found);
      test_id_to_name(api, cls, 2, found);
      test_id_to_name(api, cls, 65534, found);
      test_id_to_name(api, cls, 65535, found);
    }
  }

  rtems_test_assert(found[0]);
  rtems_test_assert(found[1]);
}

static void Init(rtems_task_argument arg)
{
  test_barrier_create();
  test_barrier_delete();
  test_barrier_get();
  test_message_queue_create();
  test_message_queue_delete();
  test_message_queue_get();
  test_partition_create();
  test_partition_delete();
  test_partition_get();
  test_rate_monotonic_create();
  test_rate_monotonic_delete();
  test_rate_monotonic_get();
  test_region_create();
  test_region_delete();
  test_region_get();
  test_semaphore_create();
  test_semaphore_delete();
  test_semaphore_get();
  test_task_create();
  test_task_delete();
  test_task_get();
  test_timer_create();
  test_timer_delete();
  test_timer_get();
  test_user_extensions_create();
  test_user_extensions_delete();
  test_some_id_to_name();
  TEST_END();
  rtems_test_exit(0);
}

static void before_object_information_initialization(void)
{
  rtems_print_printer_printk(&rtems_test_printer);
  TEST_BEGIN();
  test_barrier_get();
  test_message_queue_get();
  test_partition_get();
  test_rate_monotonic_get();
  test_semaphore_get();
  test_task_get();
  test_timer_get();
}

RTEMS_SYSINIT_ITEM(
  before_object_information_initialization,
  RTEMS_SYSINIT_INITIAL_EXTENSIONS,
  RTEMS_SYSINIT_ORDER_LAST
);

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
