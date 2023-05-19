/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <rtems/test.h>
#include <rtems/test-info.h>

#include <rtems.h>
#include <rtems/irq-extension.h>

typedef struct {
  rtems_interrupt_server_control base;
  int magic;
} server_control;

static void destroy(rtems_interrupt_server_control *base)
{
  server_control *control;

  control = (server_control *) base;
  T_step_eq_int(2, control->magic, 0x54192a88);
}

static const rtems_interrupt_server_config valid_config = {
  .name = rtems_build_name('N', 'A', 'M', 'E'),
  .priority = 123,
  .storage_size = RTEMS_MINIMUM_STACK_SIZE,
  .modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_DEFAULT_ATTRIBUTES,
  .destroy = destroy
};

T_TEST_CASE(InterruptServerCreateDestroy)
{
  server_control control;
  rtems_status_code sc;
  uint32_t server_index;
  rtems_task_priority prio;

  T_plan(7);

  memset(&control, 0xff, sizeof(control));
  control.magic = 0x54192a88;
  server_index = 0x2833763f;

  sc = rtems_interrupt_server_create(
    &control.base,
    &valid_config,
    &server_index
  );
  T_step_rsc_success(0, sc);
  T_step_ne_u32(1, server_index, 0x2833763f);

  sc = rtems_interrupt_server_delete(server_index);
  T_step_rsc_success(3, sc);

  /* Make sure the interrupt server terminated */
  prio = 0;
  sc = rtems_task_set_priority(RTEMS_SELF, 124, &prio);
  T_step_rsc_success(4, sc);
  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  T_step_rsc_success(5, sc);
  T_step_eq_u32(6, prio, 124);
}

static const rtems_interrupt_server_config invalid_config = {
  .name = rtems_build_name('N', 'A', 'M', 'E'),
  .priority = 0,
  .storage_size = RTEMS_MINIMUM_STACK_SIZE,
  .modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_DEFAULT_ATTRIBUTES,
  .destroy = destroy
};

T_TEST_CASE(InterruptServerCreateError)
{
  rtems_interrupt_server_control control;
  rtems_interrupt_server_control control_2;
  rtems_status_code sc;
  uint32_t server_index;

  memset(&control, 0xff, sizeof(control));
  memset(&control_2, 0xff, sizeof(control_2));
  server_index = 0x235e8766;

  sc = rtems_interrupt_server_create(&control, &invalid_config, &server_index);
  T_rsc(sc, RTEMS_INVALID_PRIORITY);
  T_eq_mem(&control, &control_2, sizeof(control));
  T_eq_u32(server_index, 0x235e8766);
}

static const rtems_interrupt_server_config request_config = {
  .name = rtems_build_name('N', 'A', 'M', 'E'),
  .priority = 123,
  .storage_size = RTEMS_MINIMUM_STACK_SIZE,
  .modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_DEFAULT_ATTRIBUTES
};

static void not_called(void *arg)
{
  (void) arg;
  T_unreachable();
}

static void request(void *arg)
{
  rtems_id *id;
  rtems_status_code sc;

  id = arg;
  T_step(4);
  sc = rtems_event_transient_send(*id);
  T_step_rsc_success(6, sc);
}

T_TEST_CASE(InterruptServerRequest)
{
  rtems_interrupt_server_control control;
  rtems_interrupt_server_request req;
  rtems_status_code sc;
  uint32_t server_index;
  rtems_id id;
  rtems_task_priority prio;

  T_plan(11);

  memset(&control, 0xff, sizeof(control));
  memset(&req, 0xff, sizeof(req));
  server_index = 0x99d225bd;

  sc = rtems_interrupt_server_create(
    &control,
    &request_config,
    &server_index
  );
  T_step_rsc_success(0, sc);
  T_step_ne_u32(1, server_index, 0x99d225bd);

  sc = rtems_interrupt_server_request_initialize(
    server_index,
    &req,
    not_called,
    NULL
  );
  T_step_rsc_success(2, sc);

  rtems_interrupt_server_request_submit(&req);
  rtems_interrupt_server_request_destroy(&req);

  id = rtems_task_self();
  sc = rtems_interrupt_server_request_initialize(
    server_index,
    &req,
    request,
    &id
  );
  T_step_rsc_success(3, sc);

  rtems_interrupt_server_request_submit(&req);
  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  T_step_rsc_success(5, sc);

  rtems_interrupt_server_request_destroy(&req);

  sc = rtems_interrupt_server_delete(server_index);
  T_step_rsc_success(7, sc);

  /* Make sure the interrupt server terminated */
  prio = 0;
  sc = rtems_task_set_priority(RTEMS_SELF, 124, &prio);
  T_step_rsc_success(8, sc);
  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  T_step_rsc_success(9, sc);
  T_step_eq_u32(10, prio, 124);
}

T_TEST_CASE(InterruptServerInitializeDestroy)
{
  rtems_status_code sc;
  uint32_t server_count;
  rtems_task_priority prio;

  sc = rtems_interrupt_server_delete(0);
  T_rsc(sc, RTEMS_INVALID_ID);

  server_count = 456;
  sc = rtems_interrupt_server_initialize(
    123,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &server_count
  );
  T_rsc_success(sc);
  T_eq_u32(server_count, 1);

  server_count = 456;
  sc = rtems_interrupt_server_initialize(
    123,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &server_count
  );
  T_rsc(sc, RTEMS_INCORRECT_STATE);
  T_eq_u32(server_count, 0);

  sc = rtems_interrupt_server_delete(0);
  T_rsc_success(sc);

  /* Make sure the interrupt server terminated */
  prio = 0;
  sc = rtems_task_set_priority(RTEMS_SELF, 124, &prio);
  T_rsc_success(sc);
  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  T_rsc_success(sc);
  T_eq_u32(prio, 124);

  server_count = 456;
  sc = rtems_interrupt_server_initialize(
    0,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &server_count
  );
  T_rsc(sc, RTEMS_INVALID_PRIORITY);
  T_eq_u32(server_count, 0);

  server_count = 456;
  sc = rtems_interrupt_server_initialize(
    123,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &server_count
  );
  T_rsc_success(sc);
  T_eq_u32(server_count, 1);

  sc = rtems_interrupt_server_delete(0);
  T_rsc_success(sc);

  /* Make sure the interrupt server terminated */
  prio = 0;
  sc = rtems_task_set_priority(RTEMS_SELF, 124, &prio);
  T_rsc_success(sc);
  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  T_rsc_success(sc);
  T_eq_u32(prio, 124);
}

const char rtems_test_name[] = "IRQS 1";

static void Init(rtems_task_argument argument)
{
  rtems_test_run(argument, TEST_STATE);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
