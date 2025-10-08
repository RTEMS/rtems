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
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <utime.h>

#include <rtems/imfs.h>
#include <rtems/libio.h>

const char rtems_test_name[] = "FSIMFSCONFIG 3";

static const IMFS_node_control node_control = IMFS_GENERIC_INITIALIZER(
  &rtems_filesystem_handlers_default,
  IMFS_node_initialize_generic,
  IMFS_node_destroy_default
);

static void Init(rtems_task_argument arg)
{
  (void) arg;

  const struct utimbuf times = {0};
  const char *generic = "generic";
  const char *mnt = "mnt";
  const char *dev = "device";
  const char *file = "file";
  const char *fifo = "fifo";
  int rv;
  int fd;

  TEST_BEGIN();

  rv = IMFS_make_generic_node(
    generic,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = chown(generic, 0, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = chmod(generic, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = link(generic, "link");
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  rv = mkdir(mnt, S_IRWXU);
  rtems_test_assert(rv == 0);

  rv = mknod(dev, S_IFCHR | S_IRWXU, 0);
  rtems_test_assert(rv == 0);

  fd = creat(file, S_IRWXU);
  rtems_test_assert(fd == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = mkfifo(fifo, S_IRWXU);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOSYS);

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

  errno = 0;
  rv = rename(generic, "new");
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = symlink(generic, "link");
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = utime(generic, &times);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  rv = unlink(generic);
  rtems_test_assert(rv == 0);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 1

#define CONFIGURE_FILESYSTEM_IMFS

#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
