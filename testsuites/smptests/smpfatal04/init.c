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

#include "tmacros.h"

#include <rtems.h>
#include <rtems/score/smpimpl.h>

#include <assert.h>
#include <stdlib.h>

const char rtems_test_name[] = "SMPFATAL 4";

static void Init(rtems_task_argument arg)
{
  assert(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  TEST_BEGIN();

  if (
    source == RTEMS_FATAL_SOURCE_SMP
      && !always_set_to_false
      && code == SMP_FATAL_BOOT_PROCESSOR_NOT_ASSIGNED_TO_SCHEDULER
  ) {
    rtems_status_code sc;
    rtems_id id;

    sc = rtems_scheduler_ident_by_processor(0, &id);
    assert(sc == RTEMS_INCORRECT_STATE);

    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES

#define CONFIGURE_SCHEDULER_ASSIGNMENTS RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
