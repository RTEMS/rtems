/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include <stdio.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTLS 1";

static rtems_id master_task;

static __thread volatile char tls_item = 123;

static volatile uint32_t read_write_small = 0xdeadbeefUL;

static const volatile uint32_t read_only_small = 0x601dc0feUL;

static void check_tls_item(uint32_t expected)
{
  printf("TLS item = %i\n", tls_item);
  rtems_test_assert(tls_item == expected);
}

static void task(rtems_task_argument arg)
{
  rtems_status_code sc;

  check_tls_item(123);

  sc = rtems_event_transient_send(master_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test(void)
{
  rtems_id id;
  rtems_status_code sc;

  master_task = rtems_task_self();

  rtems_test_assert(read_write_small == 0xdeadbeefUL);
  rtems_test_assert(read_only_small == 0x601dc0feUL);

  check_tls_item(123);
  tls_item = 5;

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  check_tls_item(5);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
