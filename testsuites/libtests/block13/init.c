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

#include <errno.h>
#include <string.h>

#include <rtems/blkdev.h>
#include <rtems/bdbuf.h>

#define BLOCK_COUNT 11
#define READ_COUNT 23

static int block_access_counts [BLOCK_COUNT];

#define RESET_CACHE (-1)

static const int action_sequence [READ_COUNT] = {
  0, 2, 3, 4, 5, 6, 7, 8, 9, 10,
  RESET_CACHE,
  10,
  RESET_CACHE,
  9,
  RESET_CACHE,
  8,
  RESET_CACHE,
  7, 8,
  RESET_CACHE,
  6, 7, 9
};

#define UNUSED_LINE { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

static const int expected_block_access_counts [READ_COUNT] [BLOCK_COUNT] = {
   { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
   { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
   { 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
   { 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
   { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
   UNUSED_LINE,
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
   UNUSED_LINE,
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
   UNUSED_LINE,
   { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
   UNUSED_LINE,
   { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 },
   UNUSED_LINE,
   { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 },
   { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 }
};

#define NO_TRIGGER RTEMS_DISK_READ_AHEAD_NO_TRIGGER

#define TRIGGER_AFTER_RESET RTEMS_DISK_READ_AHEAD_NO_TRIGGER

static const rtems_blkdev_bnum trigger [READ_COUNT] = {
  1, 3, 5, 5, 8, 8, 8, NO_TRIGGER, NO_TRIGGER, NO_TRIGGER,
  TRIGGER_AFTER_RESET,
  11,
  TRIGGER_AFTER_RESET,
  10,
  TRIGGER_AFTER_RESET,
  9,
  TRIGGER_AFTER_RESET,
  8, NO_TRIGGER,
  TRIGGER_AFTER_RESET,
  7, 9, NO_TRIGGER
};

#define NOT_CHANGED_BY_RESET(i) (i)

static const rtems_blkdev_bnum next [READ_COUNT] = {
  2, 4, 7, 7, 10, 10, 10, 10, 10, 10,
  NOT_CHANGED_BY_RESET(10),
  12,
  NOT_CHANGED_BY_RESET(12),
  11,
  NOT_CHANGED_BY_RESET(11),
  10,
  NOT_CHANGED_BY_RESET(10),
  9, 9,
  NOT_CHANGED_BY_RESET(9),
  8, 11, 11
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

    (*breq->req_done)(breq->done_arg, RTEMS_SUCCESSFUL);
  } else {
    errno = EINVAL;
    rv = -1;
  }

  return rv;
}

static void test_read_ahead(rtems_disk_device *dd)
{
  int i;

  for (i = 0; i < READ_COUNT; ++i) {
    int action = action_sequence [i];

    if (action != RESET_CACHE) {
      rtems_blkdev_bnum block = (rtems_blkdev_bnum) action;
      rtems_status_code sc;
      rtems_bdbuf_buffer *bd;

      printf("%i ", action);

      sc = rtems_bdbuf_read(dd, block, &bd);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      sc = rtems_bdbuf_release(bd);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      rtems_test_assert(
        memcmp(
          block_access_counts,
          expected_block_access_counts [i],
          sizeof(block_access_counts)
        ) == 0
      );
    } else {
      printf("\nreset\n");

      rtems_bdbuf_purge_dev(dd);
      memset(&block_access_counts, 0, sizeof(block_access_counts));
    }

    rtems_test_assert(trigger [i] == dd->read_ahead.trigger);
    rtems_test_assert(next [i] == dd->read_ahead.next);
  }

  printf("\n");
}

static void test(void)
{
  rtems_status_code sc;
  dev_t dev = 0;
  rtems_disk_device *dd;

  sc = rtems_disk_io_initialize();
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_disk_create_phys(
    dev,
    1,
    BLOCK_COUNT,
    test_disk_ioctl,
    NULL,
    NULL
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  dd = rtems_disk_obtain(dev);
  rtems_test_assert(dd != NULL);

  test_read_ahead(dd);

  sc = rtems_disk_release(dd);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_disk_delete(dev);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST BLOCK 13 ***");

  test();

  puts("*** END OF TEST BLOCK 13 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE 1
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE 1
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE BLOCK_COUNT
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS 3
#define CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY 1

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_PRIORITY 2

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
