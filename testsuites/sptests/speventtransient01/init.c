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

const char rtems_test_name[] = "SPEVENTTRANSIENT 1";

typedef struct {
  rtems_id client;
  bool complete;
} request;

static void server_task(rtems_task_argument arg)
{
  rtems_status_code sc;
  request *req = (request *) arg;

  req->complete = true;

  sc = rtems_event_transient_send(req->client);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_exit();
}

static void test_with_request_server(void)
{
  rtems_status_code sc;
  rtems_id id;
  request req;

  sc = rtems_event_transient_receive(RTEMS_NO_WAIT, 0);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  req.client = rtems_task_self();
  req.complete = false;

  sc = rtems_task_create(
    rtems_build_name('S', 'E', 'R', 'V'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, server_task, (rtems_task_argument) &req);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(req.complete);
}

static void test_with_request_self(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(RTEMS_NO_WAIT, 0);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  sc = rtems_event_transient_send(rtems_task_self());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_NO_WAIT, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_event_transient_clear();

  sc = rtems_event_transient_receive(RTEMS_NO_WAIT, 0);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void test_with_timeout(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(RTEMS_NO_WAIT, 0);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  sc = rtems_event_transient_receive(RTEMS_WAIT, 1);
  rtems_test_assert(sc == RTEMS_TIMEOUT);

  sc = rtems_event_transient_receive(RTEMS_NO_WAIT, 0);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_with_request_server();
  test_with_request_self();
  test_with_timeout();

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
