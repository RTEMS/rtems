/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <stdio.h>
#include <inttypes.h>

#include <rtems.h>

#include "tmacros.h"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

/*
 * This test case shows that post switch extension handlers must cope with
 * already deleted resources (e.g. _POSIX_signals_Post_switch_extension()).
 * The thread delete extensions run with thread dispatching enabled.  Only the
 * allocation mutex is locked.
 */

static rtems_id task_0 = RTEMS_ID_NONE;

static rtems_id task_1 = RTEMS_ID_NONE;

static void thread_delete_hook(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (deleted->Object.id == task_0) {
    rtems_task_priority old = 0;

    sc = rtems_task_set_priority(task_1, 2, &old);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void suicide_task(rtems_task_argument arg)
{
  printf("suicide task %" PRIuPTR "\n", arg);

  rtems_task_delete(RTEMS_SELF);
  rtems_test_assert(false);
}

void Init(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  puts("\n\n*** POSIX TEST 15 ***");

  sc = rtems_task_create(
    rtems_build_name('T', 'S', 'K', '1'),
    5,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_1
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_1, suicide_task, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('T', 'S', 'K', '0'),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_0
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_0, suicide_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  puts("*** END OF POSIX TEST 15 ***");

  rtems_test_exit(0);
  rtems_test_assert(false);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT
#define CONFIGURE_INIT_TASK_PRIORITY 4

#define CONFIGURE_INITIAL_EXTENSIONS { .thread_delete = thread_delete_hook }

#include <rtems/confdefs.h>
