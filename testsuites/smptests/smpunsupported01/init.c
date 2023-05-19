/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 On-Line Applications Research Corporation (OAR).
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
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

#include "tmacros.h"

const char rtems_test_name[] = "SMPUNSUPPORTED 1";

static void test(void)
{
  rtems_status_code sc;
  rtems_mode mode;
  rtems_id id;

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

  mode = RTEMS_INTERRUPT_LEVEL(0);
  if (mode == 0) {
    sc = rtems_task_mode(mode, RTEMS_INTERRUPT_MASK, &mode);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  mode = RTEMS_INTERRUPT_LEVEL(1);
  if (mode != 0) {
    sc = rtems_task_mode(mode, RTEMS_INTERRUPT_MASK, &mode);
    rtems_test_assert(sc == RTEMS_NOT_IMPLEMENTED);

    sc = rtems_task_create(
      rtems_build_name('T', 'A', 'S', 'K'),
      RTEMS_MINIMUM_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      mode,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    rtems_test_assert(sc == RTEMS_UNSATISFIED);
  } else {
    puts("RTEMS_INTERRUPT_LEVEL(1) not supported on this platform");
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
