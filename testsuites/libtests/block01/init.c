/**
 * @file
 *
 * @ingroup tests
 *
 * @brief Block device tests.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
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

#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include "tmacros.h"

#include <rtems.h>
#include <rtems/ramdisk.h>
#include <rtems/diskdevs.h>

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define ASSERT_SC_EQ(sc, sc_expected) rtems_test_assert((sc) == (sc_expected))

#define BLOCK_SIZE 512U

#define BLOCK_COUNT 16U

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
  puts("\n\n*** TEST BLOCK 1 ***");

  test_diskdevs();

  puts("*** END OF TEST BLOCK 1 ***");

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_DRIVERS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
