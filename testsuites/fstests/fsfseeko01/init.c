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

#include "tmacros.h"

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>

const char rtems_test_name[] = "FSFSEEKO 1";

static void test(void)
{
  FILE *file;
  int rv;
  const off_t off = sizeof(off_t) >= sizeof(int64_t)
    ? INT64_MAX
    : (sizeof(off_t) == sizeof(int32_t) ? INT32_MAX : 1);
  off_t actual_off;
  const long long_off = LONG_MAX;
  long actual_long_off;

  errno = 0;
  file = fopen("file", "w+");
  rtems_test_assert(file != NULL);
  rtems_test_assert(errno == 0);

  errno = 0;
  rv = fseek(file, long_off, SEEK_SET);
  rtems_test_assert(rv == 0);
  rtems_test_assert(errno == 0);

  errno = 0;
  actual_long_off = ftell(file);
  rtems_test_assert(actual_long_off == long_off);
  rtems_test_assert(errno == 0);

  errno = 0;
  actual_off = ftello(file);
  rtems_test_assert(actual_off == long_off);
  rtems_test_assert(errno == 0);

  errno = 0;
  rv = fseeko(file, off, SEEK_SET);
  rtems_test_assert(rv == 0);
  rtems_test_assert(errno == 0);

  if (sizeof(off_t) == sizeof(long)) {
    errno = 0;
    actual_long_off = ftell(file);
    rtems_test_assert(actual_long_off == off);
    rtems_test_assert(errno == 0);
  } else {
    errno = 0;
    actual_long_off = ftell(file);
    rtems_test_assert(actual_long_off == -1L);
    rtems_test_assert(errno == EOVERFLOW);
  }

  errno = 0;
  actual_off = ftello(file);
  rtems_test_assert(actual_off == off);
  rtems_test_assert(errno == 0);

  errno = 0;
  rv = fclose(file);
  rtems_test_assert(rv == 0);
  rtems_test_assert(errno == 0);
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

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
