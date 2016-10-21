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

const char rtems_test_name[] = "SMPFATAL 3";

static void task(rtems_task_argument arg)
{
  rtems_status_code sc;
  rtems_id *sem_id;

  sem_id = (rtems_id *) arg;

  sc = rtems_semaphore_obtain(*sem_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(0);
}

static void Init(rtems_task_argument arg)
{
  rtems_status_code sc;
  rtems_id task_id;
  rtems_id sem_id;

  TEST_BEGIN();

  sc = rtems_semaphore_create(
    rtems_build_name('M', 'R', 'S', 'P'),
    1,
    RTEMS_MULTIPROCESSOR_RESOURCE_SHARING
      | RTEMS_BINARY_SEMAPHORE,
    1,
    &sem_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(task_id, task, (rtems_task_argument) &sem_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  _Thread_Dispatch_disable();
  rtems_semaphore_obtain(sem_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  if (
    source == INTERNAL_ERROR_CORE
      && !is_internal
      && code == INTERNAL_ERROR_THREAD_QUEUE_ENQUEUE_STICKY_FROM_BAD_STATE
  ) {
    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_MRSP_SEMAPHORES 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
