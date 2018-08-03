/**
 * @file
 *
 * @ingroup test_bdbuf
 *
 * @brief Bdbuf test for disk driver error handling..
 */

/*
 * Copyright (c) 2009, 2018 embedded brains GmbH.  All rights reserved.
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
#include <stdlib.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/test.h>

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

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE BLOCK_SIZE
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE BLOCK_SIZE
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (BLOCK_SIZE * BLOCK_COUNT)

#include <rtems/confdefs.h>
