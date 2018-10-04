/*
 * Copyright 2008 Chris Johns (chrisj@rtems.org)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/**
 * BD Buffer test.
 *
 * Please add more tests
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/error.h>
#include <rtems/bdbuf.h>

#include <bsp.h>

#include "tmacros.h"

const char rtems_test_name[] = "BLOCK 6";

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define BDBUF_DISKS 2
#define BDBUF_SIZE  1024

#if 0
const rtems_bdbuf_config rtems_bdbuf_configuration =
{
  5,           /* max_read_ahead_blocks */
  5,           /* max_write_blocks */
  15,          /* swapout_priority */
  250,         /* swapout_period */
  1000,        /* swap_block_hold */
  0,           /* swapout_workers */
  15,          /* swapout_worker_priority */
  1024 * 1024, /* size */
  512,         /* buffer_min */
  4096         /* buffer_max */
};
#endif

/**
 * The bdbuf disk driver base name.
 */
#define BDBUF_DISK_DEVICE_BASE_NAME "/dev/bddisk"

/**
 * The actions the disk driver handles.
 */
typedef enum bdbuf_disk_action
{
  BDBUF_DISK_NOOP,
  BDBUF_DISK_WAIT,
  BDBUF_DISK_SLEEP,
  BDBUF_DISK_BLOCKS_INORDER
} bdbuf_disk_action;

/**
 * The BDBUF Disk driver.
 */
typedef struct bdbuf_disk
{
  const char*        name;
  uint32_t           minor;
  rtems_id           lock;
  uint32_t           block_size;
  uint32_t           block_count;
  bdbuf_disk_action  driver_action;
  const char*        watcher_name;
  rtems_id           watcher;
  int                watch_count;
  const char*        waiting_name;
  rtems_id           waiting;
  uint32_t           driver_sleep;
  rtems_disk_device* dd;
} bdbuf_disk;

/*
 * A disk drive for each pool.
 */
static bdbuf_disk bdbuf_disks[BDBUF_DISKS];

/**
 * Task control.
 */
typedef struct bdbuf_task_control
{
  bool                      die;
  const char*               name;
  rtems_id                  task;
  rtems_id                  master;
  int                       test;
  bdbuf_disk               *bdd;
  bool                      passed;
} bdbuf_task_control;

#define BDBUF_TEST_TASKS (3)

#define BDBUF_TEST_STACK_SIZE (2 * RTEMS_MINIMUM_STACK_SIZE)

/**
 * Seconds as milli-seconds.
 */
#define BDBUF_SECONDS(msec) ((msec) * 1000UL)

/**
 * Print the status code description and return true if true.
 *
 * @param sc The RTEMS status code.
 * @retval true The status code is successful.
 * @retval false The status code is not successful.
 */
static bool
bdbuf_test_print_sc (rtems_status_code sc, bool newline)
{
  if (newline)
    printf ("%s\n", rtems_status_text (sc));
  else
    printf ("%s", rtems_status_text (sc));
  return sc == RTEMS_SUCCESSFUL;
}

/**
 * BDBuf disk device driver lock.
 */
static bool
bdbuf_disk_lock (bdbuf_disk* bdd)
{
  rtems_status_code sc;
  sc = rtems_semaphore_obtain (bdd->lock, RTEMS_WAIT, 0);
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("disk ioctl: lock failed: ");
    bdbuf_test_print_sc (sc, true);
    return false;
  }
  return true;
}

/**
 * BDBuf disk device driver unlock.
 */
static bool
bdbuf_disk_unlock (bdbuf_disk* bdd)
{
  rtems_status_code sc;
  sc = rtems_semaphore_release (bdd->lock);
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("disk ioctl: unlock failed: ");
    bdbuf_test_print_sc (sc, true);
    return false;
  }
  return true;
}

/**
 * BDBUf wait for the wait event.
 */
static rtems_status_code
bdbuf_wait (const char* who, unsigned long timeout)
{
  rtems_status_code sc;
  rtems_event_set   out;
  sc = rtems_event_receive (RTEMS_EVENT_0,
                            RTEMS_WAIT | RTEMS_EVENT_ANY,
                            RTEMS_MICROSECONDS_TO_TICKS (timeout * 1000),
                            &out);
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("%s: wait: receive failed: ", who);
    bdbuf_test_print_sc (sc, true);
  }
  else if ((out & RTEMS_EVENT_0) == 0)
  {
    printf ("%s: wait: received wrong event: %08" PRIxrtems_event_set, who, out);
  }
  return sc;
}

/**
 * BDBUf send wait event.
 */
static bool
bdbuf_send_wait_event (const char* task, const char* msg, rtems_id id)
{
  printf ("%s: %s: %08" PRIxrtems_id ": ", task, msg, id);
  return  bdbuf_test_print_sc (rtems_event_send (id, RTEMS_EVENT_0), true);
}

/**
 * BDBUf wait for the wait event.
 */
static rtems_status_code
bdbuf_watch (unsigned long timeout)
{
  rtems_status_code sc;
  rtems_event_set   out;
  sc = rtems_event_receive (RTEMS_EVENT_1,
                            RTEMS_WAIT | RTEMS_EVENT_ANY,
                            RTEMS_MICROSECONDS_TO_TICKS (timeout * 1000),
                            &out);
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("watch: receive failed: ");
    bdbuf_test_print_sc (sc, true);
  }
  else if ((out & RTEMS_EVENT_1) == 0)
  {
    printf ("watch: received wrong event: %08" PRIxrtems_event_set, out);
  }
  return sc;
}

/**
 * BDBUf send wait event.
 */
static bool
bdbuf_send_watch_event (const char* task, const char* msg, rtems_id id)
{
  printf ("%s: %s: %08" PRIxrtems_id ": ", task, msg, id);
  return  bdbuf_test_print_sc (rtems_event_send (id, RTEMS_EVENT_1), true);
}

