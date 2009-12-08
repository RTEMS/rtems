/**
 * @file
 *
 * @ingroup tests
 *
 * @brief Block device tests.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <stdio.h>
#include "tmacros.h"

#include <rtems.h>
#include <rtems/ramdisk.h>
#include <rtems/diskdevs.h>

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define ASSERT_SC_EQ(sc, sc_expected) rtems_test_assert((sc) == (sc_expected))

#define BLOCK_SIZE 512U

#define BLOCK_COUNT 16U

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

  sc = rtems_disk_create_log(logical_2_dev, logical_dev, 0, 1, "/dev/rda1");
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

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_DRIVERS 2
#define CONFIGURE_MAXIMUM_SEMAPHORES 5
#define CONFIGURE_EXTRA_TASK_STACKS (8 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
