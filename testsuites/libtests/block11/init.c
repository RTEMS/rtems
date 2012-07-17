/**
 * @file
 *
 * @ingroup tests
 *
 * @brief Block device tests.
 */

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
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/ramdisk.h>
#include <rtems/diskdevs.h>
#include <rtems/malloc.h>

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define CHUNK_MAX 32U

#define AREA_SIZE ((CHUNK_MAX * (CHUNK_MAX + 1U)) / 2U)

#define BLOCK_SIZE 4U

#define BLOCK_COUNT (AREA_SIZE / BLOCK_SIZE)

static const char rda [] = "/dev/rda";

static const char rda1 [] = "/dev/rda1";

static const char not_exist [] = "not_exist";

static const char not_blkdev [] = "not_blkdev";

static const char invalid_blkdev [] = "invalid_blkdev";

static long area_a [AREA_SIZE / sizeof(long)];

static long area_b [AREA_SIZE / sizeof(long)];

static void area_init(long *area)
{
  size_t i;
  size_t n = AREA_SIZE / sizeof(long);

  for (i = 0; i < n; ++i) {
    area [i] = mrand48();
  }
}

static void area_read(int fd, long *area)
{
  size_t i;
  size_t n = CHUNK_MAX;
  char *dst = (char *) area;

  for (i = 0; i <= n; ++i) {
    ssize_t m = read(fd, dst, i);
    rtems_test_assert(m == (ssize_t) i);
    dst += i;
  }
}

static void area_write(int fd, const long *area)
{
  size_t i;
  size_t n = CHUNK_MAX;
  const char *src = (const char *) area;

  for (i = 0; i <= n; ++i) {
    ssize_t m = write(fd, src, i);
    rtems_test_assert(m == (ssize_t) i);
    src += i;
  }
}

static void area_compare(const long *area_a, const long *area_b, bool equal)
{
  bool actual_equal = memcmp(area_a, area_b, AREA_SIZE) == 0;
  rtems_test_assert(actual_equal == equal);
}

static void test_blkdev_imfs_read_and_write(void)
{
  rtems_status_code sc;
  int rv;
  ramdisk *rd;
  int fd;
  off_t off;

  rd = ramdisk_allocate(area_a, BLOCK_SIZE, BLOCK_COUNT, false);
  rtems_test_assert(rd != NULL);

  ramdisk_enable_free_at_delete_request(rd);

  sc = rtems_blkdev_create(
    rda,
    BLOCK_SIZE,
    BLOCK_COUNT,
    ramdisk_ioctl,
    rd
  );
  ASSERT_SC(sc);

  fd = open(rda, O_RDWR);
  rtems_test_assert(fd >= 0);

  area_init(area_a);
  area_read(fd, area_b);
  area_compare(area_a, area_b, true);

  off = lseek(fd, 0, SEEK_SET);
  rtems_test_assert(off == 0);

  area_init(area_b);
  area_write(fd, area_b);
  area_compare(area_a, area_b, false);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  rv = unlink(rda);
  rtems_test_assert(rv == 0);

  area_compare(area_a, area_b, true);
}

