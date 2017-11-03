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

const char rtems_test_name[] = "SPTASKNOPREEMPT 1";

static bool did_run;

static void do_not_run(rtems_task_argument arg)
{
#if 0
  rtems_test_assert(0);
#else
  did_run = true;
  rtems_task_suspend(RTEMS_SELF);
#endif
}

static void test(void)
{
  rtems_status_code sc;
  rtems_id task;

  sc = rtems_task_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task, do_not_run, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /*
   * This will start a pseudo interrupt task pre-empting the non-preemtive
   * executing task.  Later the high priority do_not_run() task is scheduled.
   * See also https://devel.rtems.org/ticket/2365.
   */
  sc = rtems_timer_initiate_server(
    RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /* This is probably a bug and not a feature */
  rtems_test_assert(did_run);

  sc = rtems_task_delete(task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
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

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_INIT_TASK_PRIORITY 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
