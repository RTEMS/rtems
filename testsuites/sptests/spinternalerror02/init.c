/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012, 2020 embedded brains GmbH & Co. KG
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

#include <rtems.h>

const char rtems_test_name[] = "SPINTERNALERROR 2";

static void test_internal_error_text(void)
{
  rtems_fatal_code error = 0;
  const char *text = NULL;
  const char *text_last;

  do {
    text_last = text;
    text = rtems_internal_error_text( error );
    ++error;
    puts( text );
  } while ( text != text_last );

  rtems_test_assert(
    error - 3 == INTERNAL_ERROR_IDLE_THREAD_STACK_TOO_SMALL
  );
}

static void test_fatal_source_text(void)
{
  rtems_fatal_source source = 0;
  const char *text = NULL;
  const char *text_last;

  do {
    text_last = text;
    text = rtems_fatal_source_text( source );
    ++source;
    puts( text );
  } while ( text != text_last );

  rtems_test_assert( source - 3 == RTEMS_FATAL_SOURCE_SPURIOUS_INTERRUPT );
}

static void test_status_text(void)
{
  rtems_status_code code = 0;
  const char *text = NULL;
  const char *text_last;

  do {
    text_last = text;
    text = rtems_status_text( code );
    ++code;
    puts( text );
  } while ( text != text_last );

  rtems_test_assert( code - 3 == RTEMS_PROXY_BLOCKING );
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_internal_error_text();
  test_fatal_source_text();
  test_status_text();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
