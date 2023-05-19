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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <rtems/ramdisk.h>
#include <rtems/bdbuf.h>

const char rtems_test_name[] = "BLOCK 16";

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

static void test(void)
{
  static const char device [] = "/dev/rda";
  rtems_status_code sc;
  int fd;
  int rv;
  rtems_disk_device *dd;
  ramdisk *rd;
  unsigned char buf [8];
  uint32_t media_block_size = sizeof(buf [0]);
  rtems_blkdev_bnum media_block_count = sizeof(buf) / sizeof(buf [0]);
  rtems_blkdev_bnum media_size = media_block_size * media_block_count;
  rtems_bdbuf_buffer *media_bd [media_block_count];
  rtems_bdbuf_buffer *large_bd;
  rtems_blkdev_bnum i;

  for (i = 0; i < media_block_count; ++i) {
    buf [i] = i;
  }

  rd = ramdisk_allocate(buf, media_block_size, media_block_count, false);
  rtems_test_assert(rd != NULL);

  sc = rtems_blkdev_create(
    device,
    media_block_size,
    media_block_count,
    ramdisk_ioctl,
    rd
  );
  ASSERT_SC(sc);

  fd = open(device, O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = rtems_disk_fd_get_disk_device(fd, &dd);
  rtems_test_assert(rv == 0);

  for (i = 0; i < media_block_count; ++i) {
    sc = rtems_bdbuf_read(dd, i, &media_bd [i]);
    ASSERT_SC(sc);

    rtems_test_assert(media_bd [i]->buffer [0] == buf [i]);
  }

  for (i = 0; i < media_block_count; ++i) {
    media_bd [i]->buffer [0] += media_block_count;

    sc = rtems_bdbuf_release_modified(media_bd [i]);
    ASSERT_SC(sc);
  }

  for (i = 0; i < media_block_count; i += 2) {
    sc = rtems_bdbuf_read(dd, i, &media_bd [i]);
    ASSERT_SC(sc);

    rtems_test_assert(media_bd [i]->buffer [0] == buf [i] + media_block_count);
  }

  sc = rtems_bdbuf_set_block_size(dd, media_size, true);
  ASSERT_SC(sc);

  for (i = 0; i < media_block_count; i += 2) {
    rtems_test_assert(media_bd [i]->state == RTEMS_BDBUF_STATE_ACCESS_PURGED);
    rtems_test_assert(media_bd [i + 1]->state == RTEMS_BDBUF_STATE_FREE);

    rtems_test_assert(media_bd [i]->buffer [0] == buf [i] + media_block_count);
    rtems_test_assert(media_bd [i + 1]->buffer [0] == buf [i + 1]);

    sc = rtems_bdbuf_release(media_bd [i]);
    ASSERT_SC(sc);
  }

  sc = rtems_bdbuf_read(dd, 0, &large_bd);
  ASSERT_SC(sc);

  rtems_test_assert(memcmp(buf, large_bd->buffer, media_size) == 0);

  sc = rtems_bdbuf_release(large_bd);
  ASSERT_SC(sc);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  rv = unlink(device);
  rtems_test_assert(rv == 0);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE 1
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE 8
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE 8

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
