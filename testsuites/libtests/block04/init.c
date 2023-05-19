/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup test_bdbuf
 *
 * @brief Bdbuf test.
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

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/ramdisk.h>
#include <rtems/bdbuf.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const char rtems_test_name[] = "BLOCK 4";

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define PRIORITY_INIT 10

#define PRIORITY_SWAPOUT 50

#define PRIORITY_HIGH 30

#define PRIORITY_LOW 40

#define BLOCK_SIZE 512

#define BLOCK_COUNT 1

static rtems_disk_device *dd;

static rtems_id task_id_low;

static rtems_id task_id_high;

static void task_low(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printk("L: try access: 0\n");

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printk("L: access: 0\n");

  sc = rtems_task_resume(task_id_high);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_sync(bd);
  ASSERT_SC(sc);

  printk("L: sync done: 0\n");

  rtems_test_assert(false);
}

static void task_high(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  sc = rtems_task_suspend(RTEMS_SELF);
  ASSERT_SC(sc);

  printk("H: try access: 0\n");

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printk("H: access: 0\n");

  printk("H: release: 0\n");

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printk("H: release done: 0\n");

  TEST_END();

  exit(0);
}

static void do_ramdisk_register(
  uint32_t media_block_size,
  rtems_blkdev_bnum media_block_count,
  const char *disk,
  rtems_disk_device **dd
)
{
  rtems_status_code sc;
  int fd;
  int rv;

  sc = ramdisk_register(media_block_size, media_block_count, false, disk);
  ASSERT_SC(sc);

  fd = open(disk, O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = rtems_disk_fd_get_disk_device(fd, dd);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  TEST_BEGIN();

  do_ramdisk_register(BLOCK_SIZE, BLOCK_COUNT, "/dev/rda", &dd);

  sc = rtems_task_create(
    rtems_build_name(' ', 'L', 'O', 'W'),
    PRIORITY_LOW,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id_low
  );
  ASSERT_SC(sc);

  sc = rtems_task_start(task_id_low, task_low, 0);
  ASSERT_SC(sc);

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', 'H'),
    PRIORITY_HIGH,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id_high
  );
  ASSERT_SC(sc);

  sc = rtems_task_start(task_id_high, task_high, 0);
  ASSERT_SC(sc);

  sc = rtems_task_suspend(RTEMS_SELF);
  ASSERT_SC(sc);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_INIT
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_SWAPOUT_TASK_PRIORITY PRIORITY_SWAPOUT

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE BLOCK_SIZE
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE BLOCK_SIZE
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE BLOCK_SIZE

#include <rtems/confdefs.h>