/**
 * Set up a disk driver watch.
 */
static void
bdbuf_set_disk_driver_watch (bdbuf_task_control* tc, int count)
{
  /*
   * Set up a disk watch and wait for the write to happen.
   */
  bdbuf_disk_lock (tc->bdd);
  tc->bdd->watcher_name = tc->name;
  tc->bdd->watcher = tc->task;
  tc->bdd->watch_count = count;
  bdbuf_disk_unlock (tc->bdd);
}

/**
 * Clear the disk driver watch.
 */
static void
bdbuf_clear_disk_driver_watch (bdbuf_task_control* tc)
{
  /*
   * Set up a disk watch and wait for the write to happen.
   */
  bdbuf_disk_lock (tc->bdd);
  tc->bdd->watcher_name = 0;
  tc->bdd->watcher = 0;
  tc->bdd->watch_count = 0;
  bdbuf_disk_unlock (tc->bdd);
}

/**
 * Wait for the disk driver watch.
 */
static bool
bdbuf_disk_driver_watch_wait (bdbuf_task_control* tc, unsigned long msecs)
{
  bool              passed = true;
  rtems_status_code sc = bdbuf_watch (msecs);
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("%s: driver watch: driver wait: ", tc->name);
    passed = bdbuf_test_print_sc (sc, true);
  }
  bdbuf_clear_disk_driver_watch (tc);
  return passed;
}

/**
 * Set the disk driver action.
 */
static void
bdbuf_set_disk_driver_action (bdbuf_task_control* tc, bdbuf_disk_action action)
{
  /*
   * Set up a disk action.
   */
  bdbuf_disk_lock (tc->bdd);
  tc->bdd->driver_action = action;
  bdbuf_disk_unlock (tc->bdd);
}

/**
 * BDBUF Sleep.
 */
static bool
bdbuf_sleep (unsigned long msecs)
{
  rtems_status_code sc;
  sc = rtems_task_wake_after (RTEMS_MICROSECONDS_TO_TICKS (msecs * 1000));
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("sleep wake after failed: ");
    bdbuf_test_print_sc (sc, true);
    return false;
  }
  return true;
}

/**
 * Initialise a task control.
 */
static void
bdbuf_task_control_init (int                       task,
                         bdbuf_task_control*       tc,
                         rtems_id                  master)
{
  char name[6];
  sprintf (name, "bdt%d", task);

  tc->die    = false;
  tc->name   = strdup (name); /* leaks */
  tc->task   = 0;
  tc->master = master;
  tc->test   = 0;
  tc->bdd    = NULL;
  tc->passed = false;
}

static bool
bdbuf_disk_ioctl_watcher (bdbuf_disk* bdd, int update)
{
  /*
   * Always wake the watcher.
   */
  if (bdd->watcher)
  {
    if (bdd->watch_count)
    {
      if (update > bdd->watch_count)
        bdd->watch_count -= update;
      else
        bdd->watch_count = 0;
    }

    if (bdd->watch_count == 0)
    {
      bdbuf_send_watch_event (bdd->watcher_name,
                              "disk ioctl: wake watcher",
                              bdd->watcher);
      bdd->watcher = 0;
    }
  }

  return true;
}


static bool
bdbuf_disk_ioctl_process (bdbuf_disk* bdd, rtems_blkdev_request* req)
{
  bool result = true;
  int  b;

  /*
   * Perform the requested action.
   */
  switch (bdd->driver_action)
  {
    case BDBUF_DISK_NOOP:
      break;

    case BDBUF_DISK_WAIT:
      if (bdd->waiting)
        printf ("disk ioctl: bad waiter: %s:%08" PRIxrtems_id "\n",
                           bdd->waiting_name, bdd->waiting);
      bdd->waiting_name = "bdd";

      bdd->waiting = rtems_task_self ();

      if (bdbuf_disk_unlock (bdd))
        result = bdbuf_wait (bdd->name, 0) == RTEMS_SUCCESSFUL;
      else
        result = false;

      /*
       * Ignore any error if one happens.
       */
      bdbuf_disk_lock (bdd);
      break;

    case BDBUF_DISK_SLEEP:
      printf ("disk ioctl: sleeping: %" PRId32 " msecs\n",
                         bdd->driver_sleep);
      result = bdbuf_sleep (bdd->driver_sleep);
      break;

    case BDBUF_DISK_BLOCKS_INORDER:
      printf ("disk ioctl: multi-block order check: count = %" PRId32 "\n",
              req->bufnum);
      for (b = 0; b < (req->bufnum - 1); b++)
        if (req->bufs[b].block >= req->bufs[b + 1].block)
          printf ("disk ioctl: out of order: index:%d "\
                  "(%" PRIdrtems_blkdev_bnum " >= %" PRIdrtems_blkdev_bnum "\n",
                  b, req->bufs[b].block, req->bufs[b + 1].block);
      break;

    default:
      printf ("disk ioctl: invalid action: %d\n",
                         bdd->driver_action);
      result = false;
      break;
  }

  return result;
}

static bool
bdbuf_disk_ioctl_leave (bdbuf_disk* bdd, int buffer_count)
{
  /*
   * Handle the watcher.
   */
  bdbuf_disk_ioctl_watcher (bdd, buffer_count);

  return true;
}

/**
 * BDBuf disk IOCTL handler.
 *
 * @param dd Disk device.
 * @param req IOCTL request code.
 * @param argp IOCTL argument.
 * @retval The IOCTL return value
 */
