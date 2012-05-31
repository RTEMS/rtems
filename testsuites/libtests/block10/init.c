/**
 * @file
 *
 * @ingroup test_bdbuf
 *
 * @brief Bdbuf test for purge.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/diskdevs.h>

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

#define PRIORITY_HIGH 1

#define PRIORITY_INIT 2

#define PRIORITY_MID 3

#define PRIORITY_SWAPOUT 4

#define PRIORITY_LOW 5

#define PRIORITY_IDLE 6

#define BLOCK_SIZE 1

#define BLOCK_COUNT 1

typedef rtems_bdbuf_buffer *(*access_func)(char task);

typedef void (*release_func)(char task, rtems_bdbuf_buffer *bd);

static rtems_disk_device *dd;

static rtems_id task_id_init;

static rtems_id task_id_purger;

static rtems_id task_id_waiter;

static const rtems_driver_address_table disk_ops = {
  .initialization_entry = NULL,
  RTEMS_GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES
};

static void set_task_prio(rtems_id task, rtems_task_priority prio)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_task_priority cur = 0;

  sc = rtems_task_set_priority(task, prio, &cur);
  ASSERT_SC(sc);
}

static void suspend(rtems_id task)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_task_suspend(task);
  ASSERT_SC(sc);
}

static void resume(rtems_id task)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_task_resume(task);
  ASSERT_SC(sc);
}

static int disk_ioctl(rtems_disk_device *dd, uint32_t req, void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (req == RTEMS_BLKIO_REQUEST) {
    rtems_blkdev_request *r = arg;

    if (r->req == RTEMS_BLKDEV_REQ_WRITE) {
      set_task_prio(RTEMS_SELF, PRIORITY_IDLE);
      set_task_prio(RTEMS_SELF, PRIORITY_SWAPOUT);
    }

    r->req_done(r->done_arg, sc);

    return 0;
  } else {
    return rtems_blkdev_ioctl(dd, req, arg);
  }
}

static rtems_status_code disk_register(
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  dev_t *dev_ptr
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_device_major_number major = 0;
  dev_t dev = 0;

  sc = rtems_io_register_driver(0, &disk_ops, &major);
  ASSERT_SC(sc);

  dev = rtems_filesystem_make_dev_t(major, 0);

  sc = rtems_disk_create_phys(
    dev,
    block_size,
    block_count,
    disk_ioctl,
    NULL,
    NULL
  );
  ASSERT_SC(sc);

  *dev_ptr = dev;

  return RTEMS_SUCCESSFUL;
}

static rtems_bdbuf_buffer *do_get(char task)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printk("%c: try get\n", task);

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printk("%c: get\n", task);

  return bd;
}

static rtems_bdbuf_buffer *do_get_mod(char task)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printk("%c: try get modified\n", task);

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_release_modified(bd);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printk("%c: get modified\n", task);

  return bd;
}

static rtems_bdbuf_buffer *do_read(char task)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printk("%c: try read\n", task);

  sc = rtems_bdbuf_read(dd, 0, &bd);
  ASSERT_SC(sc);

  printk("%c: read\n", task);

  return bd;
}

static void do_rel(char task, rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  printk("%c: release\n", task);

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printk("%c: release done\n", task);
}

static void do_rel_mod(char task, rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  printk("%c: release modified\n", task);

  sc = rtems_bdbuf_release_modified(bd);
  ASSERT_SC(sc);

  printk("%c: release modified done\n", task);
}

static void do_sync(char task, rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  printk("%c: sync\n", task);

  sc = rtems_bdbuf_sync(bd);
  ASSERT_SC(sc);

  printk("%c: sync done\n", task);
}

static void purge(char task)
{
  printk("%c: purge\n", task);

  rtems_bdbuf_purge_dev(dd);
}

static void task_purger(rtems_task_argument arg)
{
  while (true) {
    suspend(RTEMS_SELF);

    set_task_prio(RTEMS_SELF, PRIORITY_HIGH);

    purge('P');
  }

  rtems_task_delete(RTEMS_SELF);
}

static void activate_purger(rtems_task_priority prio)
{
  set_task_prio(task_id_purger, prio);
  resume(task_id_purger);
}

static void task_waiter(rtems_task_argument arg)
{
  while (true) {
    rtems_bdbuf_buffer *bd = NULL;

    suspend(RTEMS_SELF);

    bd = do_get('W');

    do_rel('W', bd);
  }

  rtems_task_delete(RTEMS_SELF);
}

static void create_waiter(void)
{
  resume(task_id_waiter);
  set_task_prio(task_id_waiter, PRIORITY_IDLE);
}

static void restore_waiter(void)
{
  set_task_prio(task_id_waiter, PRIORITY_HIGH);
}

static void check_access(access_func ac, release_func rel, bool waiter)
{
  rtems_bdbuf_buffer *bd = (*ac)('I');

  if (waiter) {
    create_waiter();
  }

  purge('I');

  (*rel)('I', bd);

  if (waiter) {
    restore_waiter();
  }
}

static void check_intermediate(release_func rel, bool waiter)
{
  rtems_bdbuf_buffer *bd = do_read('I');

  if (waiter) {
    create_waiter();
  }

  (*rel)('I', bd);

  purge('I');

  if (waiter) {
    restore_waiter();
  }
}

static void check_transfer(rtems_task_priority prio, bool waiter)
{
  rtems_bdbuf_buffer *bd = do_read('I');

  if (waiter) {
    create_waiter();
  }

  activate_purger(prio);

  do_sync('I', bd);

  if (waiter) {
    restore_waiter();
  }
}

static const bool waiter_table [] = {
  true,
  false
};

static const access_func access_table [] = {
  do_get,
  do_get_mod,
  do_read
};

static const release_func release_table [] = {
  do_rel,
  do_rel_mod,
  do_sync
};

static const rtems_task_priority purger_table [] = {
  PRIORITY_MID,
  PRIORITY_LOW
};

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array [0]))

#define WAITER_COUNT ARRAY_COUNT(waiter_table)

#define ACCESS_COUNT ARRAY_COUNT(access_table)

#define RELEASE_COUNT ARRAY_COUNT(release_table)

#define PURGER_COUNT ARRAY_COUNT(purger_table)

static const char *waiter_assoc_table [WAITER_COUNT] = {
  "with waiter",
  "without waiter"
};

static const char *access_assoc_table [ACCESS_COUNT] = {
  "get",
  "access modified",
  "read"
};

static const char *release_assoc_table [RELEASE_COUNT] = {
  "release",
  "release modified",
  "sync"
};

static const char *purger_assoc_table [PURGER_COUNT] = {
  "sync",
  "transfer"
};

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  dev_t dev = 0;
  size_t i_w = 0;
  size_t i_ac = 0;
  size_t i_rel = 0;
  size_t i_p = 0;

  printk("\n\n*** TEST BLOCK 10 ***\n");

  task_id_init = rtems_task_self();

  sc = rtems_disk_io_initialize();
  ASSERT_SC(sc);

  sc = disk_register(BLOCK_SIZE, BLOCK_COUNT, &dev);
  ASSERT_SC(sc);

  dd = rtems_disk_obtain(dev);
  assert(dd != NULL);

  sc = rtems_task_create(
    rtems_build_name('P', 'U', 'R', 'G'),
    PRIORITY_HIGH,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id_purger
  );
  ASSERT_SC(sc);

  sc = rtems_task_start(task_id_purger, task_purger, 0);
  ASSERT_SC(sc);

  set_task_prio(task_id_purger, PRIORITY_LOW);

  sc = rtems_task_create(
    rtems_build_name('W', 'A', 'I', 'T'),
    PRIORITY_HIGH,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id_waiter
  );
  ASSERT_SC(sc);

  sc = rtems_task_start(task_id_waiter, task_waiter, 0);
  ASSERT_SC(sc);

  for (i_ac = 0; i_ac < ACCESS_COUNT; ++i_ac) {
    for (i_rel = 0; i_rel < RELEASE_COUNT; ++i_rel) {
      for (i_w = 0; i_w < WAITER_COUNT; ++i_w) {
        printk("test case [access]: %s and %s %s\n", access_assoc_table [i_ac], release_assoc_table [i_rel], waiter_assoc_table [i_w]);
        check_access(access_table [i_ac], release_table [i_rel], waiter_table [i_w]);
      }
    }
  }

  for (i_rel = 0; i_rel < RELEASE_COUNT; ++i_rel) {
    for (i_w = 0; i_w < WAITER_COUNT; ++i_w) {
      printk("test case [intermediate]: %s %s\n", release_assoc_table [i_rel], waiter_assoc_table [i_w]);
      check_intermediate(release_table [i_rel], waiter_table [i_w]);
    }
  }

  for (i_p = 0; i_p < PURGER_COUNT; ++i_p) {
    for (i_w = 0; i_w < WAITER_COUNT; ++i_w) {
      printk("test case [transfer]: %s %s\n", purger_assoc_table [i_p], waiter_assoc_table [i_w]);
      check_transfer(purger_table [i_p], waiter_table [i_w]);
    }
  }

  printk("*** END OF TEST BLOCK 10 ***\n");

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_DRIVERS 4

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_INIT
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_SWAPOUT_TASK_PRIORITY PRIORITY_SWAPOUT

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE BLOCK_SIZE
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE BLOCK_SIZE
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (BLOCK_SIZE * BLOCK_COUNT)

#include <rtems/confdefs.h>
