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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems/bdbuf.h>

const char rtems_test_name[] = "BLOCK 15";

#define BLOCK_COUNT 20

#define ACTION_COUNT 8

#define REQUEST_COUNT 4

#define WRITE_COUNT 16

#define MAX_WRITE_BLOCKS 3

#define MEDIA_BLOCK_SIZE 2

#define BLOCK_SIZE 4

#define DISK_PATH "/disk"

static const rtems_blkdev_bnum action_sequence [ACTION_COUNT] = {
  2, 1, 0, 4, 5, 6, 7, 9
};

static size_t request_index;

static const uint32_t expected_request_bufnums [REQUEST_COUNT] = {
  3, 3, 1, 1
};

static size_t write_index;

static const rtems_blkdev_bnum expected_write_blocks [WRITE_COUNT] = {
  0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15, 18, 19
};

static int test_disk_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{
  int rv = 0;

  if (req == RTEMS_BLKIO_REQUEST) {
    rtems_blkdev_request *breq = arg;
    uint32_t i;

    printf("REQ %" PRIu32 "\n", breq->bufnum);

    rtems_test_assert(breq->req == RTEMS_BLKDEV_REQ_WRITE);
    rtems_test_assert(breq->bufnum == expected_request_bufnums [request_index]);
    ++request_index;

    for (i = 0; i < breq->bufnum; ++i) {
      rtems_blkdev_sg_buffer *sg = &breq->bufs [i];
      rtems_blkdev_bnum j;

      printf("W %" PRIu32 "\n", sg->block);

      rtems_test_assert(sg->block < BLOCK_COUNT);
      rtems_test_assert(sg->length == BLOCK_SIZE);

      for (j = 0; j < BLOCK_SIZE / MEDIA_BLOCK_SIZE; ++j) {
        rtems_test_assert(expected_write_blocks [write_index] == sg->block + j);
        ++write_index;
      }
    }

    rtems_blkdev_request_done(breq, RTEMS_SUCCESSFUL);
  } else if (req == RTEMS_BLKIO_CAPABILITIES) {
    *(uint32_t *) arg = RTEMS_BLKDEV_CAP_MULTISECTOR_CONT;
  } else {
    rv = rtems_blkdev_ioctl(dd, req, arg);
  }

  return rv;
}

static void test_write_requests(rtems_disk_device *dd)
{
  rtems_status_code sc;
  int i;

  sc = rtems_bdbuf_set_block_size(dd, BLOCK_SIZE, true);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < ACTION_COUNT; ++i) {
    rtems_blkdev_bnum block = action_sequence [i];
    rtems_bdbuf_buffer *bd;

    sc = rtems_bdbuf_get(dd, block, &bd);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_bdbuf_release_modified(bd);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_bdbuf_syncdev(dd);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(request_index == REQUEST_COUNT);
  rtems_test_assert(write_index == WRITE_COUNT);
}

static void test(void)
{
  rtems_status_code sc;
  rtems_disk_device *dd;
  int fd;
  int rv;

  sc = rtems_blkdev_create(
    DISK_PATH,
    MEDIA_BLOCK_SIZE,
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

  test_write_requests(dd);

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
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE 4
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (BLOCK_COUNT * BLOCK_SIZE)
#define CONFIGURE_BDBUF_MAX_WRITE_BLOCKS MAX_WRITE_BLOCKS

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
