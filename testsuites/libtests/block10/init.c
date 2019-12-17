/**
 * @file
 *
 * @ingroup test_bdbuf
 *
 * @brief Bdbuf test for purge.
 */

/*
 * Copyright (c) 2010, 2018 embedded brains GmbH.
 *
 *   embedded brains GmbH
 *   Dornierstr. 4
 *   D-82178 Puchheim
 *   Germany
 *   <rtems@embedded-brains.de>
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

const char rtems_test_name[] = "BLOCK 10";

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

#define PRIORITY_HIGH 1

#define PRIORITY_INIT 2

#define PRIORITY_MID 3

#define PRIORITY_SWAPOUT 4

#define PRIORITY_LOW 5

#define PRIORITY_IDLE 6

#define BLOCK_SIZE 1

#define BLOCK_COUNT 1

#define DISK_PATH "/disk"

typedef rtems_bdbuf_buffer *(*access_func)(char task);

typedef void (*release_func)(char task, rtems_bdbuf_buffer *bd);

static rtems_disk_device *dd;

static rtems_id task_id_init;

static rtems_id task_id_purger;

static rtems_id task_id_waiter;

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
  rtems_status_code sc = RTEMS_SUCCESSFUL;
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

static rtems_bdbuf_buffer *do_get(char task)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printf("%c: try get\n", task);

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printf("%c: get\n", task);

  return bd;
}

static rtems_bdbuf_buffer *do_get_mod(char task)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printf("%c: try get modified\n", task);

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_release_modified(bd);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_get(dd, 0, &bd);
  ASSERT_SC(sc);

  printf("%c: get modified\n", task);

  return bd;
}

static rtems_bdbuf_buffer *do_read(char task)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;

  printf("%c: try read\n", task);

  sc = rtems_bdbuf_read(dd, 0, &bd);
  ASSERT_SC(sc);

  printf("%c: read\n", task);

  return bd;
}

static void do_rel(char task, rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  printf("%c: release\n", task);

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  printf("%c: release done\n", task);
}

static void do_rel_mod(char task, rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  printf("%c: release modified\n", task);

  sc = rtems_bdbuf_release_modified(bd);
  ASSERT_SC(sc);

  printf("%c: release modified done\n", task);
}

static void do_sync(char task, rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  printf("%c: sync\n", task);

  sc = rtems_bdbuf_sync(bd);
  ASSERT_SC(sc);

  printf("%c: sync done\n", task);
}

static void purge(char task)
{
  printf("%c: purge\n", task);

  rtems_bdbuf_purge_dev(dd);
}

static void task_purger(rtems_task_argument arg)
{
  while (true) {
    suspend(RTEMS_SELF);

    set_task_prio(RTEMS_SELF, PRIORITY_HIGH);

    purge('P');
  }

  rtems_task_exit();
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

  rtems_task_exit();
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
  size_t i_w = 0;
  size_t i_ac = 0;
  size_t i_rel = 0;
  size_t i_p = 0;

  TEST_BEGIN();

  task_id_init = rtems_task_self();

  disk_register(BLOCK_SIZE, BLOCK_COUNT, &dd);

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
        printf("test case [access]: %s and %s %s\n", access_assoc_table [i_ac], release_assoc_table [i_rel], waiter_assoc_table [i_w]);
        check_access(access_table [i_ac], release_table [i_rel], waiter_table [i_w]);
      }
    }
  }

  for (i_rel = 0; i_rel < RELEASE_COUNT; ++i_rel) {
    for (i_w = 0; i_w < WAITER_COUNT; ++i_w) {
      printf("test case [intermediate]: %s %s\n", release_assoc_table [i_rel], waiter_assoc_table [i_w]);
      check_intermediate(release_table [i_rel], waiter_table [i_w]);
    }
  }

  for (i_p = 0; i_p < PURGER_COUNT; ++i_p) {
    for (i_w = 0; i_w < WAITER_COUNT; ++i_w) {
      printf("test case [transfer]: %s %s\n", purger_assoc_table [i_p], waiter_assoc_table [i_w]);
      check_transfer(purger_table [i_p], waiter_table [i_w]);
    }
  }

  TEST_END();

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
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
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE (BLOCK_SIZE * BLOCK_COUNT)

#include <rtems/confdefs.h>
