/*
 * Copyright (c) 2012, 2018 embedded brains GmbH.  All rights reserved.
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

#include "tmacros.h"

#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <rtems/bdbuf.h>

const char rtems_test_name[] = "BLOCK 14";

#define ACTION_COUNT 7

#define BLOCK_COUNT 6

#define DISK_PATH "/disk"

typedef struct {
  rtems_blkdev_bnum block;
  rtems_status_code (*get)(
    rtems_disk_device *dd,
    rtems_blkdev_bnum block,
    rtems_bdbuf_buffer **bd_ptr
  );
  rtems_status_code expected_get_status;
  rtems_status_code (*release)(rtems_bdbuf_buffer *bd);
} test_action;

static const test_action actions [ACTION_COUNT] = {
  { 0, rtems_bdbuf_read, RTEMS_SUCCESSFUL, rtems_bdbuf_release },
  { 1, rtems_bdbuf_read, RTEMS_SUCCESSFUL, rtems_bdbuf_release },
  { 2, rtems_bdbuf_read, RTEMS_SUCCESSFUL, rtems_bdbuf_release },
  { 0, rtems_bdbuf_read, RTEMS_SUCCESSFUL, rtems_bdbuf_release },
  { 4, rtems_bdbuf_get, RTEMS_SUCCESSFUL, rtems_bdbuf_sync },
  { 5, rtems_bdbuf_read, RTEMS_IO_ERROR, rtems_bdbuf_release },
  { 5, rtems_bdbuf_get, RTEMS_SUCCESSFUL, rtems_bdbuf_sync }
};

#define STATS(a, b, c, d, e, f, g, h) \
  { \
    .read_hits = a, \
    .read_misses = b, \
    .read_ahead_transfers = c, \
    .read_blocks = d, \
    .read_errors = e, \
    .write_transfers = f, \
    .write_blocks = g, \
    .write_errors = h \
  }

static const rtems_blkdev_stats expected_stats [ACTION_COUNT] = {
  STATS(0, 1, 0, 1, 0, 0, 0, 0),
  STATS(0, 2, 1, 3, 0, 0, 0, 0),
  STATS(1, 2, 2, 4, 0, 0, 0, 0),
  STATS(2, 2, 2, 4, 0, 0, 0, 0),
  STATS(2, 2, 2, 4, 0, 1, 1, 0),
  STATS(2, 3, 2, 5, 1, 1, 1, 0),
  STATS(2, 3, 2, 5, 1, 2, 2, 1)
};

static const int expected_block_access_counts [ACTION_COUNT] [BLOCK_COUNT] = {
   { 1, 0, 0, 0, 0, 0 },
   { 1, 1, 1, 0, 0, 0 },
   { 1, 1, 1, 1, 0, 0 },
   { 1, 1, 1, 1, 0, 0 },
   { 1, 1, 1, 1, 1, 0 },
   { 1, 1, 1, 1, 1, 1 },
   { 1, 1, 1, 1, 1, 2 }
};

static int block_access_counts [BLOCK_COUNT];

static int test_disk_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{
  int rv = 0;

  if (req == RTEMS_BLKIO_REQUEST) {
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    rtems_blkdev_request *breq = arg;
    rtems_blkdev_sg_buffer *sg = breq->bufs;
    uint32_t i;

    for (i = 0; i < breq->bufnum; ++i) {
      rtems_blkdev_bnum block = sg [i].block;

      rtems_test_assert(block < BLOCK_COUNT);

      ++block_access_counts [block];

      if (block == 5) {
        sc = RTEMS_IO_ERROR;
      }
    }

    rtems_blkdev_request_done(breq, sc);
  } else {
    rv = rtems_blkdev_ioctl(dd, req, arg);
  }

  return rv;
}

static void test_actions(rtems_disk_device *dd)
{
  int i;

  for (i = 0; i < ACTION_COUNT; ++i) {
    const test_action *action = &actions [i];
    rtems_status_code sc;
    rtems_bdbuf_buffer *bd;
    rtems_blkdev_stats stats;

    printf("action %i\n", i);

    sc = (*action->get)(dd, action->block, &bd);
    rtems_test_assert(sc == action->expected_get_status);

    if (sc == RTEMS_SUCCESSFUL) {
      sc = (*action->release)(bd);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }

    rtems_test_assert(
      memcmp(
        block_access_counts,
        expected_block_access_counts [i],
        sizeof(block_access_counts)
      ) == 0
    );

    rtems_bdbuf_get_device_stats(dd, &stats);

    rtems_test_assert(
      memcmp(
        &stats,
        &expected_stats [i],
        sizeof(stats)
      ) == 0
    );
  }

  rtems_blkdev_print_stats(&dd->stats, 0, 1, 2, &rtems_test_printer);
}

static void test(void)
{
  rtems_status_code sc;
  rtems_disk_device *dd;
  int fd;
  int rv;

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

  test_actions(dd);

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
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS 1
#define CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY 1

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_PRIORITY 2
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
