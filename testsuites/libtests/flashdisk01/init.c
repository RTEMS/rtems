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

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define FLASHDISK_CONFIG_COUNT 1

#define FLASHDISK_DEVICE_COUNT 1

#define FLASHDISK_SEGMENT_COUNT 4

#define FLASHDISK_SEGMENT_SIZE (16 * 1024)

#define FLASHDISK_BLOCK_SIZE 512

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
  puts("\n\n*** TEST FLASHDISK 1 ***");

  test();

  puts("*** END OF TEST FLASHDISK 1 ***");

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

#define FLASHDISK_DRIVER { \
  .initialization_entry = flashdisk_initialize, \
  .open_entry = rtems_blkdev_generic_open, \
  .close_entry = rtems_blkdev_generic_close, \
  .read_entry = rtems_blkdev_generic_read, \
  .write_entry = rtems_blkdev_generic_write, \
  .control_entry = rtems_blkdev_generic_ioctl \
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS FLASHDISK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_FILESYSTEM_RFS

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE (32 * 1024)

#define CONFIGURE_EXTRA_TASK_STACKS (8 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