static void test_blkdev_imfs_parameters(void)
{
  rtems_status_code sc;
  int rv;
  ramdisk *rd;
  int fd;
  rtems_disk_device *dd;
  struct stat st;

  rd = ramdisk_allocate(NULL, BLOCK_SIZE, BLOCK_COUNT, false);
  rtems_test_assert(rd != NULL);

  ramdisk_enable_free_at_delete_request(rd);

  sc = rtems_blkdev_create(
    rda,
    BLOCK_SIZE,
    BLOCK_COUNT,
    ramdisk_ioctl,
    rd
  );
  ASSERT_SC(sc);

  sc = rtems_blkdev_create_partition(
    rda1,
    rda,
    1,
    BLOCK_COUNT - 1
  );
  ASSERT_SC(sc);

  fd = open(rda, O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = fstat(fd, &st);
  rtems_test_assert(rv == 0);

  rv = rtems_disk_fd_get_disk_device(fd, &dd);
  rtems_test_assert(rv == 0);

  rtems_test_assert(rtems_disk_get_driver_data(dd) == rd);
  rtems_test_assert(rtems_disk_get_device_identifier(dd) == st.st_rdev);
  rtems_test_assert(rtems_disk_get_media_block_size(dd) == BLOCK_SIZE);
  rtems_test_assert(rtems_disk_get_block_size(dd) == BLOCK_SIZE);
  rtems_test_assert(rtems_disk_get_block_begin(dd) == 0);
  rtems_test_assert(rtems_disk_get_block_count(dd) == BLOCK_COUNT);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  fd = open(rda1, O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = fstat(fd, &st);
  rtems_test_assert(rv == 0);

  rv = rtems_disk_fd_get_disk_device(fd, &dd);
  rtems_test_assert(rv == 0);

  rtems_test_assert(rtems_disk_get_driver_data(dd) == rd);
  rtems_test_assert(rtems_disk_get_device_identifier(dd) == st.st_rdev);
  rtems_test_assert(rtems_disk_get_media_block_size(dd) == BLOCK_SIZE);
  rtems_test_assert(rtems_disk_get_block_size(dd) == BLOCK_SIZE);
  rtems_test_assert(rtems_disk_get_block_begin(dd) == 1);
  rtems_test_assert(rtems_disk_get_block_count(dd) == BLOCK_COUNT - 1);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  rv = unlink(rda1);
  rtems_test_assert(rv == 0);

  rv = unlink(rda);
  rtems_test_assert(rv == 0);
}

static void test_blkdev_imfs_errors(void)
{
  static uintptr_t disk_size [] = { sizeof(rtems_disk_device) + sizeof(int) };

  rtems_status_code sc;
  int rv;
  ramdisk *rd;
  void *opaque;

  rd = ramdisk_allocate(NULL, BLOCK_SIZE, BLOCK_COUNT, false);
  rtems_test_assert(rd != NULL);

  ramdisk_enable_free_at_delete_request(rd);

  sc = rtems_blkdev_create(
    rda,
    0,
    BLOCK_COUNT,
    ramdisk_ioctl,
    rd
  );
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_blkdev_create(
    rda,
    BLOCK_SIZE,
    0,
    ramdisk_ioctl,
    rd
  );
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  opaque = rtems_heap_greedy_allocate(NULL, 0);
  sc = rtems_blkdev_create(
    rda,
    BLOCK_SIZE,
    BLOCK_COUNT,
    ramdisk_ioctl,
    rd
  );
  rtems_test_assert(sc == RTEMS_NO_MEMORY);
  rtems_heap_greedy_free(opaque);

  opaque = rtems_heap_greedy_allocate(disk_size, 1);
  sc = rtems_blkdev_create(
    rda,
    BLOCK_SIZE,
    BLOCK_COUNT,
    ramdisk_ioctl,
    rd
  );
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
  rtems_heap_greedy_free(opaque);

  sc = rtems_blkdev_create(
    rda,
    BLOCK_SIZE,
    BLOCK_COUNT,
    ramdisk_ioctl,
    rd
  );
  ASSERT_SC(sc);

  sc = rtems_blkdev_create_partition(
    rda1,
    not_exist,
    0,
    BLOCK_COUNT
  );
  rtems_test_assert(sc == RTEMS_INVALID_ID);

  rv = mknod(not_blkdev, S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO, 0);
  rtems_test_assert(rv == 0);

  sc = rtems_blkdev_create_partition(
    rda1,
    not_blkdev,
    0,
    BLOCK_COUNT
  );
  rtems_test_assert(sc == RTEMS_INVALID_NODE);

  rv = mknod(invalid_blkdev, S_IFBLK | S_IRWXU | S_IRWXG | S_IRWXO, 0);
  rtems_test_assert(rv == 0);

  sc = rtems_blkdev_create_partition(
    rda1,
    invalid_blkdev,
    0,
    BLOCK_COUNT
  );
  rtems_test_assert(sc == RTEMS_NOT_IMPLEMENTED);

  sc = rtems_blkdev_create_partition(
    rda1,
    rda,
    0,
    0
  );
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_blkdev_create_partition(
    rda1,
    rda,
    BLOCK_COUNT,
    BLOCK_COUNT
  );
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_blkdev_create_partition(
    rda1,
    rda,
    0,
    BLOCK_COUNT + 1
  );
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  opaque = rtems_heap_greedy_allocate(NULL, 0);
  sc = rtems_blkdev_create_partition(
    rda1,
    rda,
    0,
    BLOCK_COUNT
  );
  rtems_test_assert(sc == RTEMS_NO_MEMORY);
  rtems_heap_greedy_free(opaque);

  opaque = rtems_heap_greedy_allocate(disk_size, 1);
  sc = rtems_blkdev_create_partition(
    rda1,
    rda,
    0,
    BLOCK_COUNT
  );
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
  rtems_heap_greedy_free(opaque);

  rv = unlink(rda);
  rtems_test_assert(rv == 0);
}

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc;

  puts("\n\n*** TEST BLOCK 11 ***");

  sc = rtems_disk_io_initialize();
  ASSERT_SC(sc);

  test_blkdev_imfs_read_and_write();
  test_blkdev_imfs_parameters();
  test_blkdev_imfs_errors();

  sc = rtems_disk_io_done();
  ASSERT_SC(sc);

  puts("*** END OF TEST BLOCK 11 ***");

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_DRIVERS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
