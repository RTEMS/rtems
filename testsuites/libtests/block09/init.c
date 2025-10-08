/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup test_bdbuf
 *
 * @brief Bdbuf test for disk driver error handling..
 */

/*
 * Copyright (C) 2009, 2018 embedded brains GmbH & Co. KG
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
#include <stdlib.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/test-info.h>

#include <bsp.h>

const char rtems_test_name[] = "BLOCK 9";

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

#define BLOCK_SIZE sizeof(char)

#define BLOCK_COUNT 4

#define BLOCK_READ_IO_ERROR 0
#define BLOCK_READ_UNSATISFIED 1
#define BLOCK_READ_SUCCESSFUL 2
#define BLOCK_WRITE_IO_ERROR 3

#define DISK_PATH "/disk"

static char disk_data [BLOCK_COUNT];

static int disk_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (req == RTEMS_BLKIO_REQUEST) {
    rtems_blkdev_request *r = arg;
    uint32_t i = 0;

    for (i = 0; i < r->bufnum; ++i) {
      rtems_blkdev_sg_buffer *sg = &r->bufs [i];
      char *buf = sg->buffer;

      if (sg->length == 1) {
        switch (r->req) {
          case RTEMS_BLKDEV_REQ_READ:
            switch (sg->block) {
              case BLOCK_READ_IO_ERROR:
                sc = RTEMS_IO_ERROR;
                break;
              case BLOCK_READ_UNSATISFIED:
                sc = RTEMS_UNSATISFIED;
                break;
              case BLOCK_READ_SUCCESSFUL:
              case BLOCK_WRITE_IO_ERROR:
                *buf = disk_data [sg->block];
                break;
              default:
                sc = RTEMS_IO_ERROR;
                break;
            }
            break;
          case RTEMS_BLKDEV_REQ_WRITE:
            switch (sg->block) {
              case BLOCK_READ_IO_ERROR:
              case BLOCK_READ_UNSATISFIED:
              case BLOCK_READ_SUCCESSFUL:
                disk_data [sg->block] = *buf;
                break;
              case BLOCK_WRITE_IO_ERROR:
                sc = RTEMS_IO_ERROR;
                break;
              default:
                sc = RTEMS_IO_ERROR;
                break;
            }
            break;
          default:
            sc = RTEMS_IO_ERROR;
            break;
        }
      } else {
        sc = RTEMS_IO_ERROR;
      }
    }

    rtems_blkdev_request_done(r, sc);

    return 0;
  } else {
    return rtems_blkdev_ioctl(dd, req, arg);
  }
}

static void disk_register(
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_disk_device **dd
)
{
  rtems_status_code sc;
  int fd;
  int rv;

  sc = rtems_blkdev_create(
    DISK_PATH,
    block_size,
    block_count,
    disk_ioctl,
    NULL
  );
  ASSERT_SC(sc);

  fd = open(DISK_PATH, O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = rtems_disk_fd_get_disk_device(fd, dd);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void check_read(
  rtems_disk_device *dd,
  rtems_blkdev_bnum block,
  rtems_status_code expected_sc
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  sc = rtems_bdbuf_read(dd, block, &bd);
  assert(sc == expected_sc);

  if (sc == RTEMS_SUCCESSFUL) {
    sc = rtems_bdbuf_release(bd);
    ASSERT_SC(sc);
  }
}

static rtems_task Init(rtems_task_argument argument)
{
  (void) argument;

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;
  rtems_disk_device *dd = NULL;

  TEST_BEGIN();

  disk_register(BLOCK_SIZE, BLOCK_COUNT, &dd);

  check_read(dd, BLOCK_READ_IO_ERROR, RTEMS_IO_ERROR);
  check_read(dd, BLOCK_READ_UNSATISFIED, RTEMS_UNSATISFIED);
  check_read(dd, BLOCK_READ_SUCCESSFUL, RTEMS_SUCCESSFUL);
  check_read(dd, BLOCK_WRITE_IO_ERROR, RTEMS_SUCCESSFUL);

  /* Check write IO error */

  sc = rtems_bdbuf_read(dd, BLOCK_WRITE_IO_ERROR, &bd);
  ASSERT_SC(sc);

  bd->buffer [0] = 1;

  sc = rtems_bdbuf_sync(bd);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_read(dd, BLOCK_WRITE_IO_ERROR, &bd);
  ASSERT_SC(sc);

  assert(bd->buffer [0] == 0);

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  /* Check write to deleted disk */

  sc = rtems_bdbuf_read(dd, BLOCK_READ_SUCCESSFUL, &bd);
  ASSERT_SC(sc);

  sc = unlink(DISK_PATH);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_sync(bd);
  ASSERT_SC(sc);

  TEST_END();

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE BLOCK_SIZE
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE BLOCK_SIZE
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (BLOCK_SIZE * BLOCK_COUNT)

#include <rtems/confdefs.h>
