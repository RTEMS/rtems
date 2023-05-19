/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
