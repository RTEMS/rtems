/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2024 Contemporary Software, Chris Johns
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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include <rtems/libio_.h>

/* Use assert() not rtems_test_assert() since it uses exit() */
#include <assert.h>

#include <rtems.h>

const char rtems_test_name[] = "LIBIO FD OPEN";

#define NUMOF(_a) (sizeof(_a) / sizeof(_a[0]))

static void Init(rtems_task_argument arg)
{
  (void) arg;

  int fd[10];
  int i;
  int base_count;

  TEST_BEGIN();

  for (i = 0; i < NUMOF(fd); i++) {
    fd[i] = -1;
  }

  base_count = rtems_libio_count_open_iops();

  fd[0] = open("/x123", O_WRONLY | O_TRUNC | O_CREAT, 0x777);
  rtems_test_assert(fd[0] != -1);
  rtems_test_assert(rtems_libio_count_open_iops() == 3 + 1);
  rtems_test_assert(write(fd[0], "0123456789\n", 11) == 11);
  rtems_test_assert(close(fd[0]) == 0);

  for (i = 0; i < 5; ++i) {
    fd[i] = open("/x123", O_RDONLY);
    rtems_test_assert(fd[i] >= 0);
    rtems_test_assert(rtems_libio_count_open_iops() == base_count + i + 1);
  }

  for (i = 5; i < 10; ++i) {
    fd[i] = open("/x123", O_RDONLY);
    rtems_test_assert(fd[i] >= 0);
    rtems_test_assert(rtems_libio_count_open_iops() == base_count + i + 1);
  }

  for (i = 0; i < 10; ++i) {
    rtems_test_assert(close(fd[i]) == 0);
  }

  TEST_END();
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 13

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
