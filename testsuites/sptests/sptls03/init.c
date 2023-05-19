/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#include <rtems/stackchk.h>
#include <rtems/score/thread.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTLS 3";

static volatile uint32_t read_write_small = 0xdeadbeefUL;

static const volatile uint32_t read_only_small = 0x601dc0feUL;

static void test(void)
{
  Thread_Control *executing = _Thread_Get_executing();

  rtems_test_assert(read_write_small == 0xdeadbeefUL);
  rtems_test_assert(read_only_small == 0x601dc0feUL);

  rtems_test_assert(executing->Start.tls_area == NULL);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  rtems_test_assert(!rtems_stack_checker_is_blown());
  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/*
 * Avoid a dependency on errno which might be a thread-local object.  This test
 * assumes that no thread-local storage object is present.
 */
#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

/*
 * This test requires full control over the present thread-local objects.  In
 * certain Newlib configurations, the Newlib reentrancy support may add
 * thread-local objects.
 */
#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