static int
bdbuf_disk_ioctl (rtems_disk_device *dd, uint32_t req, void* argp)
{
  rtems_blkdev_request *r = argp;
  bdbuf_disk           *bdd = rtems_disk_get_driver_data (dd);

  errno = 0;

  if (!bdbuf_disk_lock (bdd))
  {
    errno = EIO;
  }
  else
  {
    switch (req)
    {
      case RTEMS_BLKIO_REQUEST:
        switch (r->req)
        {
          case RTEMS_BLKDEV_REQ_READ:
            if (!bdbuf_disk_ioctl_process (bdd, r))
              rtems_blkdev_request_done(r, RTEMS_IO_ERROR);
            else
            {
              rtems_blkdev_sg_buffer* sg = r->bufs;
              uint32_t                block = RTEMS_BLKDEV_START_BLOCK (r);
              uint32_t                b;
              int32_t                 remains;

              remains = r->bufnum * bdd->block_size;

              for (b = 0; b < r->bufnum; b++, block++, sg++)
              {
                uint32_t length = sg->length;

                if (sg->length != bdd->block_size)
                  if (length > bdd->block_size)
                    length = bdd->block_size;

                memset (sg->buffer, block, length);

                remains -= length;
              }

              rtems_blkdev_request_done (r, RTEMS_SUCCESSFUL);
            }
            bdbuf_disk_ioctl_leave (bdd, r->bufnum);
            break;

          case RTEMS_BLKDEV_REQ_WRITE:
            if (!bdbuf_disk_ioctl_process (bdd, r))
              rtems_blkdev_request_done(r, RTEMS_IO_ERROR);
            else
              rtems_blkdev_request_done(r, RTEMS_SUCCESSFUL);
            bdbuf_disk_ioctl_leave (bdd, r->bufnum);
            break;

          default:
            errno = EINVAL;
            break;
        }
        break;

      default:
        rtems_blkdev_ioctl (dd, req, argp);
        break;
    }

    if (!bdbuf_disk_unlock (bdd))
      errno = EIO;
  }

  return errno == 0 ? 0 : -1;
}

static rtems_status_code
bdbuf_disk_initialize(void)
{
  uint32_t minor;

  printf ("register disks\n");

  for (minor = 0; minor < BDBUF_DISKS; minor++)
  {
    char              name[sizeof (BDBUF_DISK_DEVICE_BASE_NAME) + 10];
    bdbuf_disk*       bdd = &bdbuf_disks[minor];
    rtems_status_code sc;
    int               fd;
    int               rv;

    snprintf (name, sizeof (name),
              BDBUF_DISK_DEVICE_BASE_NAME "%" PRIu32, minor);

    bdd->name = strdup (name);
    bdd->minor = minor;

    printf ("disk init: %s\n", bdd->name);
    printf ("disk lock: ");

    sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'D', 'K'), 1,
                                 RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE |
                                 RTEMS_INHERIT_PRIORITY, 0, &bdd->lock);
    if (!bdbuf_test_print_sc (sc, true))
      return RTEMS_IO_ERROR;

    bdd->block_size  = 512 * (minor + 1);
    bdd->block_count = BDBUF_SIZE * (minor + 1);

    sc = rtems_blkdev_create(name, bdd->block_size, bdd->block_count,
                             bdbuf_disk_ioctl, bdd);
    if (sc != RTEMS_SUCCESSFUL)
    {
      printf ("disk init: create phys failed: ");
      bdbuf_test_print_sc (sc, true);
      return sc;
    }

    fd = open(name, O_RDWR);
    rtems_test_assert(fd >= 0);

    rv = rtems_disk_fd_get_disk_device(fd, &bdd->dd);
    rtems_test_assert(rv == 0);

    rv = close(fd);
    rtems_test_assert(rv == 0);
  }

  return RTEMS_SUCCESSFUL;
}

static bool
bdbuf_tests_create_task (bdbuf_task_control* tc,
                         rtems_task_priority priority,
                         rtems_task_entry    entry_point)
{
  rtems_status_code sc;

  printf ("creating task: %s: priority: %" PRIdrtems_task_priority ": ",
                     tc->name, priority);

  sc = rtems_task_create (rtems_build_name (tc->name[0], tc->name[1],
                                            tc->name[2], tc->name[3]),
                          priority,
                          BDBUF_TEST_STACK_SIZE,
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR,
                          &tc->task);
  if (!bdbuf_test_print_sc (sc, true))
    return false;

  printf ("starting task: %s: ", tc->name);

  sc = rtems_task_start (tc->task, entry_point, (rtems_task_argument) tc);

  return bdbuf_test_print_sc (sc, true);
}

/**
 * Get the block 0 buffer twice. The first time it is requested it
 * will be taken from the empty list and returned to the LRU list.
 * The second time it will be removed from the LRU list.
 */
static void
bdbuf_tests_task_0_test_1 (bdbuf_task_control* tc)
{
  rtems_status_code   sc;
  bool                passed;
  int                 i;
  rtems_bdbuf_buffer* bd;

  /*
   * Set task control's passed to false to handle a timeout.
   */
  tc->passed = false;
  passed = true;

  for (i = 0; (i < 2) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_get[0]: ", tc->name);
    sc = rtems_bdbuf_get (tc->bdd->dd, 0, &bd);
    if (!bdbuf_test_print_sc (sc, true))
    {
      passed = false;
      break;
    }

    printf ("%s: rtems_bdbuf_release[0]: ", tc->name);
    sc = rtems_bdbuf_release (bd);
    if (!bdbuf_test_print_sc (sc, true))
    {
      passed = false;
      break;
    }
  }

  tc->passed = passed;
  tc->test = 0;
}

/**
 * Get the blocks 0 -> 4 and hold them. Wake the master to tell it was have the
 * buffers then wait for the master to tell us to release a single buffer.
 * Task 1 will be block waiting for each buffer. It is a higher priority.
 */
