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

#include <tmacros.h>

const char rtems_test_name[] = "SPCONFIG 2";

static const rtems_name name = rtems_build_name('N', 'A', 'M', 'E');

static void test_barrier(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_barrier_create(name, RTEMS_DEFAULT_ATTRIBUTES, 1, &id);
  rtems_test_assert(sc == RTEMS_TOO_MANY);
}

static void test_message_queue(void)
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

  sc = rtems_message_queue_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_MESSAGE_QUEUES, 1, 0);
  sc = rtems_message_queue_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_partition(void)
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

  sc = rtems_partition_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_PARTITIONS, 1, 0);
  sc = rtems_partition_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_rate_monotonic(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_rate_monotonic_create(name, &id);
  rtems_test_assert(sc == RTEMS_TOO_MANY);

  sc = rtems_rate_monotonic_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_PERIODS, 1, 0);
  sc = rtems_rate_monotonic_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_region(void)
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

  sc = rtems_region_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_REGIONS, 1, 0);
  sc = rtems_region_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_semaphore(void)
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

  sc = rtems_semaphore_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_SEMAPHORES, 1, 0);
  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_task(void)
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

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, 1, 0);
  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_timer(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_timer_create(name, &id);
  rtems_test_assert(sc == RTEMS_TOO_MANY);

  sc = rtems_timer_delete(0);
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  id = rtems_build_id(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TIMERS, 1, 0);
  sc = rtems_timer_delete(id);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_user_extensions(void)
{
  rtems_status_code sc;
  rtems_id id;
  rtems_extensions_table table;

  memset(&table, 0, sizeof(table));
  sc = rtems_extension_create(name, &table, &id);
  rtems_test_assert(sc == RTEMS_TOO_MANY);

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
  rtems_print_printer_printk(&rtems_test_printer);
  TEST_BEGIN();
  test_barrier();
  test_message_queue();
  test_partition();
  test_rate_monotonic();
  test_region();
  test_semaphore();
  test_task();
  test_timer();
  test_user_extensions();
  test_some_id_to_name();
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
