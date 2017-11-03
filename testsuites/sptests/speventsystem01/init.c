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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPEVENTSYSTEM 1";

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

static void test_with_invalid_receiver(void)
{
  rtems_status_code sc;

  sc = rtems_event_system_send(0xffff, EVENT);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_with_invalid_address(void)
{
  rtems_status_code sc;

  sc = rtems_event_system_receive(EVENT, RTEMS_NO_WAIT, 0, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

static void test_get_pending_events(void)
{
  rtems_status_code sc;
  rtems_event_set out;

  sc = rtems_event_system_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_NO_WAIT,
    0,
    &out
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(out == 0);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_with_normal_and_system_event();
  test_with_timeout();
  test_with_invalid_receiver();
  test_with_invalid_address();
  test_get_pending_events();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
