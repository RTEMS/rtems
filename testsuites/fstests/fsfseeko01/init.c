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

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
