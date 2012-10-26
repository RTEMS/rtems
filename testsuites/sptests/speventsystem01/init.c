/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>

#include "tmacros.h"

#define EVENT RTEMS_EVENT_0

static void test_with_normal_and_system_event(void)
{
  rtems_status_code sc;
  rtems_event_set out;

  /* Assert no events pending */

  sc = rtems_event_receive(EVENT, RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  sc = rtems_event_system_receive(EVENT, RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  /* Send system event */

  sc = rtems_event_system_send(rtems_task_self(), EVENT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_receive(EVENT, RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  out = 0;
  sc = rtems_event_system_receive(EVENT, RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(out == EVENT);

  /* Send normal event */

  sc = rtems_event_send(rtems_task_self(), EVENT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  out = 0;
  sc = rtems_event_receive(EVENT, RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(out == EVENT);

  sc = rtems_event_system_receive(EVENT, RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void test_with_timeout(void)
{
  rtems_status_code sc;
  rtems_event_set out;

  sc = rtems_event_system_receive(EVENT, RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  sc = rtems_event_system_receive(EVENT, RTEMS_WAIT, 1, &out);
  rtems_test_assert(sc == RTEMS_TIMEOUT);

  sc = rtems_event_system_receive(EVENT, RTEMS_NO_WAIT, 0, &out);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SPEVENTSYSTEM 1 ***");

  test_with_normal_and_system_event();
  test_with_timeout();

  puts("*** END OF TEST SPEVENTSYSTEM 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