static void
bdbuf_tests_task_0_test_2 (bdbuf_task_control* tc)
{
  rtems_status_code   sc;
  bool                passed;
  int                 i;
  rtems_bdbuf_buffer* bd;
  rtems_chain_control buffers;

  /*
   * Set task control's passed to false to handle a timeout.
   */
  tc->passed = false;
  passed = true;

  /*
   * Get the blocks 0 -> 4 and hold them.
   */
  rtems_chain_initialize_empty (&buffers);

  for (i = 0; (i < 5) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_get[%d]: ", tc->name, i);
    sc = rtems_bdbuf_get (tc->bdd->dd, i, &bd);
    if (!bdbuf_test_print_sc (sc, true))
      passed = false;

    rtems_chain_append (&buffers, &bd->link);
  }

  /*
   * Wake the master to tell it we have the buffers.
   */
  bdbuf_send_wait_event (tc->name, "wake master", tc->master);

  if (passed)
  {
    /*
     * For each buffer we hold wait until the master wakes us
     * and then return it. Task 2 will block waiting for this
     * buffer. It is a higher priority task.
     */
    for (i = 0; (i < 5) && passed; i++)
    {
      sc = bdbuf_wait (tc->name, BDBUF_SECONDS (5));
      if (sc != RTEMS_SUCCESSFUL)
      {
        printf ("%s: wait failed: ", tc->name);
        bdbuf_test_print_sc (sc, true);
        passed = false;
        break;
      }
      else
      {
        printf ("%s: rtems_bdbuf_release[%d]: unblocks task 1\n",
                           tc->name, i);
        bd = (rtems_bdbuf_buffer*) rtems_chain_get (&buffers);
        sc = rtems_bdbuf_release (bd);
        printf ("%s: rtems_bdbuf_release[%d]: ", tc->name, i);
        if (!bdbuf_test_print_sc (sc, true))
        {
          passed = false;
          break;
        }
      }
    }
  }

  tc->passed = passed;
  tc->test = 0;
}

/**
 * Read the block 5 from the disk modify it then release it modified.
 */
static void
bdbuf_tests_task_0_test_3 (bdbuf_task_control* tc)
{
  rtems_status_code   sc;
  bool                passed;
  rtems_bdbuf_buffer* bd;

  /*
   * Set task control's passed to false to handle a timeout.
   */
  tc->passed = false;
  passed = true;

  bdbuf_disk_lock (tc->bdd);
  tc->bdd->driver_action = BDBUF_DISK_NOOP;
  bdbuf_disk_unlock (tc->bdd);

  /*
   * Read the buffer and then release it.
   */
  printf ("%s: rtems_bdbuf_read[5]: ", tc->name);
  sc = rtems_bdbuf_read (tc->bdd->dd, 5, &bd);
  if ((passed = bdbuf_test_print_sc (sc, true)))
  {
    printf ("%s: rtems_bdbuf_release_modified[5]: ", tc->name);
    sc = rtems_bdbuf_release_modified (bd);
    passed = bdbuf_test_print_sc (sc, true);
  }

  /*
   * Read the buffer again and then just release. The buffer should
   * be maintained as modified.
   */
  printf ("%s: rtems_bdbuf_read[5]: ", tc->name);
  sc = rtems_bdbuf_read (tc->bdd->dd, 5, &bd);
  if ((passed = bdbuf_test_print_sc (sc, true)))
  {
    printf ("%s: rtems_bdbuf_release[5]: ", tc->name);
    sc = rtems_bdbuf_release (bd);
    passed = bdbuf_test_print_sc (sc, true);
  }

  /*
   * Set up a disk watch and wait for the write to happen.
   */
  bdbuf_set_disk_driver_watch (tc, 1);
  passed = bdbuf_disk_driver_watch_wait (tc, BDBUF_SECONDS (5));

  tc->passed = passed;
  tc->test = 0;
}

static size_t
bdbuf_test_buffer_count (void)
{
  return rtems_bdbuf_configuration.size / rtems_bdbuf_configuration.buffer_min;
}

/**
 * Get all the blocks in the pool and hold them. Wake the master to tell it was
 * have the buffers then wait for the master to tell us to release them.
 */
static void
bdbuf_tests_task_0_test_4 (bdbuf_task_control* tc)
{
  rtems_status_code   sc;
  bool                passed;
  size_t              i;
  rtems_bdbuf_buffer* bd;
  rtems_chain_control buffers;
  size_t              num = bdbuf_test_buffer_count ();

  /*
   * Set task control's passed to false to handle a timeout.
   */
  tc->passed = false;
  passed = true;

  /*
   * Clear any disk settings.
   */
  bdbuf_clear_disk_driver_watch (tc);
  bdbuf_set_disk_driver_action (tc, BDBUF_DISK_NOOP);

  /*
   * Get the blocks 0 -> 4 and hold them.
   */
  rtems_chain_initialize_empty (&buffers);

  for (i = 0; (i < num) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_read[%zd]: ", tc->name, i);
    sc = rtems_bdbuf_read (tc->bdd->dd, i, &bd);
    if (!bdbuf_test_print_sc (sc, true))
      passed = false;

    rtems_chain_append (&buffers, &bd->link);
  }

  /*
   * Wake the master to tell it we have the buffers.
   */
  bdbuf_send_wait_event (tc->name, "wake master", tc->master);

  if (passed)
  {
    bdbuf_sleep (250);

    bdbuf_set_disk_driver_watch (tc, num / 2);

    /*
     * Release half the buffers, wait 500msecs then release the
     * remainder. This tests the swap out timer on each buffer.
     */
    printf ("%s: rtems_bdbuf_release_modified[0]: unblocks task 1\n",
                       tc->name);
    bd = (rtems_bdbuf_buffer*) rtems_chain_get (&buffers);
    sc = rtems_bdbuf_release_modified (bd);
    printf ("%s: rtems_bdbuf_release_modified[0]: ", tc->name);
    passed = bdbuf_test_print_sc (sc, true);
    if (passed)
    {
      for (i = 1; (i < (num / 2)) && passed; i++)
      {
        printf ("%s: rtems_bdbuf_release_modified[%zd]: " \
                           "unblocks task 1\n", tc->name, i);
        bd = (rtems_bdbuf_buffer*) rtems_chain_get (&buffers);
        sc = rtems_bdbuf_release_modified (bd);
        printf ("%s: rtems_bdbuf_release_modified[%zd]: ", tc->name, i);
        passed = bdbuf_test_print_sc (sc, true);
        if (!passed)
          break;
      }

      if (passed)
      {
        passed = bdbuf_disk_driver_watch_wait (tc, BDBUF_SECONDS (5));

        if (passed)
        {
          bdbuf_sleep (500);

          bdbuf_set_disk_driver_watch (tc, num / 2);

          for (i = 0; (i < (num / 2)) && passed; i++)
          {
            printf ("%s: rtems_bdbuf_release_modified[%zd]: ",
                               tc->name, i + (num / 2));
            bd = (rtems_bdbuf_buffer*) rtems_chain_get (&buffers);
            passed = bdbuf_test_print_sc (rtems_bdbuf_release_modified (bd),
                                          true);
            if (!passed)
              break;
          }

          passed = bdbuf_disk_driver_watch_wait (tc, BDBUF_SECONDS (5));

          if (passed)
          {
            if (!rtems_chain_is_empty (&buffers))
            {
              passed = false;
              printf ("%s: buffer chain not empty\n", tc->name);
            }
          }
        }
      }
    }
  }

  tc->passed = passed;
  tc->test = 0;
}

