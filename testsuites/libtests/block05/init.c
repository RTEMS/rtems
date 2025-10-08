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

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/bdbuf.h>
#include <rtems/blkdev.h>

#include <tmacros.h>

const char rtems_test_name[] = "BLOCK 5";

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define ASSERT_SC(sc) rtems_test_assert((sc) == RTEMS_SUCCESSFUL)

#define PRIORITY_INIT 1

#define PRIORITY_HIGH 2

#define PRIORITY_LOW_ALT 3

#define PRIORITY_SWAPOUT 4

#define PRIORITY_LOW 5

#define PRIORITY_RESUME 6

#define BLOCK_SIZE_A 1

#define BLOCK_COUNT_A 2

#define BLOCK_SIZE_B 2

#define BLOCK_COUNT_B 1

/* In case of trouble change this to 1 or 2 for more output */
static unsigned output_level = 0;

static rtems_disk_device *dd_a;

static rtems_disk_device *dd_b;

static rtems_id task_id_init;

static rtems_id task_id_low;

static rtems_id task_id_high;

static rtems_id task_id_resume;

static volatile bool finish_low;

static volatile bool finish_high;

static volatile enum resume_style {
  RESUME_IMMEDIATE,
  RESUME_FINISH
} resume;

static volatile enum trig_style {
  SUSP = 0,
  CONT
} trig;

static volatile enum get_type {
  GET = 0,
  READ
} trig_get, low_get, high_get;

static volatile enum blk_kind {
  BLK_A0 = 0,
  BLK_A1,
  BLK_B0
} trig_blk, low_blk, high_blk;

static volatile enum rel_type {
  REL = 0,
  REL_MOD,
  SYNC
} trig_rel, low_rel, high_rel;

static const char trig_style_desc [] = {
  'S',
  'C'
};

static const char get_type_desc [] = {
  'G',
  'R'
};

static const char *blk_kind_desc [] = {
  "A0",
  "A1",
  "B0"
};

static const char rel_type_desc [] = {
  'R',
  'M',
  'S'
};

static void print(unsigned level, const char *fmt, ...)
{
  if (level <= output_level) {
    va_list ap;

    va_start(ap, fmt);
    vprintk(fmt, ap);
    va_end(ap);
  }
}

static void set_task_prio(rtems_id task, rtems_task_priority prio)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_task_priority cur = 0;

  sc = rtems_task_set_priority(task, prio, &cur);
  ASSERT_SC(sc);
}

static rtems_bdbuf_buffer *get(enum get_type type, enum blk_kind kind)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;
  rtems_blkdev_bnum blk_index = 0;
  rtems_status_code (*get_bd)(
    rtems_disk_device *,
    rtems_blkdev_bnum,
    rtems_bdbuf_buffer **
  ) = NULL;
  rtems_disk_device *dd = NULL;
  size_t bds_per_group = 0;

  switch (kind) {
    case BLK_A0:
      dd = dd_a;
      blk_index = 0;
      bds_per_group = 2;
      break;
    case BLK_A1:
      dd = dd_a;
      blk_index = 1;
      bds_per_group = 2;
      break;
    case BLK_B0:
      dd = dd_b;
      blk_index = 0;
      bds_per_group = 1;
      break;
    default:
      rtems_test_assert(false);
      break;
  }

  switch (type) {
    case GET:
      get_bd = rtems_bdbuf_get;
      break;
    case READ:
      get_bd = rtems_bdbuf_read;
      break;
    default:
      rtems_test_assert(false);
      break;
  }

  sc = (*get_bd)(dd, blk_index, &bd);
  rtems_test_assert(
    sc == RTEMS_SUCCESSFUL
      && bd->dd == dd
      && bd->block == blk_index
      && bd->group->bds_per_group == bds_per_group
   );

  return bd;
}

static void rel(rtems_bdbuf_buffer *bd, enum rel_type type)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code (*rel_bd)(rtems_bdbuf_buffer *) = NULL;

  switch (type) {
    case REL:
      rel_bd = rtems_bdbuf_release;
      break;
    case REL_MOD:
      rel_bd = rtems_bdbuf_release_modified;
      break;
    case SYNC:
      rel_bd = rtems_bdbuf_sync;
      break;
    default:
      rtems_test_assert(false);
      break;
  }

  sc = (*rel_bd)(bd);
  ASSERT_SC(sc);
}

static void task_low(rtems_task_argument arg)
{
  (void) arg;

  rtems_status_code sc = RTEMS_SUCCESSFUL;

  while (true) {
    print(2, "LG\n");
    rtems_bdbuf_buffer *bd = get(low_get, low_blk);

    print(2, "LR\n");
    rel(bd, low_rel);

    finish_low = true;

    sc = rtems_task_suspend(RTEMS_SELF);
    ASSERT_SC(sc);
  }
}

static void task_high(rtems_task_argument arg)
{
  (void) arg;

  rtems_status_code sc = RTEMS_SUCCESSFUL;

  while (true) {
    print(2, "HG\n");
    rtems_bdbuf_buffer *bd = get(high_get, high_blk);

    print(2, "HR\n");
    rel(bd, high_rel);

    finish_high = true;

    sc = rtems_task_suspend(RTEMS_SELF);
    ASSERT_SC(sc);
  }
}

