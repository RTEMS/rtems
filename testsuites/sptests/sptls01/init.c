/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2014, 2022 embedded brains GmbH & Co. KG
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

#include <rtems/bspIo.h>
#include <rtems/stackchk.h>
#include <rtems/sysinit.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/tls.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTLS 1";

static rtems_id master_task;

static __thread volatile char tls_item = 123;

static volatile uint32_t read_write_small = 0xdeadbeefUL;

static const volatile uint32_t read_only_small = 0x601dc0feUL;

static void check_tls_item(uint32_t expected)
{
  printk("TLS item = %i\n", tls_item);
  rtems_test_assert(tls_item == expected);
}

static void task(rtems_task_argument arg)
{
  rtems_status_code sc;

  check_tls_item(123);
  tls_item = 42;

  sc = rtems_event_transient_send(master_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void check_tls_size(void)
{
  const volatile TLS_Configuration *config;
  uintptr_t tls_size;

  config = &_TLS_Configuration;
  tls_size = (uintptr_t) config->size;

  if (tls_size != 1) {
    printk(
      "WARNING: The thread-local storage size is %" PRIuPTR ".  It should be\n"
      "exactly one for this test.  Check the BSP implementation.  The BSP\n"
      "should not pull in thread-local storage objects such as errno for\n"
      "this test.\n",
      tls_size
    );
    rtems_test_assert(tls_size == 1);
  }
}

static Thread_Control *get_thread(rtems_id id)
{
  Thread_Control *the_thread;
  ISR_lock_Context lock_context;

  the_thread = _Thread_Get(id, &lock_context);
  _ISR_lock_ISR_enable(&lock_context);
  return the_thread;
}

static void test(void)
{
  rtems_id id;
  rtems_status_code sc;
  Thread_Control *self;
  Thread_Control *other;
  char *self_tp;
  char *other_tp;
  uintptr_t tls_item_offset;
  char *self_tls_item;
  char *other_tls_item;

  master_task = rtems_task_self();

  rtems_test_assert(read_write_small == 0xdeadbeefUL);
  rtems_test_assert(read_only_small == 0x601dc0feUL);

  check_tls_item(123);
  tls_item = 5;

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(id, task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  self = get_thread(master_task);
  other = get_thread(id);
  self_tp = _CPU_Get_TLS_thread_pointer(&self->Registers);
  other_tp = _CPU_Get_TLS_thread_pointer(&other->Registers);
  tls_item_offset = (uintptr_t) (&tls_item - self_tp);
  self_tls_item = self_tp + tls_item_offset;
  other_tls_item = other_tp + tls_item_offset;
  rtems_test_assert(*self_tls_item == 5);
  rtems_test_assert(*other_tls_item == 123);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(*self_tls_item == 5);
  rtems_test_assert(*other_tls_item == 42);

  sc = rtems_task_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  check_tls_item(5);
}

static void test_idle_during_system_init(void)
{
  rtems_print_printer_printk(&rtems_test_printer);
  TEST_BEGIN();

  check_tls_item(123);
}

static void Init(rtems_task_argument arg)
{
  test();

  rtems_test_assert(!rtems_stack_checker_is_blown());
  check_tls_size();
  TEST_END();

  rtems_test_exit(0);
}

RTEMS_SYSINIT_ITEM(
  test_idle_during_system_init,
  RTEMS_SYSINIT_IDLE_THREADS,
  RTEMS_SYSINIT_ORDER_LAST
);

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/*
 * Avoid a dependency on errno which might be a thread-local object.  This test
 * assumes that no thread-local storage object other than tls_item is present.
 */
#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

/*
 * This test requires full control over the present thread-local objects.  In
 * certain Newlib configurations, the Newlib reentrancy support may add
 * thread-local objects.
 */
#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
