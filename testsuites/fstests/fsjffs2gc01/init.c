/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <tmacros.h>
#include <fstest.h>

#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <rtems/jffs2.h>

const char rtems_test_name[] = "FSJFFS2GC 1";

static const rtems_jffs2_info info_initial = {
  .flash_size = 131072,
  .flash_blocks = 8,
  .flash_block_size = 16384,
  .used_size = 96,
  .dirty_size = 0,
  .wasted_size = 0,
  .free_size = 130976,
  .bad_size = 0,
  .clean_blocks = 0,
  .dirty_blocks = 0,
  .erasable_blocks = 0,
  .free_blocks = 8,
  .bad_blocks = 0
};

static const rtems_jffs2_info info_big_created = {
  .flash_size = 131072,
  .flash_blocks = 8,
  .flash_block_size = 16384,
  .used_size = 22292,
  .dirty_size = 0,
  .wasted_size = 164,
  .free_size = 108616,
  .bad_size = 0,
  .clean_blocks = 1,
  .dirty_blocks = 0,
  .erasable_blocks = 0,
  .free_blocks = 7,
  .bad_blocks = 0
};

static const rtems_jffs2_info info_big_removed = {
  .flash_size = 131072,
  .flash_blocks = 8,
  .flash_block_size = 16384,
  .used_size = 72,
  .dirty_size = 16384,
  .wasted_size = 6000,
  .free_size = 108616,
  .bad_size = 0,
  .clean_blocks = 0,
  .dirty_blocks = 0,
  .erasable_blocks = 1,
  .free_blocks = 7,
  .bad_blocks = 0
};

static const rtems_jffs2_info info_more_files_created = {
  .flash_size = 131072,
  .flash_blocks = 8,
  .flash_block_size = 16384,
  .used_size = 54896,
  .dirty_size = 23336,
  .wasted_size = 36,
  .free_size = 52804,
  .bad_size = 0,
  .clean_blocks = 2,
  .dirty_blocks = 1,
  .erasable_blocks = 1,
  .free_blocks = 4,
  .bad_blocks = 0
};

static const rtems_jffs2_info info_some_files_removed = {
  .flash_size = 131072,
  .flash_blocks = 8,
  .flash_block_size = 16384,
  .used_size = 23528,
  .dirty_size = 47372,
  .wasted_size = 7368,
  .free_size = 52804,
  .bad_size = 0,
  .clean_blocks = 0,
  .dirty_blocks = 3,
  .erasable_blocks = 1,
  .free_blocks = 4,
  .bad_blocks = 0
};

static const rtems_jffs2_info info_after_first_gc = {
  .flash_size = 131072,
  .flash_blocks = 8,
  .flash_block_size = 16384,
  .used_size = 23540,
  .dirty_size = 30988,
  .wasted_size = 7368,
  .free_size = 69176,
  .bad_size = 0,
  .clean_blocks = 0,
  .dirty_blocks = 3,
  .erasable_blocks = 0,
  .free_blocks = 5,
  .bad_blocks = 0
};

static const rtems_jffs2_info info_after_excessive_gc = {
  .flash_size = 131072,
  .flash_blocks = 8,
  .flash_block_size = 16384,
  .used_size = 7224,
  .dirty_size = 0,
  .wasted_size = 12,
  .free_size = 123836,
  .bad_size = 0,
  .clean_blocks = 0,
  .dirty_blocks = 0,
  .erasable_blocks = 0,
  .free_blocks = 8,
  .bad_blocks = 0
};

static char big[] = "big";

static const char * const more[] = {
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7"
};

#define ASSERT_INFO(a, b) do { \
  rv = ioctl(fd, RTEMS_JFFS2_GET_INFO, &info); \
  rtems_test_assert(rv == 0); \
  rtems_test_assert(memcmp(a, b, sizeof(*a)) == 0); \
} while (0)

static const mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

static char keg[523];

static uint32_t simple_random(uint32_t v)
{
  v *= 1664525;
  v += 1013904223;

  return v;
}

static void init_keg(void)
{
  size_t i;
  uint32_t v;

  v = 123;
  for (i = 0; i < sizeof(keg); ++i) {
    v = simple_random(v);
    keg[i] = (uint8_t) (v >> 23);
  }
}

static void write_kegs(int fd, int kegs)
{
  int i;

  for (i = 0; i < kegs; ++i) {
    ssize_t n;

    n = write(fd, &keg[0], sizeof(keg));
    rtems_test_assert(n == (ssize_t) sizeof(keg));
  }
}

static void create_big_file(void)
{
  int fd;
  int rv;

  fd = open(&big[0], O_WRONLY | O_TRUNC | O_CREAT, mode);
  rtems_test_assert(fd >= 0);

  write_kegs(fd, 37);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void remove_big_file(void)
{
  int rv;

  rv = unlink(&big[0]);
  rtems_test_assert(rv == 0);
}

static void create_more_files(void)
{
  int fds[RTEMS_ARRAY_SIZE(more)];
  int rv;
  size_t i;
  int j;

  for (i = 0; i < RTEMS_ARRAY_SIZE(fds); ++i) {
    fds[i] = open(more[i], O_WRONLY | O_TRUNC | O_CREAT, mode);
    rtems_test_assert(fds[i] >= 0);
  }

  for (j = 0; j < 13; ++j) {
    for (i = 0; i < RTEMS_ARRAY_SIZE(fds); ++i) {
      write_kegs(fds[i], 1);
    }
  }

  for (i = 0; i < RTEMS_ARRAY_SIZE(fds); ++i) {
    rv = close(fds[i]);
    rtems_test_assert(rv == 0);
  }
}

static void remove_some_files(void)
{
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE(more); i += 2) {
    int rv;

    rv = unlink(more[i]);
    rtems_test_assert(rv == 0);
  }
}

void test(void)
{
  int fd;
  int rv;
  int counter;
  rtems_jffs2_info info;

  init_keg();

  /*
   * Ensure that jiffies != 0, to use most likely path in
   * jffs2_mark_node_obsolete().
   */
  while (rtems_clock_get_ticks_since_boot() == 0) {
    /* Wait */
  }

  fd = open("/", O_RDONLY);
  rtems_test_assert(fd >= 0);

  ASSERT_INFO(&info, &info_initial);

  create_big_file();
  ASSERT_INFO(&info, &info_big_created);

  remove_big_file();
  ASSERT_INFO(&info, &info_big_removed);

  create_more_files();
  ASSERT_INFO(&info, &info_more_files_created);

  remove_some_files();
  ASSERT_INFO(&info, &info_some_files_removed);

  rv = ioctl(fd, RTEMS_JFFS2_ON_DEMAND_GARBAGE_COLLECTION);
  rtems_test_assert(rv == 0);
  ASSERT_INFO(&info, &info_after_first_gc);

  rv = ioctl(fd, RTEMS_JFFS2_ON_DEMAND_GARBAGE_COLLECTION);
  rtems_test_assert(rv == 0);
  ASSERT_INFO(&info, &info_after_first_gc);

  counter = 0;

  while (true) {
    errno = ENXIO;
    rv = ioctl(fd, RTEMS_JFFS2_FORCE_GARBAGE_COLLECTION);
    if (rv == -1) {
      rtems_test_assert(errno == EIO);
      break;
    }

    ++counter;
    rtems_test_assert(rv == 0);
  }

  rtems_test_assert(counter == 19);

  rv = ioctl(fd, RTEMS_JFFS2_GET_INFO, &info);
  rtems_test_assert(rv == 0);
  ASSERT_INFO(&info, &info_after_excessive_gc);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}
