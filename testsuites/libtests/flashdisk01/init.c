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
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include <rtems/flashdisk.h>
#include <rtems/libio.h>
#include <rtems/blkdev.h>
#include <rtems/rtems-rfs-format.h>

#include "test-file-system.h"

const char rtems_test_name[] = "FLASHDISK 1";

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define FLASHDISK_CONFIG_COUNT 1

#define FLASHDISK_DEVICE_COUNT 1

#define FLASHDISK_SEGMENT_COUNT 4U

#define FLASHDISK_SEGMENT_SIZE (16 * 1024)

#define FLASHDISK_BLOCK_SIZE 512U

#define FLASHDISK_BLOCKS_PER_SEGMENT \
  (FLASHDISK_SEGMENT_SIZE / FLASHDISK_BLOCK_SIZE)

#define FLASHDISK_SIZE \
  (FLASHDISK_SEGMENT_COUNT * FLASHDISK_SEGMENT_SIZE)

static const rtems_rfs_format_config rfs_config;

static const char device [] = "/dev/fdda";

static const char mnt [] = "/mnt";

static const char file [] = "/mnt/file";

static uint8_t flashdisk_data [FLASHDISK_SIZE];

static void flashdisk_print_status(const char *disk_path)
{
  int rv;
  int fd = open(disk_path, O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = ioctl(fd, RTEMS_FDISK_IOCTL_PRINT_STATUS);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static int test_rfs_mount_handler(
  const char *disk_path,
  const char *mount_path,
  void *arg
)
{
  (void) arg;

  return mount_and_make_target_path(
    disk_path,
    mount_path,
    RTEMS_FILESYSTEM_TYPE_RFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL
  );
}

static int test_rfs_format_handler(
  const char *disk_path,
  void *arg
)
{
  (void) arg;

  flashdisk_print_status(disk_path);

  rtems_test_assert(0);

  errno = EIO;

  return -1;
}

static const test_file_system_handler test_rfs_handler = {
  .mount = test_rfs_mount_handler,
  .format = test_rfs_format_handler
};

static void test(void)
{
  int rv;
  const void *data = NULL;

  rv = mkdir(mnt, S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(rv == 0);

  rv = rtems_rfs_format(device, &rfs_config);
  rtems_test_assert(rv == 0);

  rv = mount(
    device,
    mnt,
    RTEMS_FILESYSTEM_TYPE_RFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    data
  );
  rtems_test_assert(rv == 0);

  rv = mknod(file, S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO, 0);
  rtems_test_assert(rv == 0);

  rv = unmount(mnt);
  rtems_test_assert(rv == 0);

  test_file_system_with_handler(
    0,
    device,
    mnt,
    &test_rfs_handler,
    NULL
  );

  flashdisk_print_status(device);
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  test();

  TEST_END();

  rtems_test_exit(0);
}

static void erase_device(void)
{
  memset(&flashdisk_data [0], 0xff, FLASHDISK_SIZE);
}

static rtems_device_driver flashdisk_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  erase_device();

  return rtems_fdisk_initialize(major, minor, arg);
}

static uint8_t *get_data_pointer(
  const rtems_fdisk_segment_desc *sd,
  uint32_t segment,
  uint32_t offset
)
{
  offset += sd->offset + (segment - sd->segment) * sd->size;

  return &flashdisk_data [offset];
}

static int flashdisk_read(
  const rtems_fdisk_segment_desc *sd,
  uint32_t device,
  uint32_t segment,
  uint32_t offset,
  void *buffer,
  uint32_t size
)
{
  (void) device;

  int eno = 0;
  const uint8_t *data = get_data_pointer(sd, segment, offset);

  memcpy(buffer, data, size);

  return eno;
}

static int flashdisk_write(
  const rtems_fdisk_segment_desc *sd,
  uint32_t device,
  uint32_t segment,
  uint32_t offset,
  const void *buffer,
  uint32_t size
)
{
  (void) device;

  int eno = 0;
  uint8_t *data = get_data_pointer(sd, segment, offset);

  memcpy(data, buffer, size);

  return eno;
}

static int flashdisk_blank(
  const rtems_fdisk_segment_desc *sd,
  uint32_t device,
  uint32_t segment,
  uint32_t offset,
  uint32_t size
)
{
  (void) device;

  int eno = 0;
  const uint8_t *current = get_data_pointer(sd, segment, offset);
  const uint8_t *end = current + size;

  while (eno == 0 && current != end) {
    if (*current != 0xff) {
      eno = EIO;
    }
    ++current;
  }

  return eno;
}

static int flashdisk_verify(
  const rtems_fdisk_segment_desc *sd,
  uint32_t device,
  uint32_t segment,
  uint32_t offset,
  const void *buffer,
  uint32_t size
)
{
  (void) device;

  int eno = 0;
  uint8_t *data = get_data_pointer(sd, segment, offset);

  if (memcmp(data, buffer, size) != 0) {
    eno = EIO;
  }

  return eno;
}

static int flashdisk_erase(
  const rtems_fdisk_segment_desc *sd,
  uint32_t device,
  uint32_t segment
)
{
  (void) device;

  int eno = 0;
  uint8_t *data = get_data_pointer(sd, segment, 0);

  memset(data, 0xff, sd->size);

  return eno;
}

static int flashdisk_erase_device(
  const rtems_fdisk_device_desc *sd,
  uint32_t device
)
{
  (void) sd;
  (void) device;

  int eno = 0;

  erase_device();

  return eno;
}

static const rtems_fdisk_segment_desc flashdisk_segment_desc = {
  .count = FLASHDISK_SEGMENT_COUNT,
  .segment = 0,
  .offset = 0,
  .size = FLASHDISK_SEGMENT_SIZE
};

static const rtems_fdisk_driver_handlers flashdisk_ops = {
  .read = flashdisk_read,
  .write = flashdisk_write,
  .blank = flashdisk_blank,
  .verify = flashdisk_verify,
  .erase = flashdisk_erase,
  .erase_device = flashdisk_erase_device
};

static const rtems_fdisk_device_desc flashdisk_device = {
  .segment_count = 1,
  .segments = &flashdisk_segment_desc,
  .flash_ops = &flashdisk_ops
};

const rtems_flashdisk_config
rtems_flashdisk_configuration [FLASHDISK_CONFIG_COUNT] = {
  {
    .block_size = FLASHDISK_BLOCK_SIZE,
    .device_count = FLASHDISK_DEVICE_COUNT,
    .devices = &flashdisk_device,
    .flags = RTEMS_FDISK_CHECK_PAGES
      | RTEMS_FDISK_BLANK_CHECK_BEFORE_WRITE,
    .unavail_blocks = FLASHDISK_BLOCKS_PER_SEGMENT,
    .compact_segs = 2,
    .avail_compact_segs = 1,
    .info_level = 0
  }
};

uint32_t rtems_flashdisk_configuration_size = FLASHDISK_CONFIG_COUNT;

#define FLASHDISK_DRIVER { .initialization_entry = flashdisk_initialize }

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS FLASHDISK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_FILESYSTEM_RFS

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (32U * 1024U)

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