static void
bdbuf_tests_task_0_test_5 (bdbuf_task_control* tc)
{
  bdbuf_tests_task_0_test_4 (tc);
}

static void
bdbuf_tests_task_0_test_6 (bdbuf_task_control* tc)
{
  rtems_status_code   sc;
  bool                passed;
  int                 i;
  rtems_bdbuf_buffer* bd;
  rtems_chain_control buffers;

  /*
   * Set task control's passed to false to handle a timeout.
   */
  tc->passed = false;
  passed = true;

  /*
   * Clear any disk settings.
   */
  bdbuf_clear_disk_driver_watch (tc);
  bdbuf_set_disk_driver_action (tc, BDBUF_DISK_NOOP);

  /*
   * Get the blocks 0 -> 4 and hold them.
   */
  rtems_chain_initialize_empty (&buffers);

  for (i = 0; (i < 5) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_read[%d]: ", tc->name, i);
    sc = rtems_bdbuf_get (tc->bdd->dd, i, &bd);
    if (!bdbuf_test_print_sc (sc, true))
      passed = false;

    rtems_chain_append (&buffers, &bd->link);
  }

  for (i = 0; (i < 4) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_release_modified[%d]: ",
                       tc->name, i);
    bd = (rtems_bdbuf_buffer*) rtems_chain_get (&buffers);
    passed = bdbuf_test_print_sc (rtems_bdbuf_release_modified (bd),
                                  true);
  }

  if (passed)
  {
    printf ("%s: rtems_bdbuf_sync[%d]: ", tc->name, i);
    bd = (rtems_bdbuf_buffer*) rtems_chain_get (&buffers);

    passed = bdbuf_test_print_sc (rtems_bdbuf_sync (bd), true);
  }

  tc->passed = passed;
  tc->test = 0;
}

static void
bdbuf_tests_task_0_test_7 (bdbuf_task_control* tc)
{
  rtems_status_code   sc;
  bool                passed;
  int                 i;
  rtems_bdbuf_buffer* bd;
  rtems_chain_control buffers;

  /*
   * Set task control's passed to false to handle a timeout.
   */
  tc->passed = false;
  passed = true;

  /*
   * Clear any disk settings.
   */
  bdbuf_clear_disk_driver_watch (tc);
  bdbuf_set_disk_driver_action (tc, BDBUF_DISK_NOOP);

  /*
   * Get the blocks 0 -> 4 and hold them.
   */
  rtems_chain_initialize_empty (&buffers);

  for (i = 0; (i < 5) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_read[%d]: ", tc->name, i);
    sc = rtems_bdbuf_get (tc->bdd->dd, i, &bd);
    if (!bdbuf_test_print_sc (sc, true))
      passed = false;

    rtems_chain_append (&buffers, &bd->link);
  }

  for (i = 0; (i < 5) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_release_modified[%d]: ",
                       tc->name, i);
    bd = (rtems_bdbuf_buffer*) rtems_chain_get (&buffers);
    passed = bdbuf_test_print_sc (rtems_bdbuf_release_modified (bd),
                                  true);
  }

  if (passed)
  {
    printf ("%s: rtems_bdbuf_syncdev[%" PRIu32 ": ",
                       tc->name,
                       tc->bdd->minor);
    passed = bdbuf_test_print_sc (rtems_bdbuf_syncdev (tc->bdd->dd), true);
  }

  tc->passed = passed;
  tc->test = 0;
}