static void task_resume(rtems_task_argument arg)
{
  (void) arg;

  while (true) {
    bool do_resume = false;

    switch (resume) {
      case RESUME_IMMEDIATE:
        print(2, "RI\n");
        do_resume = true;
        break;
      case RESUME_FINISH:
        print(2, "RF\n");
        do_resume = finish_low && finish_high;
        break;
      default:
        rtems_test_assert(false);
        break;
    }

    if (do_resume) {
      rtems_task_resume(task_id_init);
    }
  }
}

static void execute_test(unsigned i)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;
  bool suspend = false;

  print(
    1,
    "[%05u]T(%c,%c,%s,%c)L(%c,%s,%c)H(%c,%s,%c)\n",
    i,
    trig_style_desc [trig],
    get_type_desc [trig_get],
    blk_kind_desc [trig_blk],
    rel_type_desc [trig_rel],
    get_type_desc [low_get],
    blk_kind_desc [low_blk],
    rel_type_desc [low_rel],
    get_type_desc [high_get],
    blk_kind_desc [high_blk],
    rel_type_desc [high_rel]
  );

  set_task_prio(task_id_low, PRIORITY_LOW);

  finish_low = false;
  finish_high = false;

  sc = rtems_task_resume(task_id_low);
  ASSERT_SC(sc);

  sc = rtems_task_resume(task_id_high);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_get(dd_b, 0, &bd);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL && bd->dd == dd_b && bd->block == 0);

  sc = rtems_bdbuf_release(bd);
  ASSERT_SC(sc);

  switch (trig) {
    case SUSP:
      suspend = true;
      break;
    case CONT:
      suspend = false;
      break;
    default:
      rtems_test_assert(false);
      break;
  }

  print(2, "TG\n");
  bd = get(trig_get, trig_blk);

  if (suspend) {
    print(2, "S\n");
    resume = RESUME_IMMEDIATE;
    sc = rtems_task_suspend(RTEMS_SELF);
    ASSERT_SC(sc);
  }
  resume = RESUME_FINISH;

  print(2, "TR\n");
  rel(bd, trig_rel);

  sc = rtems_task_suspend(RTEMS_SELF);
  ASSERT_SC(sc);

  print(2, "F\n");

  sc = rtems_bdbuf_syncdev(dd_a);
  ASSERT_SC(sc);

  sc = rtems_bdbuf_syncdev(dd_b);
  ASSERT_SC(sc);
}

static int disk_ioctl(rtems_disk_device *dd, uint32_t req, void *argp)
{
  if (req == RTEMS_BLKIO_REQUEST) {
    rtems_blkdev_request *r = argp;

    set_task_prio(task_id_low, PRIORITY_LOW_ALT);

    switch (r->req) {
      case RTEMS_BLKDEV_REQ_READ:
      case RTEMS_BLKDEV_REQ_WRITE:
        rtems_blkdev_request_done(r, RTEMS_SUCCESSFUL);
        return 0;
      default:
        errno = EINVAL;
        return -1;
    }
  } else {
    return rtems_blkdev_ioctl(dd, req, argp);
  }
}

static void disk_register(
  const char *dev,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_disk_device **dd_ptr
)
{
  rtems_status_code sc;
  int rv;
  int fd;

  sc = rtems_blkdev_create(
    dev,
    block_size,
    block_count,
    disk_ioctl,
    NULL
  );
  ASSERT_SC(sc);

  fd = open(dev, O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = rtems_disk_fd_get_disk_device(fd, dd_ptr);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static rtems_task Init(rtems_task_argument argument)
{
  (void) argument;

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned i = 0;

  TEST_BEGIN();

  task_id_init = rtems_task_self();

  disk_register("dd_b", BLOCK_SIZE_A, BLOCK_COUNT_A, &dd_a);

  disk_register("dd_a", BLOCK_SIZE_B, BLOCK_COUNT_B, &dd_b);

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

  sc = rtems_task_create(
    rtems_build_name('R', 'E', 'S', 'U'),
    PRIORITY_RESUME,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id_resume
  );
  ASSERT_SC(sc);

  sc = rtems_task_start(task_id_resume, task_resume, 0);
  ASSERT_SC(sc);

  sc = rtems_task_suspend(task_id_low);
  ASSERT_SC(sc);

  sc = rtems_task_suspend(task_id_high);
  ASSERT_SC(sc);

  for (trig = SUSP; trig <= CONT; ++trig) {
    for (trig_get = GET; trig_get <= READ; ++trig_get) {
      for (low_get = GET; low_get <= READ; ++low_get) {
        for (high_get = GET; high_get <= READ; ++high_get) {
          for (trig_blk = BLK_A0; trig_blk <= BLK_B0; ++trig_blk) {
            for (low_blk = BLK_A0; low_blk <= BLK_B0; ++low_blk) {
              for (high_blk = BLK_A0; high_blk <= BLK_B0; ++high_blk) {
                for (trig_rel = REL; trig_rel <= SYNC; ++trig_rel) {
                  for (low_rel = REL; low_rel <= SYNC; ++low_rel) {
                    for (high_rel = REL; high_rel <= SYNC; ++high_rel) {
                      execute_test(i);
                      ++i;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  TEST_END();

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_INIT
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_SWAPOUT_TASK_PRIORITY PRIORITY_SWAPOUT

#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE BLOCK_SIZE_A
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE BLOCK_SIZE_B
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE BLOCK_SIZE_B

#include <rtems/confdefs.h>
