/**
 * @file
 *
 * @ingroup test_bdbuf
 *
 * @brief Bdbuf test.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>
#include <rtems/ramdisk.h>
#include <rtems/bdbuf.h>
#include <rtems/diskdevs.h>

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define PRIORITY_INIT 10

#define PRIORITY_SWAPOUT 50

#define PRIORITY_HIGH 30

#define PRIORITY_LOW 40

#define BLOCK_SIZE_A 512

#define BLOCK_COUNT_A 2

#define BLOCK_SIZE_B 1024

#define BLOCK_COUNT_B 1

static rtems_disk_device *dd_a;

static rtems_disk_device *dd_b;

static volatile bool sync_done = false;

static rtems_id task_id_low;

static rtems_id task_id_high;

static void task_low(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  rtems_test_assert(!sync_done);

  printk("L: try access: A0\n");

  sc = rtems_bdbuf_get(dd_a, 0, &bd);
  ASSERT_SC(sc);

  rtems_test_assert(sync_done);

  printk("L: access: A0\n");

  rtems_test_assert(bd->dd == dd_a);

  printk("*** END OF TEST BLOCK 2 ***\n");

  exit(0);
}

static void task_high(rtems_task_argument arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  rtems_test_assert(!sync_done);

  printk("H: try access: A0\n");

  sc = rtems_bdbuf_get(dd_a, 0, &bd);
  ASSERT_SC(sc);

  rtems_test_assert(sync_done);

  printk("H: access: A0\n");

  printk("H: release: A0\n");

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printk("H: release done: A0\n");

  printk("H: try access: B0\n");

  sc = rtems_bdbuf_get(dd_b, 0, &bd);
  ASSERT_SC(sc);

  printk("H: access: B0\n");

  /* If we reach this code, we have likely a bug */

  printk("H: release: B0\n");

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printk("H: release done: B0\n");

  rtems_task_delete(RTEMS_SELF);
}

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;
  dev_t dev_a = 0;
  dev_t dev_b = 0;

  printk("\n\n*** TEST BLOCK 2 ***\n");

  sc = rtems_disk_io_initialize();
  ASSERT_SC(sc);

  sc = ramdisk_register(BLOCK_SIZE_A, BLOCK_COUNT_A, false, "/dev/rda", &dev_a);
  ASSERT_SC(sc);

  sc = ramdisk_register(BLOCK_SIZE_B, BLOCK_COUNT_B, false, "/dev/rdb", &dev_b);
  ASSERT_SC(sc);

  dd_a = rtems_disk_obtain(dev_a);
  rtems_test_assert(dd_a != NULL);

  dd_b = rtems_disk_obtain(dev_b);
  rtems_test_assert(dd_b != NULL);

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

  sc = rtems_bdbuf_get(dd_a, 0, &bd);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_sync(bd);
  ASSERT_SC(sc);

  printk("I: sync done: A0\n");

  sync_done = true;

  sc = rtems_task_suspend(RTEMS_SELF);
  ASSERT_SC(sc);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_DRIVERS 3

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_INIT
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_SWAPOUT_TASK_PRIORITY PRIORITY_SWAPOUT

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE BLOCK_SIZE_A
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE BLOCK_SIZE_B
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE BLOCK_SIZE_B

#include <rtems/confdefs.h>
