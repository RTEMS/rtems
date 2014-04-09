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

#include <rtems.h>
#include <rtems/test.h>
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
  bool is_internal,
  rtems_fatal_code code
)
{
  rtems_test_begink();

  if (
    source == RTEMS_FATAL_SOURCE_SMP
      && !is_internal
      && code == SMP_FATAL_MANDATORY_PROCESSOR_NOT_PRESENT
  ) {
    rtems_test_endk();
  }
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_SMP_APPLICATION

/* Lets see when the first RTEMS system hits this limit */
#define CONFIGURE_SMP_MAXIMUM_PROCESSORS 64

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP(a);

#define CONFIGURE_SCHEDULER_CONTROLS \
  RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP(a, rtems_build_name('S', 'I', 'M', 'P'))

#define ASSIGN \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY)

#define CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS \
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
