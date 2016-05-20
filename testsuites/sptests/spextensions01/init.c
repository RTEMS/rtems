/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#define TESTS_USE_PRINTK
#include "tmacros.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include <bsp.h>

const char rtems_test_name[] = "SPEXTENSIONS 1";

static int counter;

static int active_extensions = 2;

static rtems_id master_task;

static void assert_static_order(int index)
{
  assert((counter % active_extensions) == index);
  ++counter;
}

static void assert_forward_order(int index)
{
  assert((counter % active_extensions) == index);
  ++counter;
}

static void assert_reverse_order(int index)
{
  assert((counter % active_extensions) == (5 - index));
  ++counter;
}

static bool zero_thread_create(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(0);
  return true;
}

static void zero_thread_start(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(0);
}

static void zero_thread_restart(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(0);
}

static void zero_thread_delete(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(0);
}

static void zero_thread_switch(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(0);
}

static void zero_thread_begin(rtems_tcb *a)
{
  assert_static_order(0);
}

static void zero_thread_exitted(rtems_tcb *a)
{
  assert_static_order(0);
}

static void zero_fatal(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  if (source == RTEMS_FATAL_SOURCE_EXIT) {
    assert_static_order(0);
  }
}

static void zero_thread_terminate(rtems_tcb *a)
{
  assert_static_order(0);
}

static bool one_thread_create(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(1);
  return true;
}

static void one_thread_start(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(1);
}

static void one_thread_restart(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(1);
}

static void one_thread_delete(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(1);
}

static void one_thread_switch(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(1);
}

static void one_thread_begin(rtems_tcb *a)
{
  assert_static_order(1);
}

static void one_thread_exitted(rtems_tcb *a)
{
  assert_static_order(1);
}

static void one_fatal(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  if (source == RTEMS_FATAL_SOURCE_EXIT) {
    assert_static_order(1);
  }
}

static void one_thread_terminate(rtems_tcb *a)
{
  assert_static_order(1);
}

static bool two_thread_create(rtems_tcb *a, rtems_tcb *b)
{
  assert_forward_order(2);
  return true;
}

static void two_thread_start(rtems_tcb *a, rtems_tcb *b)
{
  assert_forward_order(2);
}

static void two_thread_restart(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(2);
}

static void two_thread_delete(rtems_tcb *a, rtems_tcb *b)
{
  assert_reverse_order(2);
}

static void two_thread_switch(rtems_tcb *a, rtems_tcb *b)
{
  assert_forward_order(2);
}

static void two_thread_begin(rtems_tcb *a)
{
  assert_forward_order(2);
}

static void two_thread_exitted(rtems_tcb *a)
{
  assert_forward_order(2);
}

static void two_fatal(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  if (source == RTEMS_FATAL_SOURCE_EXIT) {
    assert_reverse_order(2);
    assert(counter == 72);
    TEST_END();
  }
}

static void two_thread_terminate(rtems_tcb *a)
{
  assert_forward_order(2);
}

static bool three_thread_create(rtems_tcb *a, rtems_tcb *b)
{
  assert_forward_order(3);
  return true;
}

static void three_thread_start(rtems_tcb *a, rtems_tcb *b)
{
  assert_forward_order(3);
}

static void three_thread_restart(rtems_tcb *a, rtems_tcb *b)
{
  assert_static_order(3);
}

static void three_thread_delete(rtems_tcb *a, rtems_tcb *b)
{
  assert_reverse_order(3);
}

static void three_thread_switch(rtems_tcb *a, rtems_tcb *b)
{
  assert_forward_order(3);
}

static void three_thread_begin(rtems_tcb *a)
{
  assert_forward_order(3);
}

static void three_thread_exitted(rtems_tcb *a)
{
  assert_forward_order(3);
}

static void three_fatal(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  if (source == RTEMS_FATAL_SOURCE_EXIT) {
    assert_reverse_order(3);
  }
}

static void three_thread_terminate(rtems_tcb *a)
{
  assert_forward_order(3);
}

#define ZERO \
  { \
    .thread_create = zero_thread_create, \
    .thread_start = zero_thread_start, \
    .thread_restart = zero_thread_restart, \
    .thread_delete = zero_thread_delete, \
    .thread_switch = zero_thread_switch, \
    .thread_begin = zero_thread_begin, \
    .thread_exitted = zero_thread_exitted, \
    .fatal = zero_fatal, \
    .thread_terminate = zero_thread_terminate \
  }

#define ONE \
  { \
    .thread_create = one_thread_create, \
    .thread_start = one_thread_start, \
    .thread_restart = one_thread_restart, \
    .thread_delete = one_thread_delete, \
    .thread_switch = one_thread_switch, \
    .thread_begin = one_thread_begin, \
    .thread_exitted = one_thread_exitted, \
    .fatal = one_fatal, \
    .thread_terminate = one_thread_terminate \
  }

static const rtems_extensions_table two = {
  .thread_create = two_thread_create,
  .thread_start = two_thread_start,
  .thread_restart = two_thread_restart,
  .thread_delete = two_thread_delete,
  .thread_switch = two_thread_switch,
  .thread_begin = two_thread_begin,
  .thread_exitted = two_thread_exitted,
  .fatal = two_fatal,
  .thread_terminate = two_thread_terminate
};

static const rtems_extensions_table three = {
  .thread_create = three_thread_create,
  .thread_start = three_thread_start,
  .thread_restart = three_thread_restart,
  .thread_delete = three_thread_delete,
  .thread_switch = three_thread_switch,
  .thread_begin = three_thread_begin,
  .thread_exitted = three_thread_exitted,
  .fatal = three_fatal,
  .thread_terminate = three_thread_terminate
};

static const rtems_extensions_table initial_test =
  RTEMS_TEST_INITIAL_EXTENSION;

#ifdef BSP_INITIAL_EXTENSION
static const rtems_extensions_table initial_bsp =
  BSP_INITIAL_EXTENSION;
#endif

static void wake_up_master(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_send(master_task);
  assert(sc == RTEMS_SUCCESSFUL);
}

static void wait_for_worker(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static void worker(rtems_task_argument arg)
{
  wake_up_master();

  (void) rtems_task_suspend(RTEMS_SELF);
  assert(false);
}

static void test(void)
{
  rtems_status_code sc;
  rtems_id id;

  master_task = rtems_task_self();

#ifdef BSP_INITIAL_EXTENSION
  sc = rtems_extension_create(
    rtems_build_name(' ', 'B', 'S', 'P'),
    &initial_bsp,
    &id
  );
  assert(sc == RTEMS_SUCCESSFUL);
#undef BSP_INITIAL_EXTENSION
#endif

  sc = rtems_extension_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    &initial_test,
    &id
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_extension_create(
    rtems_build_name('2', ' ', ' ', ' '),
    &two,
    &id
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_extension_create(
    rtems_build_name('3', ' ', ' ', ' '),
    &three,
    &id
  );
  assert(sc == RTEMS_SUCCESSFUL);

  active_extensions = 4;
  assert(counter == 14);
  counter = 16;

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, worker, 0);
  assert(sc == RTEMS_SUCCESSFUL);

  wait_for_worker();

  sc = rtems_task_restart(id, 0);
  assert(sc == RTEMS_SUCCESSFUL);

  wait_for_worker();

  sc = rtems_task_delete(id);
  assert(sc == RTEMS_SUCCESSFUL);

  /* Process zombies to trigger delete extensions */
  sc = rtems_task_create(
    rtems_build_name('N', 'U', 'L', 'L'),
    2,
    SIZE_MAX,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  assert(sc == RTEMS_UNSATISFIED);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 4

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS ZERO, ONE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