static void
bdbuf_tests_task_0_test_8 (bdbuf_task_control* tc)
{
  rtems_status_code   sc;
  bool                passed;
  int                 i;
  rtems_bdbuf_buffer* bd;
  rtems_chain_control buffers;
  rtems_chain_node*   node;
  rtems_chain_node*   pnode;

  /*
   * Set task control's passed to false to handle a timeout.
   */
  tc->passed = false;
  passed = true;

  /*
   * Clear any disk settings.
   */
  bdbuf_clear_disk_driver_watch (tc);
  bdbuf_set_disk_driver_action (tc, BDBUF_DISK_NOOP);

  /*
   * Get the blocks 0 -> 4 and hold them.
   */
  rtems_chain_initialize_empty (&buffers);

  for (i = 0; (i < 5) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_read[%d]: ", tc->name, i);
    sc = rtems_bdbuf_get (tc->bdd->dd, i, &bd);
    if (!bdbuf_test_print_sc (sc, true))
      passed = false;

    rtems_chain_append (&buffers, &bd->link);
  }

  node = rtems_chain_tail (&buffers);
  node = node->previous;

  bd = (rtems_bdbuf_buffer*) node;
  pnode = node->previous;
  rtems_chain_extract (node);
  node = pnode;
  printf ("%s: rtems_bdbuf_release_modified[4]: ", tc->name);
  passed = bdbuf_test_print_sc (rtems_bdbuf_release_modified (bd), true);

  bd = (rtems_bdbuf_buffer*) node;
  pnode = node->previous;
  rtems_chain_extract (node);
  node = pnode;
  printf ("%s: rtems_bdbuf_release_modified[3]: ", tc->name);
  passed = bdbuf_test_print_sc (rtems_bdbuf_release_modified (bd), true);

  for (i = 0; (i < 3) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_release_modified[%d]: ",
                       tc->name, i);
    bd = (rtems_bdbuf_buffer*) rtems_chain_get (&buffers);
    passed = bdbuf_test_print_sc (rtems_bdbuf_release_modified (bd),
                                  true);
  }

  if (passed)
  {
    /*
     * Check the block order.
     */
    bdbuf_set_disk_driver_action (tc, BDBUF_DISK_BLOCKS_INORDER);

    printf ("%s: rtems_bdbuf_syncdev[%" PRIu32 "]: checking order\n",
                       tc->name,
                       tc->bdd->minor);
    sc = rtems_bdbuf_syncdev (tc->bdd->dd);
    printf ("%s: rtems_bdbuf_syncdev[%" PRIu32 "]: ",
                       tc->name,
                       tc->bdd->minor);
    passed = bdbuf_test_print_sc (sc, true);
  }

  tc->passed = passed;
  tc->test = 0;
}

static void
bdbuf_tests_task_0 (rtems_task_argument arg)
{
  bdbuf_task_control* tc = (bdbuf_task_control*) arg;

  while (!tc->die)
  {
    switch (tc->test)
    {
      case 0:
        /*
         * Wait for the next test.
         */
        bdbuf_wait (tc->name, 0);
        break;

      case 1:
        bdbuf_tests_task_0_test_1 (tc);
        break;

      case 2:
        bdbuf_tests_task_0_test_2 (tc);
        break;

      case 3:
        bdbuf_tests_task_0_test_3 (tc);
        break;

      case 4:
        bdbuf_tests_task_0_test_4 (tc);
        break;

      case 5:
        bdbuf_tests_task_0_test_5 (tc);
        break;

      case 6:
        bdbuf_tests_task_0_test_6 (tc);
        break;

      case 7:
        bdbuf_tests_task_0_test_7 (tc);
        break;

      case 8:
        bdbuf_tests_task_0_test_8 (tc);
        break;

      default:
        /*
         * Invalid test for this task. An error.
         */
        printf ("%s: invalid test: %d\n", tc->name, tc->test);
        tc->passed = false;
        tc->test = 0;
        break;
    }
  }

  printf ("%s: delete task\n", tc->name);
  rtems_task_exit();
}

/**
 * Get the blocks 0 -> 4 and release them. Task 0 should be holding
 * each one.
 */
static void
bdbuf_tests_ranged_get_release (bdbuf_task_control* tc,
                                bool                wake_master,
                                int                 lower,
                                int                 upper)
{
  rtems_status_code   sc;
  bool                passed;
  int                 i;
  rtems_bdbuf_buffer* bd;

  /*
   * Set task control's passed to false to handle a timeout.
   */
  tc->passed = false;
  passed = true;

  for (i = lower; (i < upper) && passed; i++)
  {
    printf ("%s: rtems_bdbuf_get[%d]: blocking ...\n", tc->name, i);
    sc = rtems_bdbuf_get (tc->bdd->dd, i, &bd);
    printf ("%s: rtems_bdbuf_get[%d]: ", tc->name, i);
    if (!bdbuf_test_print_sc (sc, true))
    {
      passed = false;
      break;
    }

    printf ("%s: rtems_bdbuf_release[%d]: ", tc->name, i);
    sc = rtems_bdbuf_release (bd);
    if (!bdbuf_test_print_sc (sc, true))
    {
      passed = false;
      break;
    }

    /*
     * Wake the master to tell it we have finished.
     */
    if (wake_master)
      bdbuf_send_wait_event (tc->name, "wake master", tc->master);
  }

  tc->passed = passed;
  tc->test = 0;
}

static void
bdbuf_tests_task_1 (rtems_task_argument arg)
{
  bdbuf_task_control* tc = (bdbuf_task_control*) arg;

  while (!tc->die)
  {
    switch (tc->test)
    {
      case 0:
        /*
         * Wait for the next test.
         */
        bdbuf_wait (tc->name, 0);
        break;

      case 2:
        bdbuf_tests_ranged_get_release (tc, false, 0, 5);
        break;

      case 4:
        bdbuf_tests_ranged_get_release (tc, false, 0, 9);
        break;

      case 5:
        bdbuf_tests_ranged_get_release (tc, false, 20, 25);
        break;

      default:
        /*
         * Invalid test for this task. An error.
         */
        printf ("%s: invalid test: %d\n", tc->name, tc->test);
        tc->passed = false;
        tc->test = 0;
        break;
    }
  }

  printf ("%s: delete task\n", tc->name);
  rtems_task_exit();
}

/**
 * Get the blocks 0 -> 4 and release them. Task 0 should be holding
 * each one.
 */
static void
bdbuf_tests_task_2_test_2 (bdbuf_task_control* tc)
{
  /*
   * Use task 1's test 2. They are the same.
   */
  bdbuf_tests_ranged_get_release (tc, true, 0, 5);
}

