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

const char rtems_test_name[] = "SMPFATAL 5";

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
      && code == SMP_FATAL_MANDATORY_PROCESSOR_NOT_PRESENT
  ) {
    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

/* Lets see when the first RTEMS system hits this limit */
#define CONFIGURE_MAXIMUM_PROCESSORS 64

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_SIMPLE_SMP(a);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(a, rtems_build_name('S', 'I', 'M', 'P'))

#define ASSIGN \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
 ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, \
 ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, \
 ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, \
 ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, \
 ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, \
 ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, \
 ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, \
 ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN, ASSIGN

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
