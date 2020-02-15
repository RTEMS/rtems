#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

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
 * http://www.rtems.org/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "29"
#define FATAL_ERROR_DESCRIPTION          "yield in interrupt context"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL

#define CONFIGURE_MAXIMUM_TIMERS 1

static void timer(rtems_id id, void *arg)
{
  rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
}

static void force_error(void)
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_timer_create(
    rtems_build_name('T', 'I', 'M', 'E'),
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_fire_after(id, 1, timer, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_exit();
}

#include "../spfatal_support/spfatalimpl.h"