static void
bdbuf_tests_task_2 (rtems_task_argument arg)
{
  bdbuf_task_control* tc = (bdbuf_task_control*) arg;

  while (!tc->die)
  {
    switch (tc->test)
    {
      case 0:
        /*
         * Wait for the next test.
         */
        bdbuf_wait (tc->name, 0);
        break;

      case 2:
        bdbuf_tests_task_2_test_2 (tc);
        break;

      default:
        /*
         * Invalid test for this task. An error.
         */
        printf ("%s: invalid test: %d\n", tc->name, tc->test);
        tc->passed = false;
        tc->test = 0;
        break;
    }
  }

  printf ("%s: delete task\n", tc->name);
  rtems_task_exit();
}

/**
 * Table of task entry points.
 */
static rtems_task_entry bdbuf_test_tasks[] =
{
  bdbuf_tests_task_0,
  bdbuf_tests_task_1,
  bdbuf_tests_task_2
};

#define BDBUF_TESTS_PRI_HIGH (30)

/**
 * Wait for the all tests to finish. This is signalled by the test
 * number becoming 0.
 */
static bool
bdbuf_tests_finished (bdbuf_task_control* tasks)
{
  uint32_t time = 0;
  bool     finished = false;

  while (time < (10 * 4))
  {
    int t;

    finished = true;

    for (t = 0; t < BDBUF_TEST_TASKS; t++)
      if (tasks[t].test)
      {
        finished = false;
        break;
      }

    if (finished)
      break;

    bdbuf_sleep (250);
    time++;
  }

  if (!finished)
    printf ("master: test timed out\n");
  else
  {
    int t;
    for (t = 0; t < BDBUF_TEST_TASKS; t++)
      if (!tasks[0].passed)
      {
        finished = false;
        break;
      }
  }

  return finished;
}

/**
 * Test 1.
 *
 * # Get task 0 to get buffer 0 from the pool then release it twice.
 */
static bool
bdbuf_test_1 (bdbuf_task_control* tasks)
{
  tasks[0].test = 1;

  /*
   * Use pool 0.
   */
  tasks[0].bdd = &bdbuf_disks[0];

  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  return bdbuf_tests_finished (tasks);
}

/**
 * Test 2.
 *
 * # Get task 0 to get buffers 0 -> 4 from the pool hold them. Then get
 * task 1 and task 2 to get them with blocking. The 2 tasks tests the
 * priority blocking on the buffer.
 */
static bool
bdbuf_test_2 (bdbuf_task_control* tasks)
{
  int i;

  tasks[0].test = 2;
  tasks[1].test = 2;
  tasks[2].test = 2;

  /*
   * Use pool 0.
   */
  tasks[0].bdd = &bdbuf_disks[0];
  tasks[1].bdd = &bdbuf_disks[0];
  tasks[2].bdd = &bdbuf_disks[0];

  /*
   * Wake task 0 and wait for it to have all the buffers.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);
  if (bdbuf_wait ("master", BDBUF_SECONDS (5)) != RTEMS_SUCCESSFUL)
    return false;

  /*
   * Wake task 1.
   */
  bdbuf_send_wait_event ("master", "wake task 1", tasks[1].task);

  /*
   * Wake task 2.
   */
  bdbuf_send_wait_event ("master", "wake task 2", tasks[2].task);

  for (i = 0; i < 5; i++)
  {
    /*
     * Wake task 0 and watch task 2 then task 1 get the released buffer.
     */
    bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

    /*
     * Wait until task 2 has the buffer.
     */
    if (bdbuf_wait ("master", BDBUF_SECONDS (5)) != RTEMS_SUCCESSFUL)
      return false;
  }

  /*
   * Wait for the tests to finish.
   */
  return bdbuf_tests_finished (tasks);
}

/**
 * Test 3.
 *
 * # Read a block from disk into the buffer, modify the block and release
 * it modified. Use a block great then 4 because 0 -> 4 are in the cache.
 */
