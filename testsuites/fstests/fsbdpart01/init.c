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

#include <sys/stat.h>
#include <fcntl.h>

#include <rtems/bdpart.h>
#include <rtems/blkdev.h>
#include <rtems/ide_part_table.h>
#include <rtems/ramdisk.h>

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define PARTITION_COUNT 9

static const char rda [] = "/dev/rda";

static const char *const bdpart_rdax [PARTITION_COUNT] = {
  "/dev/rda1",
  "/dev/rda2",
  "/dev/rda3",
  "/dev/rda4",
  "/dev/rda5",
  "/dev/rda6",
  "/dev/rda7",
  "/dev/rda8",
  "/dev/rda9"
};

static const char *const ide_part_table_rdax [PARTITION_COUNT] = {
  "/dev/rda1",
  "/dev/rda2",
  "/dev/rda3",
  "/dev/rda5",
  "/dev/rda6",
  "/dev/rda7",
  "/dev/rda8",
  "/dev/rda9",
  "/dev/rda10"
};

static const rtems_blkdev_bnum starts [PARTITION_COUNT] = {
  63, 126, 189, 315, 441, 567, 693, 819, 945
};

static const rtems_bdpart_format format = {
  .mbr = {
    .type = RTEMS_BDPART_FORMAT_MBR,
    .disk_id = 0xdeadbeef,
    .dos_compatibility = true
  }
};

static const unsigned distribution [PARTITION_COUNT] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1
};

static void test_logical_disks(const char *const *rdax, bool exists)
{
  size_t i = 0;

  for (i = 0; i < PARTITION_COUNT; ++i) {
    int fd = open(rdax [i], O_RDONLY);

    if (exists) {
      rtems_disk_device *dd = NULL;
      int rv = 0;

      rtems_test_assert(fd >= 0);

      rv = rtems_disk_fd_get_disk_device(fd, &dd);
      rtems_test_assert(rv == 0);

      rtems_test_assert(dd->start == starts [i]);
      rtems_test_assert(dd->size == 63);

      rv = close(fd);
      rtems_test_assert(rv == 0);
    } else {
      rtems_test_assert(fd == -1);
    }
  }
}

static void test_bdpart(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdpart_partition created_partitions [PARTITION_COUNT];
  rtems_bdpart_format actual_format;
  rtems_bdpart_partition actual_partitions [PARTITION_COUNT];
  size_t actual_count = PARTITION_COUNT;
  size_t i = 0;

  memset(&created_partitions [0], 0, sizeof(created_partitions));
  memset(&actual_format, 0, sizeof(actual_format));
  memset(&actual_partitions [0], 0, sizeof(actual_partitions));

  for (i = 0; i < PARTITION_COUNT; ++i) {
    rtems_bdpart_to_partition_type(
      RTEMS_BDPART_MBR_FAT_32,
      created_partitions [i].type
    );
  }

  sc = rtems_bdpart_create(
    rda,
    &format,
    &created_partitions [0],
    &distribution [0],
    PARTITION_COUNT
  );
  ASSERT_SC(sc);

  sc = rtems_bdpart_write(
    rda,
    &format,
    &created_partitions [0],
    PARTITION_COUNT
  );
  ASSERT_SC(sc);

  sc = rtems_bdpart_read(
    rda,
    &actual_format,
    &actual_partitions [0],
    &actual_count
  );
  ASSERT_SC(sc);
  rtems_test_assert(actual_format.mbr.disk_id == format.mbr.disk_id);
  rtems_test_assert(
    memcmp(
      &actual_partitions [0],
      &created_partitions [0],
      PARTITION_COUNT
    ) == 0
  );

  sc = rtems_bdpart_register(
    rda,
    actual_partitions,
    actual_count
  );
  ASSERT_SC(sc);
  test_logical_disks(&bdpart_rdax [0], true);

  sc = rtems_bdpart_unregister(
    rda,
    actual_partitions,
    actual_count
  );
  ASSERT_SC(sc);
  test_logical_disks(&bdpart_rdax [0], false);

  sc = rtems_bdpart_register_from_disk(rda);
  ASSERT_SC(sc);
  test_logical_disks(&bdpart_rdax [0], true);

  sc = rtems_bdpart_unregister(
    rda,
    actual_partitions,
    actual_count
  );
  ASSERT_SC(sc);
  test_logical_disks(&bdpart_rdax [0], false);

  rtems_bdpart_dump(&actual_partitions [0], actual_count);
}

static void test_ide_part_table(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  test_logical_disks(&ide_part_table_rdax [0], false);
  sc = rtems_ide_part_table_initialize(rda);
  ASSERT_SC(sc);
  test_logical_disks(&ide_part_table_rdax [0], true);
}

static void Init(rtems_task_argument arg)
{
  printf("\n\n*** TEST FSBDPART 1 ***\n");

  test_bdpart();
  test_ide_part_table();

  printf("*** END OF TEST FSBDPART 1 ***\n");

  rtems_test_exit(0);
}

rtems_ramdisk_config rtems_ramdisk_configuration [] = {
  { .block_size = 512, .block_num = 1024 }
};

size_t rtems_ramdisk_configuration_size = 1;

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS RAMDISK_DRIVER_TABLE_ENTRY
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_EXTRA_TASK_STACKS (8 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
