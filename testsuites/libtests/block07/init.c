/**
 * @file
 *
 * @ingroup test_bdbuf
 *
 * @brief Bdbuf test for block size change event.
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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "tmacros.h"

#include <rtems.h>
#include <rtems/ramdisk.h>
#include <rtems/bdbuf.h>

const char rtems_test_name[] = "BLOCK 7";

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define PRIORITY_INIT 1

#define PRIORITY_HIGH 2

#define PRIORITY_MID 3

#define PRIORITY_LOW 4

#define PRIORITY_SWAPOUT 5

#define PRIORITY_IDLE 6

#define BLOCK_SIZE_A 1

#define BLOCK_SIZE_B 2

#define BLOCK_COUNT 2

static rtems_disk_device *dd;

static rtems_id task_id_low;

static rtems_id task_id_mid;

static rtems_id task_id_high;

static void change_block_size(void)
{
  int rv = 0;
  int fd = open("/dev/rda", O_RDWR);

  rtems_test_assert(fd >= 0);

  rv = rtems_disk_fd_set_block_size(fd, BLOCK_SIZE_B);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void task_low(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printf("L: try access: 0\n");

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printf("L: access: 0\n");

  rtems_test_assert(bd->group->bds_per_group == 2);

  printf("L: release: 0\n");

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printf("L: release done: 0\n");

  TEST_END();

  exit(0);
}

static void task_mid(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printf("M: try access: 0\n");

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printf("M: access: 0\n");

  rtems_test_assert(bd->group->bds_per_group == 1);

  printf("M: release: 0\n");

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printf("M: release done: 0\n");

  rtems_task_exit();
}

static void task_high(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  change_block_size();

  printf("H: try access: 0\n");

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printf("H: access: 0\n");

  rtems_test_assert(bd->group->bds_per_group == 1);

  printf("H: release: 0\n");

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printf("H: release done: 0\n");

  rtems_task_exit();
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
  rtems_task_priority cur_prio = 0;
  rtems_bdbuf_buffer *bd = NULL;

  TEST_BEGIN();

  do_ramdisk_register(BLOCK_SIZE_A, BLOCK_COUNT, "/dev/rda", &dd);

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
    rtems_build_name(' ', 'M', 'I', 'D'),
    PRIORITY_MID,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id_mid
  );
  ASSERT_SC(sc);

  sc = rtems_task_start(task_id_mid, task_mid, 0);
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

  sc = rtems_task_suspend(task_id_mid);
  ASSERT_SC(sc);

  sc = rtems_task_suspend(task_id_high);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_get(dd, 1, &bd);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printf("I: try access: 0\n");

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printf("I: access: 0\n");

  sc = rtems_task_set_priority(RTEMS_SELF, PRIORITY_IDLE, &cur_prio);
  ASSERT_SC(sc);

  sc = rtems_task_resume(task_id_high);
  ASSERT_SC(sc);

  sc = rtems_task_resume(task_id_mid);
  ASSERT_SC(sc);

  sc = rtems_task_set_priority(RTEMS_SELF, PRIORITY_INIT, &cur_prio);
  ASSERT_SC(sc);

  printf("I: release: 0\n");

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printf("I: release done: 0\n");

  rtems_task_exit();
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_INIT
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_SWAPOUT_TASK_PRIORITY PRIORITY_SWAPOUT

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE BLOCK_SIZE_A
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE BLOCK_SIZE_B
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (BLOCK_SIZE_A * BLOCK_COUNT)

#include <rtems/confdefs.h>