static bool
bdbuf_test_3 (bdbuf_task_control* tasks)
{
  tasks[0].test = 3;

  /*
   * Use pool 0.
   */
  tasks[0].bdd = &bdbuf_disks[0];

  /*
   * Wake task 0.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  return bdbuf_tests_finished (tasks);
}

/**
 * Test 4.
 *
 * # Read every buffer in the pool and hold. Then get task 1 to ask for another
 *   buffer that is being accessed. It will block waiting for it to appear.
 */
static bool
bdbuf_test_4 (bdbuf_task_control* tasks)
{
  tasks[0].test = 4;
  tasks[1].test = 4;

  /*
   * Use pool 0.
   */
  tasks[0].bdd = &bdbuf_disks[0];
  tasks[1].bdd = &bdbuf_disks[0];

  /*
   * Wake task 0.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  /*
   * Wait for the buffers in the pool to be taken.
   */
  if (bdbuf_wait ("master", BDBUF_SECONDS (5)) != RTEMS_SUCCESSFUL)
    return false;

  bdbuf_sleep (100);

  /*
   * Wake task 1 to read another one and have to block.
   */
  bdbuf_send_wait_event ("master", "wake task 1", tasks[1].task);

  bdbuf_sleep (100);

  /*
   * Wake task 0 to release it buffers.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  return bdbuf_tests_finished (tasks);
}

/**
 * Test 5.
 *
 * # Read every buffer in the pool and hold. Then get task 1 to ask for a new
 *   buffer. It will block waiting for one to appear.
 */
static bool
bdbuf_test_5 (bdbuf_task_control* tasks)
{
  tasks[0].test = 5;
  tasks[1].test = 5;

  /*
   * Use pool 0.
   */
  tasks[0].bdd = &bdbuf_disks[0];
  tasks[1].bdd = &bdbuf_disks[0];

  /*
   * Wake task 0.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  /*
   * Wait for the buffers in the pool to be taken.
   */
  if (bdbuf_wait ("master", BDBUF_SECONDS (5)) != RTEMS_SUCCESSFUL)
    return false;

  bdbuf_sleep (100);

  /*
   * Wake task 1 to read another one and have to block.
   */
  bdbuf_send_wait_event ("master", "wake task 1", tasks[1].task);

  bdbuf_sleep (100);

  /*
   * Wake task 0 to release it buffers.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  return bdbuf_tests_finished (tasks);
}

/**
 * Test 6.
 *
 * # Get 5 buffers, release modify 4 then sync the last.
 */
static bool
bdbuf_test_6 (bdbuf_task_control* tasks)
{
  tasks[0].test = 6;

  /*
   * Use pool 0.
   */
  tasks[0].bdd = &bdbuf_disks[0];

  /*
   * Wake task 0.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  return bdbuf_tests_finished (tasks);
}

/**
 * Test 7.
 *
 * # Get 5 buffers, release modify them all then sync the device.
 */
static bool
bdbuf_test_7 (bdbuf_task_control* tasks)
{
  tasks[0].test = 7;

  /*
   * Use pool 0.
   */
  tasks[0].bdd = &bdbuf_disks[0];

  /*
   * Wake task 0.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  return bdbuf_tests_finished (tasks);
}

/**
 * Test 8.
 *
 * # Get 5 buffers, release modify the last 2 then the reset from 0.
 */
static bool
bdbuf_test_8 (bdbuf_task_control* tasks)
{
  tasks[0].test = 8;

  /*
   * Use pool 0.
   */
  tasks[0].bdd = &bdbuf_disks[0];

  /*
   * Wake task 0.
   */
  bdbuf_send_wait_event ("master", "wake task 0", tasks[0].task);

  return bdbuf_tests_finished (tasks);
}

/**
 * A test.
 */
typedef bool (*bdbuf_test) (bdbuf_task_control* tasks);

/**
 * A test name and function.
 */
typedef struct bdbuf_test_ident
{
  const char* label;
  bdbuf_test  test;
} bdbuf_test_ident;

/**
 * Table of tests.
 */
static bdbuf_test_ident bdbuf_tests[] =
{
  {
    "Task 0 get buffer 0 from pool 0",
    bdbuf_test_1
  },
  {
    "Task 0 get buffer 0 -> 4 from pool 0, task 2 and 1 block getting",
    bdbuf_test_2
  },
  {
    "Task 0 read buffer 5, modify and release modified",
    bdbuf_test_3
  },
  {
    "Task 0 read all buffers, task 1 blocks waiting for acessed buffer",
    bdbuf_test_4
  },
  {
    "Task 0 read all buffers, task 1 blocks waiting for new buffer",
    bdbuf_test_5
  },
  {
    "Task 0 release modified 4 buffers then syncs a 5th buffer",
    bdbuf_test_6
  },
  {
    "Task 0 release modified 5 buffers then sync the device",
    bdbuf_test_7
  },
  {
    "Task 0 releases modified 5 buffers is out or order sequence and the" \
    " driver checks the buffers are in order",
    bdbuf_test_8
  }
};

#define BDBUF_TEST_NUM (sizeof (bdbuf_tests) / sizeof (bdbuf_test_ident))

/**
 * Test the BD Buffering code.
 */
static void
bdbuf_tester (void)
{
  bdbuf_task_control        tasks[BDBUF_TEST_TASKS];
  rtems_task_priority       old_priority;
  int                       t;
  bool                      passed = true;
  rtems_status_code         sc;

  sc = bdbuf_disk_initialize();
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /*
   * Change priority to a lower one.
   */
  printf ("lower priority to %d: ", BDBUF_TESTS_PRI_HIGH + 1);
  bdbuf_test_print_sc (rtems_task_set_priority (RTEMS_SELF,
                                                BDBUF_TESTS_PRI_HIGH + 1,
                                                &old_priority),
                       true);

  /*
   * Make sure the swapout task has run. The user could block
   * the swapout task from running until later. This is not
   * tested.
   */
  bdbuf_sleep (100);

  /*
   * Start the test tasks used to test the threading parts
   * of the bdbuf code.
   */
  for (t = 0; t < BDBUF_TEST_TASKS; t++)
  {
    bdbuf_task_control_init (t, &tasks[t],
                             rtems_task_self ());

    if (!bdbuf_tests_create_task (&tasks[t],
                                  BDBUF_TESTS_PRI_HIGH - t,
                                  bdbuf_test_tasks[t]))
    return;
  }

  /*
   * Let the test tasks run if they have not already done so.
   */
  bdbuf_sleep (100);

  /*
   * Perform each test.
   */
  for (t = 0; (t < BDBUF_TEST_NUM) && passed; t++)
  {
    printf ("test %d: %s\n", t + 1, bdbuf_tests[t].label);
    passed = bdbuf_tests[t].test (tasks);
    printf ("test %d: %s\n", t + 1, passed ? "passed" : "failed");
  }
}

static rtems_task Init(rtems_task_argument argument)
{
  TEST_BEGIN();

  bdbuf_tester ();

  TEST_END();

  exit (0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_BDBUF_TASK_STACK_SIZE BDBUF_TEST_STACK_SIZE

#define CONFIGURE_MAXIMUM_TASKS (1 + BDBUF_TEST_TASKS)
#define CONFIGURE_MAXIMUM_SEMAPHORES 2

#define CONFIGURE_EXTRA_TASK_STACKS \
  (BDBUF_TEST_TASKS * BDBUF_TEST_STACK_SIZE)

#define CONFIGURE_INIT_TASK_STACK_SIZE BDBUF_TEST_STACK_SIZE
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
