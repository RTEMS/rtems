/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#include <stdlib.h>

/* Use assert() not rtems_test_assert() since it uses exit() */
#include <assert.h>

#include <rtems.h>

#define EXIT_STATUS 123

static void atexit_not_reached(void)
{
  assert(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code error
)
{
  if (
    source == RTEMS_FATAL_SOURCE_EXIT
      && !is_internal
      && error == EXIT_STATUS
  ) {
    printk("*** END OF TEST EXIT 2 ***\n");
  }
}

static void exit_task(rtems_task_argument arg)
{
  int rv;

  rv = atexit(atexit_not_reached);
  assert(rv == 0);

  _Exit(EXIT_STATUS);
}

static void Init(rtems_task_argument arg)
{
  rtems_status_code sc;
  rtems_id id;

  printk("\n\n*** TEST EXIT 2 ***\n");

  sc = rtems_task_create(
    rtems_build_name('E', 'X', 'I', 'T'),
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, exit_task, 0);
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(RTEMS_SELF);
  assert(sc == RTEMS_SUCCESSFUL);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_INITIAL_EXTENSIONS { .fatal = fatal_extension }

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
