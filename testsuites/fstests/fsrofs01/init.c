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

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include <rtems/libio.h>
#include <rtems/rtems-rfs-format.h>
#include <rtems/ramdisk.h>

const char rtems_test_name[] = "FSROFS 1";

static const rtems_rfs_format_config rfs_config;

static const char rda [] = "/dev/rda";

static const char mnt [] = "/mnt";

static const char file [] = "/mnt/file";

static const char not_exist [] = "/mnt/not_exist";

static void test_mount(bool writeable)
{
  int rv;
  const void *data = NULL;

  rv = mount(
    rda,
    mnt,
    RTEMS_FILESYSTEM_TYPE_RFS,
    writeable ? RTEMS_FILESYSTEM_READ_WRITE : 0,
    data
  );
  rtems_test_assert(rv == 0);
}

static void test_create_file_system(void)
{
  int rv;

  rv = mkdir(mnt, S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(rv == 0);

  rv = rtems_rfs_format(rda, &rfs_config);
  rtems_test_assert(rv == 0);

  test_mount(true);

  rv = mknod(file, S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO, 0);
  rtems_test_assert(rv == 0);

  rv = unmount(mnt);
  rtems_test_assert(rv == 0);
}

static void test_rofs(void)
{
  int rv;
  int fd;
  char buf [1];
  ssize_t n;

  test_mount(false);

  fd = open(file, O_RDONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(fd >= 0);

  n = read(fd, buf, sizeof(buf));
  rtems_test_assert(n == 0);

  errno = 0;
  n = write(fd, buf, sizeof(buf));
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == EBADF);

  errno = 0;
  rv = ftruncate(fd, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);

  errno = 0;
  rv = fchmod(fd, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EROFS);

  errno = 0;
  rv = fchown(fd, 0, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EROFS);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  errno = 0;
  fd = open(not_exist, O_RDONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(fd == -1);
  rtems_test_assert(errno == EROFS);

  errno = 0;
  rv = mknod(not_exist, S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EROFS);

  errno = 0;
  rv = mkdir(not_exist, S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EROFS);

  errno = 0;
  rv = rename(file, not_exist);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EROFS);

  errno = 0;
  rv = link(file, not_exist);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EROFS);

  errno = 0;
  rv = unlink(file);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EROFS);

  errno = 0;
  rv = symlink(file, not_exist);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EROFS);

  rv = unmount(mnt);
  rtems_test_assert(rv == 0);
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  test_create_file_system();
  test_rofs();

  TEST_END();
  rtems_test_exit(0);
}

rtems_ramdisk_config rtems_ramdisk_configuration [] = {
  { .block_size = 512, .block_num = 1024 }
};

size_t rtems_ramdisk_configuration_size = 1;

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS RAMDISK_DRIVER_TABLE_ENTRY
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_FILESYSTEM_RFS

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_EXTRA_TASK_STACKS (8 * 1024)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
