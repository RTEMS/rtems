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

#include <rtems/bdpart.h>
#include <rtems/blkdev.h>
#include <rtems/ide_part_table.h>
#include <rtems/ramdisk.h>
#include <rtems/libcsupport.h>

const char rtems_test_name[] = "FSBDPART 1";

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
  rtems_resource_snapshot before;
  size_t actual_count = PARTITION_COUNT;
  size_t i = 0;

  memset(&created_partitions [0], 0, sizeof(created_partitions));
  memset(&actual_format, 0, sizeof(actual_format));
  memset(&actual_partitions [0], 0, sizeof(actual_partitions));

  rtems_resource_snapshot_take(&before);

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

  rtems_test_assert(rtems_resource_snapshot_check(&before));

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

  rtems_test_assert(rtems_resource_snapshot_check(&before));

  rtems_bdpart_dump(&actual_partitions [0], actual_count);
}

static void test_ide_part_table(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  test_logical_disks(&ide_part_table_rdax [0], false);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  sc = rtems_ide_part_table_initialize(rda);
#pragma GCC diagnostic pop
  ASSERT_SC(sc);
  test_logical_disks(&ide_part_table_rdax [0], true);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_bdpart();
  test_ide_part_table();

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

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 13

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
