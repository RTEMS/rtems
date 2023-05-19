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
#include <rtems/malloc.h>

static void ensure_server_termination(void)
{
  rtems_status_code sc;
  rtems_task_priority prio;
  rtems_id id;
  uint32_t cpu_self;
  uint32_t cpu_other;

  prio = 0;
  sc = rtems_task_set_priority(RTEMS_SELF, 124, &prio);
  T_rsc_success(sc);

  cpu_self = rtems_scheduler_get_processor();
  cpu_other = (cpu_self + 1) % 2;

  sc = rtems_scheduler_ident_by_processor(cpu_other, &id);
  T_rsc_success(sc);

  sc = rtems_task_set_scheduler(RTEMS_SELF, id, 124);
  T_rsc_success(sc);

  sc = rtems_scheduler_ident_by_processor(cpu_self, &id);
  T_rsc_success(sc);

  sc = rtems_task_set_scheduler(RTEMS_SELF, id, prio);
  T_rsc_success(sc);
}

T_TEST_CASE(InterruptServerSMPInitializeIncorrectState)
{
  rtems_status_code sc;
  uint32_t server_count;

  T_assert_eq_u32(rtems_scheduler_get_processor_maximum(), 2);

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
  T_eq_u32(server_count, 2);

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
  ensure_server_termination();

  sc = rtems_interrupt_server_delete(1);
  T_rsc_success(sc);
  ensure_server_termination();
}

T_TEST_CASE(InterruptServerSMPInitializeInvalidPriority)
{
  rtems_status_code sc;
  uint32_t server_count;

  T_assert_eq_u32(rtems_scheduler_get_processor_maximum(), 2);

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
  T_eq_u32(server_count, 2);

  sc = rtems_interrupt_server_delete(0);
  T_rsc_success(sc);
  ensure_server_termination();

  sc = rtems_interrupt_server_delete(1);
  T_rsc_success(sc);
  ensure_server_termination();
}

T_TEST_CASE(InterruptServerSMPInitializeNoMemory)
{
  rtems_status_code sc;
  uint32_t server_count;
  void *greedy;

  T_assert_eq_u32(rtems_scheduler_get_processor_maximum(), 2);

  greedy = rtems_heap_greedy_allocate(NULL, 0);

  server_count = 456;
  sc = rtems_interrupt_server_initialize(
    123,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &server_count
  );
  T_rsc(sc, RTEMS_NO_MEMORY);
  T_eq_u32(server_count, 1);

  rtems_heap_greedy_free(greedy);

  sc = rtems_interrupt_server_delete(0);
  T_rsc_success(sc);
  ensure_server_termination();

  sc = rtems_interrupt_server_delete(1);
  T_rsc(sc, RTEMS_INVALID_ID);
}

T_TEST_CASE(InterruptServerSMPInitializeNoScheduler)
{
  rtems_status_code sc;
  uint32_t server_count;
  rtems_id scheduler_id;
  rtems_task_priority prio;

  T_assert_eq_u32(rtems_scheduler_get_processor_maximum(), 2);

  scheduler_id = 0;
  sc = rtems_scheduler_ident_by_processor(1, &scheduler_id);
  T_rsc_success(sc);
  T_ne_u32(scheduler_id, 0);

  sc = rtems_scheduler_remove_processor(scheduler_id, 1);
  T_rsc_success(sc);

  server_count = 456;
  sc = rtems_interrupt_server_initialize(
    123,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &server_count
  );
  T_rsc_success(sc);
  T_eq_u32(server_count, 2);

  sc = rtems_interrupt_server_delete(0);
  T_rsc_success(sc);

  sc = rtems_interrupt_server_delete(1);
  T_rsc_success(sc);

  prio = 0;
  sc = rtems_task_set_priority(RTEMS_SELF, 124, &prio);
  T_rsc_success(sc);

  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  T_rsc_success(sc);

  sc = rtems_scheduler_add_processor(scheduler_id, 1);
  T_rsc_success(sc);
}

const char rtems_test_name[] = "SMPIRQS 1";

static void Init(rtems_task_argument argument)
{
  rtems_test_run(argument, TEST_STATE);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_SCHEDULER_EDF_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_EDF_SMP(a);

RTEMS_SCHEDULER_EDF_SMP(b);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(a, rtems_build_name('A', ' ', ' ', ' ')), \
  RTEMS_SCHEDULER_TABLE_EDF_SMP(b, rtems_build_name('B', ' ', ' ', ' '))

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
