/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup tests
 *
 * @brief Block device tests.
 */

/*
 * Copyright (C) 2009, 2018 embedded brains GmbH & Co. KG
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

#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include "tmacros.h"

#include <rtems.h>
#include <rtems/ramdisk.h>
#include <rtems/diskdevs.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

const char rtems_test_name[] = "BLOCK 1";

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define ASSERT_SC_EQ(sc, sc_expected) rtems_test_assert((sc) == (sc_expected))

#define BLOCK_SIZE 512U

#define BLOCK_COUNT 16U

static const rtems_driver_address_table ramdisk_ops = {
  .initialization_entry = NULL,
  RTEMS_GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES
};

static void test_block_io_control_api(dev_t dev, ramdisk *rd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_disk_device *dd = NULL;
  rtems_disk_device *fd_dd = NULL;
  int fd = -1;
  int rv = -1;
  uint32_t value = 0;
  rtems_blkdev_bnum block_count = 0;

  sc = rtems_disk_create_phys(dev, BLOCK_SIZE, BLOCK_COUNT, ramdisk_ioctl, rd, "/dev/rda");
  ASSERT_SC(sc);

  dd = rtems_disk_obtain(dev);
  rtems_test_assert(dd != NULL);

  fd = open("/dev/rda", O_RDWR);
  rtems_test_assert(fd >= 0);

  value = 0;
  rv = rtems_disk_fd_get_media_block_size(fd, &value);
  rtems_test_assert(rv == 0);
  rtems_test_assert(value == BLOCK_SIZE);

  value = 0;
  rv = rtems_disk_fd_get_block_size(fd, &value);
  rtems_test_assert(rv == 0);
  rtems_test_assert(value == BLOCK_SIZE);

  value = 1024;
  rv = rtems_disk_fd_set_block_size(fd, value);
  rtems_test_assert(rv == 0);

  value = 0;
  rv = rtems_disk_fd_get_block_size(fd, &value);
  rtems_test_assert(rv == 0);
  rtems_test_assert(value == 1024);

  block_count = 0;
  rv = rtems_disk_fd_get_block_count(fd, &block_count);
  rtems_test_assert(rv == 0);
  rtems_test_assert(block_count == BLOCK_COUNT);

  rv = rtems_disk_fd_get_disk_device(fd, &fd_dd);
  rtems_test_assert(rv == 0);
  rtems_test_assert(fd_dd == dd);

  rv = rtems_disk_fd_sync(fd);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  sc = rtems_disk_release(dd);
  ASSERT_SC(sc);

  sc = rtems_disk_delete(dev);
  ASSERT_SC(sc);
}

static void test_diskdevs(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;
  rtems_disk_device *physical_dd = NULL;
  rtems_disk_device *logical_dd = NULL;
  rtems_disk_device *dd = NULL;
  dev_t physical_dev = 0;
  dev_t logical_dev = 0;
  dev_t logical_2_dev = 0;
  dev_t const big_major_dev = rtems_filesystem_make_dev_t((rtems_device_major_number) -2, 0);
  dev_t const big_minor_dev = rtems_filesystem_make_dev_t(0, (rtems_device_minor_number) -2);
  ramdisk *const rd = ramdisk_allocate(NULL, BLOCK_SIZE, BLOCK_COUNT, false);

  rtems_test_assert(rd != NULL);

  sc = rtems_disk_io_initialize();
  ASSERT_SC(sc);

  sc = rtems_io_register_driver(0, &ramdisk_ops, &major);
  ASSERT_SC(sc);

  physical_dev = rtems_filesystem_make_dev_t(major, minor);
  logical_dev = rtems_filesystem_make_dev_t(major, minor + 1);
  logical_2_dev = rtems_filesystem_make_dev_t(major, minor + 2);

  /* Consistency checks for physical disks creation */

  sc = rtems_disk_create_phys(physical_dev, BLOCK_SIZE, BLOCK_COUNT, NULL, rd, "/dev/rda");
  ASSERT_SC_EQ(sc, RTEMS_INVALID_ADDRESS);

  sc = rtems_disk_create_phys(physical_dev, 0, BLOCK_COUNT, ramdisk_ioctl, rd, "/dev/rda");
  ASSERT_SC_EQ(sc, RTEMS_INVALID_NUMBER);

  sc = rtems_disk_create_phys(big_major_dev, BLOCK_SIZE, BLOCK_COUNT, ramdisk_ioctl, rd, "/dev/rda");
  ASSERT_SC_EQ(sc, RTEMS_NO_MEMORY);

  sc = rtems_disk_create_phys(big_minor_dev, BLOCK_SIZE, BLOCK_COUNT, ramdisk_ioctl, rd, "/dev/rda");
  ASSERT_SC_EQ(sc, RTEMS_NO_MEMORY);

  sc = rtems_disk_create_phys(physical_dev, BLOCK_SIZE, BLOCK_COUNT, ramdisk_ioctl, rd, NULL);
  ASSERT_SC(sc);

  sc = rtems_disk_create_phys(physical_dev, BLOCK_SIZE, BLOCK_COUNT, ramdisk_ioctl, rd, NULL);
  ASSERT_SC_EQ(sc, RTEMS_RESOURCE_IN_USE);

  sc = rtems_disk_delete(physical_dev);
  ASSERT_SC(sc);

  /* Consistency checks for logical disks creation */

  sc = rtems_disk_create_log(logical_dev, physical_dev, 0, 1, "/dev/rda1");
  ASSERT_SC_EQ(sc, RTEMS_INVALID_ID);

  sc = rtems_disk_create_phys(physical_dev, BLOCK_SIZE, BLOCK_COUNT, ramdisk_ioctl, rd, "/dev/rda");
  ASSERT_SC(sc);

  sc = rtems_disk_create_log(big_major_dev, physical_dev, 0, 1, "/dev/rda1");
  ASSERT_SC_EQ(sc, RTEMS_NO_MEMORY);

  sc = rtems_disk_create_log(big_minor_dev, physical_dev, 0, 1, "/dev/rda1");
  ASSERT_SC_EQ(sc, RTEMS_NO_MEMORY);

  sc = rtems_disk_create_log(logical_dev, physical_dev, BLOCK_COUNT, 0, "/dev/rda1");
  ASSERT_SC_EQ(sc, RTEMS_INVALID_NUMBER);

  sc = rtems_disk_create_log(logical_dev, physical_dev, 0, BLOCK_COUNT + 1, "/dev/rda1");
  ASSERT_SC_EQ(sc, RTEMS_INVALID_NUMBER);

  sc = rtems_disk_create_log(logical_dev, physical_dev, 1, BLOCK_COUNT, "/dev/rda1");
  ASSERT_SC_EQ(sc, RTEMS_INVALID_NUMBER);

  sc = rtems_disk_create_log(logical_dev, physical_dev, 0, 1, "/dev/rda1");
  ASSERT_SC(sc);

  sc = rtems_disk_create_log(logical_dev, physical_dev, 0, 1, "/dev/rda1");
  ASSERT_SC_EQ(sc, RTEMS_RESOURCE_IN_USE);

  sc = rtems_disk_create_log(logical_2_dev, logical_dev, 0, 1, "/dev/rda2");
  ASSERT_SC_EQ(sc, RTEMS_INVALID_ID);

  sc = rtems_disk_delete(logical_dev);
  ASSERT_SC(sc);

  /* Consistency checks delete */

  sc = rtems_disk_create_log(logical_dev, physical_dev, 0, 1, "/dev/rda1");
  ASSERT_SC(sc);

  physical_dd = rtems_disk_obtain(physical_dev);
  rtems_test_assert(physical_dd != NULL && physical_dd->uses == 2);

  sc = rtems_disk_release(physical_dd);
  ASSERT_SC(sc);

  logical_dd = rtems_disk_obtain(logical_dev);
  rtems_test_assert(logical_dd != NULL && logical_dd->uses == 1);

  sc = rtems_disk_delete(physical_dev);
  ASSERT_SC(sc);

  sc = rtems_disk_create_phys(physical_dev, BLOCK_SIZE, BLOCK_COUNT, ramdisk_ioctl, rd, "/dev/rda");
  ASSERT_SC_EQ(sc, RTEMS_RESOURCE_IN_USE);

  dd = rtems_disk_obtain(physical_dev);
  rtems_test_assert(dd == NULL);

  dd = rtems_disk_obtain(logical_dev);
  rtems_test_assert(dd == NULL);

  sc = rtems_disk_release(logical_dd);
  ASSERT_SC(sc);

  /* Test block IO control API */

  test_block_io_control_api(physical_dev, rd);

  /* Cleanup */

  sc = rtems_io_unregister_driver(major);
  ASSERT_SC(sc);

  ramdisk_free(rd);

  sc = rtems_disk_io_done();
  ASSERT_SC(sc);
}

static rtems_task Init(rtems_task_argument argument)
{
  (void) argument;

  TEST_BEGIN();

  test_diskdevs();

  TEST_END();

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_DRIVERS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
