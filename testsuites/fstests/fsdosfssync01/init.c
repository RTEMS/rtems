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

#include <rtems/libio.h>
#include <rtems/blkdev.h>
#include <rtems/dosfs.h>
#include <rtems/ramdisk.h>

static void create_file(const char *file)
{
  int fd;
  int rv;

  fd = creat(file, S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(fd >= 0);

  rv = fsync(fd);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void write_to_file(const char *file, bool sync)
{
  int fd;
  char buf [1];
  ssize_t n;
  off_t pos;
  int rv;

  fd = open(file, O_RDWR);
  rtems_test_assert(fd >= 0);

  n = write(fd, buf, sizeof(buf));
  rtems_test_assert(n == (ssize_t) sizeof(buf));

  pos = lseek(fd, 0, SEEK_END);
  rtems_test_assert(pos == sizeof(buf));

  if (sync) {
    rv = fsync(fd);
    rtems_test_assert(rv == 0);
  }

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void check_file_size(const char *file, off_t size)
{
  int fd;
  off_t pos;
  int rv;

  fd = open(file, O_RDWR);
  rtems_test_assert(fd >= 0);

  pos = lseek(fd, 0, SEEK_END);
  rtems_test_assert(pos == size);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void test(const char *rda, const char *mnt, const char *file)
{
  rtems_status_code sc;
  int disk_fd;
  int rv;

  sc = rtems_disk_io_initialize();
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  disk_fd = open(rda, O_RDWR);
  rtems_test_assert(disk_fd >= 0);

  rv = msdos_format(rda, NULL);
  rtems_test_assert(rv == 0);

  rv = mount_and_make_target_path(
    rda,
    mnt,
    RTEMS_FILESYSTEM_TYPE_DOSFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL
  );
  rtems_test_assert(rv == 0);

  create_file(file);
  rv = rtems_disk_fd_purge(disk_fd);
  rtems_test_assert(rv == 0);

  write_to_file(file, false);
  rv = rtems_disk_fd_purge(disk_fd);
  rtems_test_assert(rv == 0);
  check_file_size(file, 0);

  write_to_file(file, true);
  rv = rtems_disk_fd_purge(disk_fd);
  rtems_test_assert(rv == 0);
  check_file_size(file, 1);

  rv = unmount(mnt);
  rtems_test_assert(rv == 0);

  rv = close(disk_fd);
  rtems_test_assert(rv == 0);
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST FSDOSFSSYNC 1 ***");

  test("/dev/rda", "/mnt", "/mnt/file");

  puts("*** END OF TEST FSDOSFSSYNC 1 ***");

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

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_EXTRA_TASK_STACKS (8 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
