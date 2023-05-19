/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012, 2018 embedded brains GmbH & Co. KG
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
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <rtems/blkdev.h>
#include <rtems/bdbuf.h>

const char rtems_test_name[] = "BLOCK 12";

#define BLOCK_COUNT 15

#define DISK_PATH "/disk"

static int block_access_counts [BLOCK_COUNT];

static int expected_block_access_counts [BLOCK_COUNT] = {
  2, 0, 2, 0, 2, 0, 2, 2, 2, 0, 2, 0, 2, 0, 2
};

static const rtems_blkdev_bnum read_sequence [] = {
  0, 2, 4, 6, 8, 10, 12, 14, 7
};

static int test_disk_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{
  int rv = 0;

  if (req == RTEMS_BLKIO_REQUEST) {
    rtems_blkdev_request *breq = arg;
    rtems_blkdev_sg_buffer *sg = breq->bufs;
    uint32_t i;

    rtems_test_assert(breq->req == RTEMS_BLKDEV_REQ_READ);

    for (i = 0; i < breq->bufnum; ++i) {
      rtems_blkdev_bnum block = sg [i].block;

      rtems_test_assert(block < BLOCK_COUNT);

      ++block_access_counts [block];
    }

    rtems_blkdev_request_done(breq, RTEMS_SUCCESSFUL);
  } else {
    rv = rtems_blkdev_ioctl(dd, req, arg);
  }

  return rv;
}

static void do_read_sequence(rtems_disk_device *dd)
{
  size_t i;

  for (i = 0; i < sizeof(read_sequence) / sizeof(read_sequence [0]); ++i) {
    rtems_status_code sc;
    rtems_bdbuf_buffer *bd;

    sc = rtems_bdbuf_read(dd, read_sequence [i], &bd);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_bdbuf_release(bd);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void test(void)
{
  rtems_status_code sc;
  int fd;
  int rv;
  rtems_disk_device *dd;

  sc = rtems_blkdev_create(
    DISK_PATH,
    1,
    BLOCK_COUNT,
    test_disk_ioctl,
    NULL
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  fd = open(DISK_PATH, O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = rtems_disk_fd_get_disk_device(fd, &dd);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  do_read_sequence(dd);
  rtems_bdbuf_purge_dev(dd);
  do_read_sequence(dd);

  rtems_test_assert(
    memcmp(
      block_access_counts,
      expected_block_access_counts,
      sizeof(block_access_counts)
    ) == 0
  );

  rv = unlink(DISK_PATH);
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

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE 1
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE 1
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE BLOCK_COUNT

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
