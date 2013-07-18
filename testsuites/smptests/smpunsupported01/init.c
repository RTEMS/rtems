/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#include "tmacros.h"

static void test(void)
{
  rtems_status_code sc;
  rtems_mode mode;
  rtems_id id;

  rtems_test_assert(rtems_configuration_is_smp_enabled());

  sc = rtems_task_delete(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_NOT_IMPLEMENTED);

  sc = rtems_task_variable_add(RTEMS_SELF, NULL, NULL);
  rtems_test_assert(sc == RTEMS_NOT_IMPLEMENTED);

  sc = rtems_task_variable_delete(RTEMS_SELF, NULL);
  rtems_test_assert(sc == RTEMS_NOT_IMPLEMENTED);

  sc = rtems_task_variable_get(RTEMS_SELF, NULL, NULL);
  rtems_test_assert(sc == RTEMS_NOT_IMPLEMENTED);

  sc = rtems_task_mode(RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &mode);
  rtems_test_assert(sc == RTEMS_NOT_IMPLEMENTED);

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SMPUNSUPPORTED 1 ***");

  test();

  puts("*** END OF TEST SMPUNSUPPORTED 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS 1

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
