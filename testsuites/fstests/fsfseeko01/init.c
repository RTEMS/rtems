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
 * http://www.rtems.com/license/LICENSE.
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

static void test(void)
{
  FILE *file;
  int rv;
  const off_t off = sizeof(off_t) >= sizeof(int64_t)
    ? INT64_MAX
    : (sizeof(off_t) == sizeof(int32_t) ? INT32_MAX : 1);
  off_t actual_off;
 
  errno = 0;
  file = fopen("file", "w+");
  perror("fopen");
  rtems_test_assert(file != NULL);

  errno = 0;
  rv = fseeko(file, off, SEEK_SET);
  perror("fseeko");
  rtems_test_assert(rv == 0);

  errno = 0;
  actual_off = ftello(file);
  perror("ftello");
  rtems_test_assert(actual_off == off);

  errno = 0;
  rv = fclose(file);
  perror("fclose");
  rtems_test_assert(rv == 0);
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST FSFSEEKO 1 ***");

  test();

  puts("*** END OF TEST FSFSEEKO 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
