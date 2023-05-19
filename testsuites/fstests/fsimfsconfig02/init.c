/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2015 embedded brains GmbH & Co. KG
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

#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <rtems/imfs.h>
#include <rtems/libio.h>

const char rtems_test_name[] = "FSIMFSCONFIG 2";

static void Init(rtems_task_argument arg)
{
  const char *mnt = "mnt";
  const char *link = "link";
  char buf[1];
  int rv;

  TEST_BEGIN();

  rv = mkdir(mnt, S_IRWXU);
  rtems_test_assert(rv == 0);

  rv = mount(
    "",
    mnt,
    RTEMS_FILESYSTEM_TYPE_IMFS,
    RTEMS_FILESYSTEM_READ_ONLY,
    NULL
  );
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = unmount(mnt);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  rv = symlink(mnt, link);
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = readlink(link, &buf[0], sizeof(buf));
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_FILESYSTEM_IMFS

#define CONFIGURE_IMFS_DISABLE_READLINK
#define CONFIGURE_IMFS_DISABLE_UNMOUNT

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
