/**
 * @file
 *
 * @ingroup rtems_bdbuf
 *
 * Block device buffer management.
 */

/*
 * Disk I/O buffering
 * Buffer managment
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Peterburg, Russia
 * Author: Andrey G. Ivanov <Andrey.Ivanov@oktet.ru>
 *         Victor V. Vengerov <vvv@oktet.ru>
 *         Alexander Kukuta <kam@oktet.ru>
 *
 * Copyright (C) 2008,2009 Chris Johns <chrisj@rtems.org>
 *    Rewritten to remove score mutex access. Fixes many performance
 *    issues.
 * 
 * @(#) bdbuf.c,v 1.14 2004/04/17 08:15:17 ralf Exp
 */

/**
 * Set to 1 to enable debug tracing.
 */
#define RTEMS_BDBUF_TRACE 0

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/malloc.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#if RTEMS_BDBUF_TRACE
#include <stdio.h>
#endif

#include "rtems/bdbuf.h"

/*
 * Simpler label for this file.
 */
#define bdbuf_config rtems_bdbuf_configuration

/**
 * A swapout transfer transaction data. This data is passed to a worked thread
 * to handle the write phase of the transfer.
 */
typedef struct rtems_bdbuf_swapout_transfer
{
  rtems_chain_control   bds;       /**< The transfer list of BDs. */
  dev_t                 dev;       /**< The device the transfer is for. */
  rtems_blkdev_request* write_req; /**< The write request array. */
} rtems_bdbuf_swapout_transfer;

/**
 * Swapout worker thread. These are available to take processing from the
 * main swapout thread and handle the I/O operation.
 */
typedef struct rtems_bdbuf_swapout_worker
{
  rtems_chain_node             link;     /**< The threads sit on a chain when
                                          * idle. */
  rtems_id                     id;       /**< The id of the task so we can wake
                                          * it. */
  volatile bool                enabled;  /**< The worked is enabled. */
  rtems_bdbuf_swapout_transfer transfer; /**< The transfer data for this
                                          * thread. */
} rtems_bdbuf_swapout_worker;

/**
 * The BD buffer cache.
 */
typedef struct rtems_bdbuf_cache
{
  rtems_id            swapout;           /**< Swapout task ID */
  volatile bool       swapout_enabled;   /**< Swapout is only running if
                                          * enabled. Set to false to kill the
                                          * swap out task. It deletes itself. */
  rtems_chain_control swapout_workers;   /**< The work threads for the swapout
                                          * task. */
  
  rtems_bdbuf_buffer* bds;               /**< Pointer to table of buffer
                                          * descriptors. */
  void*               buffers;           /**< The buffer's memory. */
  size_t              buffer_min_count;  /**< Number of minimum size buffers
                                          * that fit the buffer memory. */
  size_t              max_bds_per_group; /**< The number of BDs of minimum
                                          * buffer size that fit in a group. */
  uint32_t            flags;             /**< Configuration flags. */

  rtems_id            lock;              /**< The cache lock. It locks all
                                          * cache data, BD and lists. */
  rtems_id            sync_lock;         /**< Sync calls block writes. */
  volatile bool       sync_active;       /**< True if a sync is active. */
  volatile rtems_id   sync_requester;    /**< The sync requester. */
  volatile dev_t      sync_device;       /**< The device to sync and -1 not a
                                          * device sync. */

  rtems_bdbuf_buffer* tree;              /**< Buffer descriptor lookup AVL tree
                                          * root. There is only one. */
  rtems_chain_control ready;             /**< Free buffers list, read-ahead, or
                                          * resized group buffers. */
  rtems_chain_control lru;               /**< Least recently used list */
  rtems_chain_control modified;          /**< Modified buffers list */
  rtems_chain_control sync;              /**< Buffers to sync list */

  rtems_id            access;            /**< Obtain if waiting for a buffer in
                                          * the ACCESS state. */
  volatile uint32_t   access_waiters;    /**< Count of access blockers. */
  rtems_id            transfer;          /**< Obtain if waiting for a buffer in
                                          * the TRANSFER state. */
  volatile uint32_t   transfer_waiters;  /**< Count of transfer blockers. */
  rtems_id            waiting;           /**< Obtain if waiting for a buffer
                                          * and the none are available. */
  volatile uint32_t   wait_waiters;      /**< Count of waiting blockers. */

  size_t              group_count;       /**< The number of groups. */
  rtems_bdbuf_group*  groups;            /**< The groups. */
  
  bool                initialised;       /**< Initialised state. */
} rtems_bdbuf_cache;

/**
 * Fatal errors
 */
#define RTEMS_BLKDEV_FATAL_ERROR(n) \
  (((uint32_t)'B' << 24) | ((uint32_t)(n) & (uint32_t)0x00FFFFFF))

#define RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY  RTEMS_BLKDEV_FATAL_ERROR(1)
#define RTEMS_BLKDEV_FATAL_BDBUF_SWAPOUT      RTEMS_BLKDEV_FATAL_ERROR(2)
#define RTEMS_BLKDEV_FATAL_BDBUF_SYNC_LOCK    RTEMS_BLKDEV_FATAL_ERROR(3)
#define RTEMS_BLKDEV_FATAL_BDBUF_SYNC_UNLOCK  RTEMS_BLKDEV_FATAL_ERROR(4)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_LOCK   RTEMS_BLKDEV_FATAL_ERROR(5)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_UNLOCK RTEMS_BLKDEV_FATAL_ERROR(6)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_1 RTEMS_BLKDEV_FATAL_ERROR(7)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_2 RTEMS_BLKDEV_FATAL_ERROR(8)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_3 RTEMS_BLKDEV_FATAL_ERROR(9)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAKE   RTEMS_BLKDEV_FATAL_ERROR(10)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_WAKE      RTEMS_BLKDEV_FATAL_ERROR(11)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_NOMEM     RTEMS_BLKDEV_FATAL_ERROR(12)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_WK_CREATE RTEMS_BLKDEV_FATAL_ERROR(13)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_WK_START  RTEMS_BLKDEV_FATAL_ERROR(14)
#define BLKDEV_FATAL_BDBUF_SWAPOUT_RE         RTEMS_BLKDEV_FATAL_ERROR(15)
#define BLKDEV_FATAL_BDBUF_SWAPOUT_TS         RTEMS_BLKDEV_FATAL_ERROR(16)

/**
 * The events used in this code. These should be system events rather than
 * application events.
 */
#define RTEMS_BDBUF_TRANSFER_SYNC  RTEMS_EVENT_1
#define RTEMS_BDBUF_SWAPOUT_SYNC   RTEMS_EVENT_2

/**
 * The swap out task size. Should be more than enough for most drivers with
 * tracing turned on.
 */
#define SWAPOUT_TASK_STACK_SIZE (8 * 1024)

/**
 * Lock semaphore attributes. This is used for locking type mutexes.
 *
 * @warning Priority inheritance is on.
 */
#define RTEMS_BDBUF_CACHE_LOCK_ATTRIBS \
  (RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | \
   RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

/**
 * Waiter semaphore attributes.
 *
 * @warning Do not configure as inherit priority. If a driver is in the driver
 *          initialisation table this locked semaphore will have the IDLE task
 *          as the holder and a blocking task will raise the priority of the
 *          IDLE task which can cause unsual side effects.
 */
#define RTEMS_BDBUF_CACHE_WAITER_ATTRIBS \
  (RTEMS_PRIORITY | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
   RTEMS_NO_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

/*
 * The swap out task.
 */
static rtems_task rtems_bdbuf_swapout_task(rtems_task_argument arg);

/**
 * The Buffer Descriptor cache.
 */
static rtems_bdbuf_cache bdbuf_cache;

/**
 * Print a message to the bdbuf trace output and flush it.
 *
 * @param format The format string. See printf for details.
 * @param ... The arguments for the format text.
 * @return int The number of bytes written to the output.
 */
#if RTEMS_BDBUF_TRACE
bool rtems_bdbuf_tracer;
static void
rtems_bdbuf_printf (const char *format, ...)
{
  va_list args;
  va_start (args, format);
  if (rtems_bdbuf_tracer)
  {
    fprintf (stdout, "bdbuf:");
    vfprintf (stdout, format, args);
    fflush (stdout);
  }
}
#endif

/**
 * The default maximum height of 32 allows for AVL trees having between
 * 5,704,880 and 4,294,967,295 nodes, depending on order of insertion.  You may
 * change this compile-time constant as you wish.
 */
#ifndef RTEMS_BDBUF_AVL_MAX_HEIGHT
#define RTEMS_BDBUF_AVL_MAX_HEIGHT (32)
#endif

/**
 * Searches for the node with specified dev/block.
 *
 * @param root pointer to the root node of the AVL-Tree
 * @param dev device search key
 * @param block block search key
 * @retval NULL node with the specified dev/block is not found
 * @return pointer to the node with specified dev/block
 */
static rtems_bdbuf_buffer *
rtems_bdbuf_avl_search (rtems_bdbuf_buffer** root,
                        dev_t                dev,
                        rtems_blkdev_bnum    block)
{
  rtems_bdbuf_buffer* p = *root;

  while ((p != NULL) && ((p->dev != dev) || (p->block != block)))
  {
    if ((p->dev < dev) || ((p->dev == dev) && (p->block < block)))
    {
      p = p->avl.right;
    }
    else
    {
      p = p->avl.left;
    }
  }

  return p;
}

/**
 * Inserts the specified node to the AVl-Tree.
 *
 * @param root pointer to the root node of the AVL-Tree
 * @param node Pointer to the node to add.
 * @retval 0 The node added successfully
 * @retval -1 An error occured
 */
static int
rtems_bdbuf_avl_insert(rtems_bdbuf_buffer** root,
                       rtems_bdbuf_buffer*  node)
{
  dev_t             dev = node->dev;
  rtems_blkdev_bnum block = node->block;

  rtems_bdbuf_buffer*  p = *root;
  rtems_bdbuf_buffer*  q;
  rtems_bdbuf_buffer*  p1;
  rtems_bdbuf_buffer*  p2;
  rtems_bdbuf_buffer*  buf_stack[RTEMS_BDBUF_AVL_MAX_HEIGHT];
  rtems_bdbuf_buffer** buf_prev = buf_stack;

  bool modified = false;

  if (p == NULL)
  {
    *root = node;
    node->avl.left = NULL;
    node->avl.right = NULL;
    node->avl.bal = 0;
    return 0;
  }

  while (p != NULL)
  {
    *buf_prev++ = p;

    if ((p->dev < dev) || ((p->dev == dev) && (p->block < block)))
    {
      p->avl.cache = 1;
      q = p->avl.right;
      if (q == NULL)
      {
        q = node;
        p->avl.right = q = node;
        break;
      }
    }
    else if ((p->dev != dev) || (p->block != block))
    {
      p->avl.cache = -1;
      q = p->avl.left;
      if (q == NULL)
      {
        q = node;
        p->avl.left = q;
        break;
      }
    }
    else
    {
      return -1;
    }

    p = q;
  }

  q->avl.left = q->avl.right = NULL;
  q->avl.bal = 0;
  modified = true;
  buf_prev--;

  while (modified)
  {
    if (p->avl.cache == -1)
    {
      switch (p->avl.bal)
      {
        case 1:
          p->avl.bal = 0;
          modified = false;
          break;

        case 0:
          p->avl.bal = -1;
          break;

        case -1:
          p1 = p->avl.left;
          if (p1->avl.bal == -1) /* simple LL-turn */
          {
            p->avl.left = p1->avl.right;
            p1->avl.right = p;
            p->avl.bal = 0;
            p = p1;
          }
          else /* double LR-turn */
          {
            p2 = p1->avl.right;
            p1->avl.right = p2->avl.left;
            p2->avl.left = p1;
            p->avl.left = p2->avl.right;
            p2->avl.right = p;
            if (p2->avl.bal == -1) p->avl.bal = +1; else p->avl.bal = 0;
            if (p2->avl.bal == +1) p1->avl.bal = -1; else p1->avl.bal = 0;
            p = p2;
          }
          p->avl.bal = 0;
          modified = false;
          break;

        default:
          break;
      }
    }
    else
    {
      switch (p->avl.bal)
      {
        case -1:
          p->avl.bal = 0;
          modified = false;
          break;

        case 0:
          p->avl.bal = 1;
          break;

        case 1:
          p1 = p->avl.right;
          if (p1->avl.bal == 1) /* simple RR-turn */
          {
            p->avl.right = p1->avl.left;
            p1->avl.left = p;
            p->avl.bal = 0;
            p = p1;
          }
          else /* double RL-turn */
          {
            p2 = p1->avl.left;
            p1->avl.left = p2->avl.right;
            p2->avl.right = p1;
            p->avl.right = p2->avl.left;
            p2->avl.left = p;
            if (p2->avl.bal == +1) p->avl.bal = -1; else p->avl.bal = 0;
            if (p2->avl.bal == -1) p1->avl.bal = +1; else p1->avl.bal = 0;
            p = p2;
          }
          p->avl.bal = 0;
          modified = false;
          break;

        default:
          break;
      }
    }
    q = p;
    if (buf_prev > buf_stack)
    {
      p = *--buf_prev;

      if (p->avl.cache == -1)
      {
        p->avl.left = q;
      }
      else
      {
        p->avl.right = q;
      }
    }
    else
    {
      *root = p;
      break;
    }
  };

  return 0;
}


/**
 * Removes the node from the tree.
 *
 * @param root Pointer to pointer to the root node
 * @param node Pointer to the node to remove
 * @retval 0 Item removed
 * @retval -1 No such item found
 */
static int
rtems_bdbuf_avl_remove(rtems_bdbuf_buffer**      root,
                       const rtems_bdbuf_buffer* node)
{
  dev_t             dev = node->dev;
  rtems_blkdev_bnum block = node->block;

  rtems_bdbuf_buffer*  p = *root;
  rtems_bdbuf_buffer*  q;
  rtems_bdbuf_buffer*  r;
  rtems_bdbuf_buffer*  s;
  rtems_bdbuf_buffer*  p1;
  rtems_bdbuf_buffer*  p2;
  rtems_bdbuf_buffer*  buf_stack[RTEMS_BDBUF_AVL_MAX_HEIGHT];
  rtems_bdbuf_buffer** buf_prev = buf_stack;

  bool modified = false;

  memset (buf_stack, 0, sizeof(buf_stack));

  while (p != NULL)
  {
    *buf_prev++ = p;

    if ((p->dev < dev) || ((p->dev == dev) && (p->block < block)))
    {
      p->avl.cache = 1;
      p = p->avl.right;
    }
    else if ((p->dev != dev) || (p->block != block))
    {
      p->avl.cache = -1;
      p = p->avl.left;
    }
    else
    {
      /* node found */
      break;
    }
  }

  if (p == NULL)
  {
    /* there is no such node */
    return -1;
  }

  q = p;

  buf_prev--;
  if (buf_prev > buf_stack)
  {
    p = *(buf_prev - 1);
  }
  else
  {
    p = NULL;
  }

  /* at this moment q - is a node to delete, p is q's parent */
  if (q->avl.right == NULL)
  {
    r = q->avl.left;
    if (r != NULL)
    {
      r->avl.bal = 0;
    }
    q = r;
  }
  else
  {
    rtems_bdbuf_buffer **t;

    r = q->avl.right;

    if (r->avl.left == NULL)
    {
      r->avl.left = q->avl.left;
      r->avl.bal = q->avl.bal;
      r->avl.cache = 1;
      *buf_prev++ = q = r;
    }
    else
    {
      t = buf_prev++;
      s = r;

      while (s->avl.left != NULL)
      {
        *buf_prev++ = r = s;
        s = r->avl.left;
        r->avl.cache = -1;
      }

      s->avl.left = q->avl.left;
      r->avl.left = s->avl.right;
      s->avl.right = q->avl.right;
      s->avl.bal = q->avl.bal;
      s->avl.cache = 1;

      *t = q = s;
    }
  }

  if (p != NULL)
  {
    if (p->avl.cache == -1)
    {
      p->avl.left = q;
    }
    else
    {
      p->avl.right = q;
    }
  }
  else
  {
    *root = q;
  }

  modified = true;

  while (modified)
  {
    if (buf_prev > buf_stack)
    {
      p = *--buf_prev;
    }
    else
    {
      break;
    }

    if (p->avl.cache == -1)
    {
      /* rebalance left branch */
      switch (p->avl.bal)
      {
        case -1:
          p->avl.bal = 0;
          break;
        case  0:
          p->avl.bal = 1;
          modified = false;
          break;

        case +1:
          p1 = p->avl.right;

          if (p1->avl.bal >= 0) /* simple RR-turn */
          {
            p->avl.right = p1->avl.left;
            p1->avl.left = p;

            if (p1->avl.bal == 0)
            {
              p1->avl.bal = -1;
              modified = false;
            }
            else
            {
              p->avl.bal = 0;
              p1->avl.bal = 0;
            }
            p = p1;
          }
          else /* double RL-turn */
          {
            p2 = p1->avl.left;

            p1->avl.left = p2->avl.right;
            p2->avl.right = p1;
            p->avl.right = p2->avl.left;
            p2->avl.left = p;

            if (p2->avl.bal == +1) p->avl.bal = -1; else p->avl.bal = 0;
            if (p2->avl.bal == -1) p1->avl.bal = 1; else p1->avl.bal = 0;

            p = p2;
            p2->avl.bal = 0;
          }
          break;

        default:
          break;
      }
    }
    else
    {
      /* rebalance right branch */
      switch (p->avl.bal)
      {
        case +1:
          p->avl.bal = 0;
          break;

        case  0:
          p->avl.bal = -1;
          modified = false;
          break;

        case -1:
          p1 = p->avl.left;

          if (p1->avl.bal <= 0) /* simple LL-turn */
          {
            p->avl.left = p1->avl.right;
            p1->avl.right = p;
            if (p1->avl.bal == 0)
            {
              p1->avl.bal = 1;
              modified = false;
            }
            else
            {
              p->avl.bal = 0;
              p1->avl.bal = 0;
            }
            p = p1;
          }
          else /* double LR-turn */
          {
            p2 = p1->avl.right;

            p1->avl.right = p2->avl.left;
            p2->avl.left = p1;
            p->avl.left = p2->avl.right;
            p2->avl.right = p;

            if (p2->avl.bal == -1) p->avl.bal = 1; else p->avl.bal = 0;
            if (p2->avl.bal == +1) p1->avl.bal = -1; else p1->avl.bal = 0;

            p = p2;
            p2->avl.bal = 0;
          }
          break;

        default:
          break;
      }
    }

    if (buf_prev > buf_stack)
    {
      q = *(buf_prev - 1);

      if (q->avl.cache == -1)
      {
        q->avl.left = p;
      }
      else
      {
        q->avl.right = p;
      }
    }
    else
    {
      *root = p;
      break;
    }

  }

  return 0;
}

/**
 * Lock the mutex. A single task can nest calls.
 *
 * @param lock The mutex to lock.
 * @param fatal_error_code The error code if the call fails.
 */
static void
rtems_bdbuf_lock (rtems_id lock, uint32_t fatal_error_code)
{
  rtems_status_code sc = rtems_semaphore_obtain (lock,
                                                 RTEMS_WAIT,
                                                 RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (fatal_error_code);
}

/**
 * Unlock the mutex.
 *
 * @param lock The mutex to unlock.
 * @param fatal_error_code The error code if the call fails.
 */
static void
rtems_bdbuf_unlock (rtems_id lock, uint32_t fatal_error_code)
{
  rtems_status_code sc = rtems_semaphore_release (lock);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (fatal_error_code);
}

/**
 * Lock the cache. A single task can nest calls.
 */
static void
rtems_bdbuf_lock_cache (void)
{
  rtems_bdbuf_lock (bdbuf_cache.lock, RTEMS_BLKDEV_FATAL_BDBUF_CACHE_LOCK);
}

/**
 * Unlock the cache.
 */
static void
rtems_bdbuf_unlock_cache (void)
{
  rtems_bdbuf_unlock (bdbuf_cache.lock, RTEMS_BLKDEV_FATAL_BDBUF_CACHE_UNLOCK);
}

/**
 * Lock the cache's sync. A single task can nest calls.
 */
static void
rtems_bdbuf_lock_sync (void)
{
  rtems_bdbuf_lock (bdbuf_cache.sync_lock, RTEMS_BLKDEV_FATAL_BDBUF_SYNC_LOCK);
}

/**
 * Unlock the cache's sync lock. Any blocked writers are woken.
 */
static void
rtems_bdbuf_unlock_sync (void)
{
  rtems_bdbuf_unlock (bdbuf_cache.sync_lock,
                      RTEMS_BLKDEV_FATAL_BDBUF_SYNC_UNLOCK);
}

/**
 * Wait until woken. Semaphores are used so a number of tasks can wait and can
 * be woken at once. Task events would require we maintain a list of tasks to
 * be woken and this would require storgage and we do not know the number of
 * tasks that could be waiting.
 *
 * While we have the cache locked we can try and claim the semaphore and
 * therefore know when we release the lock to the cache we will block until the
 * semaphore is released. This may even happen before we get to block.
 *
 * A counter is used to save the release call when no one is waiting.
 *
 * The function assumes the cache is locked on entry and it will be locked on
 * exit.
 *
 * @param sema The semaphore to block on and wait.
 * @param waiters The wait counter for this semaphore.
 */
static void
rtems_bdbuf_wait (rtems_id* sema, volatile uint32_t* waiters)
{
  rtems_status_code sc;
  rtems_mode        prev_mode;
  
  /*
   * Indicate we are waiting.
   */
  *waiters += 1;

  /*
   * Disable preemption then unlock the cache and block.  There is no POSIX
   * condition variable in the core API so this is a work around.
   *
   * The issue is a task could preempt after the cache is unlocked because it is
   * blocking or just hits that window, and before this task has blocked on the
   * semaphore. If the preempting task flushes the queue this task will not see
   * the flush and may block for ever or until another transaction flushes this
   * semaphore.
   */
  sc = rtems_task_mode (RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &prev_mode);

  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_1);
  
  /*
   * Unlock the cache, wait, and lock the cache when we return.
   */
  rtems_bdbuf_unlock_cache ();

  sc = rtems_semaphore_obtain (*sema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  
  if (sc != RTEMS_UNSATISFIED)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_2);
  
  rtems_bdbuf_lock_cache ();

  sc = rtems_task_mode (prev_mode, RTEMS_ALL_MODE_MASKS, &prev_mode);

  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_3);
  
  *waiters -= 1;
}

/**
 * Wake a blocked resource. The resource has a counter that lets us know if
 * there are any waiters.
 *
 * @param sema The semaphore to release.
 * @param waiters The wait counter for this semaphore.
 */
static void
rtems_bdbuf_wake (rtems_id sema, volatile uint32_t* waiters)
{
  if (*waiters)
  {
    rtems_status_code sc;

    sc = rtems_semaphore_flush (sema);
  
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAKE);
  }
}

/**
 * Add a buffer descriptor to the modified list. This modified list is treated
 * a litte differently to the other lists. To access it you must have the cache
 * locked and this is assumed to be the case on entry to this call.
 *
 * If the cache has a device being sync'ed and the bd is for that device the
 * call must block and wait until the sync is over before adding the bd to the
 * modified list. Once a sync happens for a device no bd's can be added the
 * modified list. The disk image is forced to be snapshot at that moment in
 * time.
 *
 * and you must
 * hold the sync lock. The sync lock is used to block writes while a sync is
 * active.
 *
 * @param bd The bd to queue to the cache's modified list.
 */
static void
rtems_bdbuf_append_modified (rtems_bdbuf_buffer* bd)
{
  /*
   * If the cache has a device being sync'ed check if this bd is for that
   * device. If it is unlock the cache and block on the sync lock. Once we have
   * the sync lock release it.
   */
  if (bdbuf_cache.sync_active && (bdbuf_cache.sync_device == bd->dev))
  {
    rtems_bdbuf_unlock_cache ();
    /* Wait for the sync lock */
    rtems_bdbuf_lock_sync ();
    rtems_bdbuf_unlock_sync ();
    rtems_bdbuf_lock_cache ();
  }
      
  bd->state = RTEMS_BDBUF_STATE_MODIFIED;

  rtems_chain_append (&bdbuf_cache.modified, &bd->link);
}

/**
 * Wait the swapper task.
 */
static void
rtems_bdbuf_wake_swapper (void)
{
  rtems_status_code sc = rtems_event_send (bdbuf_cache.swapout,
                                           RTEMS_BDBUF_SWAPOUT_SYNC);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_WAKE);
}

/**
 * Compute the number of BDs per group for a given buffer size.
 *
 * @param size The buffer size. It can be any size and we scale up.
 */
static size_t
rtems_bdbuf_bds_per_group (size_t size)
{
  size_t bufs_per_size;
  size_t bds_per_size;
  
  if (size > rtems_bdbuf_configuration.buffer_max)
    return 0;
  
  bufs_per_size = ((size - 1) / bdbuf_config.buffer_min) + 1;
  
  for (bds_per_size = 1;
       bds_per_size < bufs_per_size;
       bds_per_size <<= 1)
    ;

  return bdbuf_cache.max_bds_per_group / bds_per_size;
}

/**
 * Reallocate a group. The BDs currently allocated in the group are removed
 * from the ALV tree and any lists then the new BD's are prepended to the ready
 * list of the cache.
 *
 * @param group The group to reallocate.
 * @param new_bds_per_group The new count of BDs per group.
 */
static void
rtems_bdbuf_group_realloc (rtems_bdbuf_group* group, size_t new_bds_per_group)
{
  rtems_bdbuf_buffer* bd;
  int                 b;
  size_t              bufs_per_bd;

  bufs_per_bd = bdbuf_cache.max_bds_per_group / group->bds_per_group;
  
  for (b = 0, bd = group->bdbuf;
       b < group->bds_per_group;
       b++, bd += bufs_per_bd)
  {
    if ((bd->state == RTEMS_BDBUF_STATE_CACHED) ||
        (bd->state == RTEMS_BDBUF_STATE_MODIFIED) ||
        (bd->state == RTEMS_BDBUF_STATE_READ_AHEAD))
    {
      if (rtems_bdbuf_avl_remove (&bdbuf_cache.tree, bd) != 0)
        rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);
      rtems_chain_extract (&bd->link);
    }
  }
  
  group->bds_per_group = new_bds_per_group;
  bufs_per_bd = bdbuf_cache.max_bds_per_group / new_bds_per_group;
  
  for (b = 0, bd = group->bdbuf;
       b < group->bds_per_group;
       b++, bd += bufs_per_bd)
    rtems_chain_prepend (&bdbuf_cache.ready, &bd->link);
}

/**
 * Get the next BD from the list. This call assumes the cache is locked.
 *
 * @param bds_per_group The number of BDs per block we are need.
 * @param list The list to find the BD on.
 * @return The next BD if found or NULL is none are available.
 */
static rtems_bdbuf_buffer*
rtems_bdbuf_get_next_bd (size_t               bds_per_group,
                         rtems_chain_control* list)
{
  rtems_chain_node* node = rtems_chain_first (list);
  while (!rtems_chain_is_tail (list, node))
  {
    rtems_bdbuf_buffer* bd = (rtems_bdbuf_buffer*) node;

    /*
     * If this bd is already part of a group that supports the same number of
     * BDs per group return it. If the bd is part of another group check the
     * number of users and if 0 we can take this group and resize it.
     */
    if (bd->group->bds_per_group == bds_per_group)
    {
      rtems_chain_extract (node);
      bd->group->users++;
      return bd;
    }

    if (bd->group->users == 0)
    {
      /*
       * We use the group to locate the start of the BDs for this group.
       */
      rtems_bdbuf_group_realloc (bd->group, bds_per_group);
      bd = (rtems_bdbuf_buffer*) rtems_chain_get (&bdbuf_cache.ready);
      return bd;
    }

    node = rtems_chain_next (node);
  }
  
  return NULL;
}

/**
 * Initialise the cache.
 *
 * @return rtems_status_code The initialisation status.
 */
rtems_status_code
rtems_bdbuf_init (void)
{
  rtems_bdbuf_group*  group;
  rtems_bdbuf_buffer* bd;
  uint8_t*            buffer;
  int                 b;
  int                 cache_aligment;
  rtems_status_code   sc;

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("init\n");
#endif

  /*
   * Check the configuration table values.
   */
  if ((bdbuf_config.buffer_max % bdbuf_config.buffer_min) != 0)
    return RTEMS_INVALID_NUMBER;
  
  /*
   * We use a special variable to manage the initialisation incase we have
   * completing threads doing this. You may get errors if the another thread
   * makes a call and we have not finished initialisation.
   */
  if (bdbuf_cache.initialised)
    return RTEMS_RESOURCE_IN_USE;

  bdbuf_cache.initialised = true;
  
  /*
   * For unspecified cache alignments we use the CPU alignment.
   */
  cache_aligment = 32; /* FIXME rtems_cache_get_data_line_size() */
  if (cache_aligment <= 0)
    cache_aligment = CPU_ALIGNMENT;

  bdbuf_cache.sync_active    = false;
  bdbuf_cache.sync_device    = -1;
  bdbuf_cache.sync_requester = 0;
  bdbuf_cache.tree           = NULL;

  rtems_chain_initialize_empty (&bdbuf_cache.swapout_workers);
  rtems_chain_initialize_empty (&bdbuf_cache.ready);
  rtems_chain_initialize_empty (&bdbuf_cache.lru);
  rtems_chain_initialize_empty (&bdbuf_cache.modified);
  rtems_chain_initialize_empty (&bdbuf_cache.sync);

  bdbuf_cache.access           = 0;
  bdbuf_cache.access_waiters   = 0;
  bdbuf_cache.transfer         = 0;
  bdbuf_cache.transfer_waiters = 0;
  bdbuf_cache.waiting          = 0;
  bdbuf_cache.wait_waiters     = 0;

  /*
   * Create the locks for the cache.
   */
  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 'l'),
                               1, RTEMS_BDBUF_CACHE_LOCK_ATTRIBS, 0,
                               &bdbuf_cache.lock);
  if (sc != RTEMS_SUCCESSFUL)
  {
    bdbuf_cache.initialised = false;
    return sc;
  }

  rtems_bdbuf_lock_cache ();
  
  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 's'),
                               1, RTEMS_BDBUF_CACHE_LOCK_ATTRIBS, 0,
                               &bdbuf_cache.sync_lock);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return sc;
  }
  
  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 'a'),
                               0, RTEMS_BDBUF_CACHE_WAITER_ATTRIBS, 0,
                               &bdbuf_cache.access);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_semaphore_delete (bdbuf_cache.sync_lock);
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return sc;
  }

  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 't'),
                               0, RTEMS_BDBUF_CACHE_WAITER_ATTRIBS, 0,
                               &bdbuf_cache.transfer);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_semaphore_delete (bdbuf_cache.access);
    rtems_semaphore_delete (bdbuf_cache.sync_lock);
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return sc;
  }

  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 'w'),
                               0, RTEMS_BDBUF_CACHE_WAITER_ATTRIBS, 0,
                               &bdbuf_cache.waiting);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_semaphore_delete (bdbuf_cache.transfer);
    rtems_semaphore_delete (bdbuf_cache.access);
    rtems_semaphore_delete (bdbuf_cache.sync_lock);
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return sc;
  }
  
  /*
   * Allocate the memory for the buffer descriptors.
   */
  bdbuf_cache.bds = calloc (sizeof (rtems_bdbuf_buffer),
                            bdbuf_config.size / bdbuf_config.buffer_min);
  if (!bdbuf_cache.bds)
  {
    rtems_semaphore_delete (bdbuf_cache.transfer);
    rtems_semaphore_delete (bdbuf_cache.access);
    rtems_semaphore_delete (bdbuf_cache.sync_lock);
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return RTEMS_NO_MEMORY;
  }

  /*
   * Compute the various number of elements in the cache.
   */
  bdbuf_cache.buffer_min_count =
    bdbuf_config.size / bdbuf_config.buffer_min;
  bdbuf_cache.max_bds_per_group =
    bdbuf_config.buffer_max / bdbuf_config.buffer_min;
  bdbuf_cache.group_count =
    bdbuf_cache.buffer_min_count / bdbuf_cache.max_bds_per_group;

  /*
   * Allocate the memory for the buffer descriptors.
   */
  bdbuf_cache.groups = calloc (sizeof (rtems_bdbuf_group),
                               bdbuf_cache.group_count);
  if (!bdbuf_cache.groups)
  {
    free (bdbuf_cache.bds);
    rtems_semaphore_delete (bdbuf_cache.transfer);
    rtems_semaphore_delete (bdbuf_cache.access);
    rtems_semaphore_delete (bdbuf_cache.sync_lock);
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return RTEMS_NO_MEMORY;
  }
  
  /*
   * Allocate memory for buffer memory. The buffer memory will be cache
   * aligned. It is possible to free the memory allocated by rtems_memalign()
   * with free(). Return 0 if allocated.
   */
  if (rtems_memalign ((void **) &bdbuf_cache.buffers,
                      cache_aligment,
                      bdbuf_cache.buffer_min_count * bdbuf_config.buffer_min) != 0)
  {
    free (bdbuf_cache.groups);
    free (bdbuf_cache.bds);
    rtems_semaphore_delete (bdbuf_cache.transfer);
    rtems_semaphore_delete (bdbuf_cache.access);
    rtems_semaphore_delete (bdbuf_cache.sync_lock);
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return RTEMS_NO_MEMORY;
  }

  /*
   * The cache is empty after opening so we need to add all the buffers to it
   * and initialise the groups.
   */
  for (b = 0, group = bdbuf_cache.groups,
         bd = bdbuf_cache.bds, buffer = bdbuf_cache.buffers;
       b < bdbuf_cache.buffer_min_count;
       b++, bd++, buffer += bdbuf_config.buffer_min)
  {
    bd->dev        = -1;
    bd->group      = group;
    bd->buffer     = buffer;
    bd->avl.left   = NULL;
    bd->avl.right  = NULL;
    bd->state      = RTEMS_BDBUF_STATE_EMPTY;
    bd->error      = 0;
    bd->waiters    = 0;
    bd->hold_timer = 0;
    
    rtems_chain_append (&bdbuf_cache.ready, &bd->link);

    if ((b % bdbuf_cache.max_bds_per_group) ==
        (bdbuf_cache.max_bds_per_group - 1))
      group++;
  }

  for (b = 0,
         group = bdbuf_cache.groups,
         bd = bdbuf_cache.bds;
       b < bdbuf_cache.group_count;
       b++,
         group++,
         bd += bdbuf_cache.max_bds_per_group)
  {
    group->bds_per_group = bdbuf_cache.max_bds_per_group;
    group->users = 0;
    group->bdbuf = bd;
  }
         
  /*
   * Create and start swapout task. This task will create and manage the worker
   * threads.
   */
  bdbuf_cache.swapout_enabled = true;
  
  sc = rtems_task_create (rtems_build_name('B', 'S', 'W', 'P'),
                          (bdbuf_config.swapout_priority ?
                           bdbuf_config.swapout_priority :
                           RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT),
                          SWAPOUT_TASK_STACK_SIZE,
                          RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR,
                          RTEMS_LOCAL | RTEMS_NO_FLOATING_POINT,
                          &bdbuf_cache.swapout);
  if (sc != RTEMS_SUCCESSFUL)
  {
    free (bdbuf_cache.buffers);
    free (bdbuf_cache.groups);
    free (bdbuf_cache.bds);
    rtems_semaphore_delete (bdbuf_cache.transfer);
    rtems_semaphore_delete (bdbuf_cache.access);
    rtems_semaphore_delete (bdbuf_cache.sync_lock);
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return sc;
  }

  sc = rtems_task_start (bdbuf_cache.swapout,
                         rtems_bdbuf_swapout_task,
                         (rtems_task_argument) &bdbuf_cache);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_task_delete (bdbuf_cache.swapout);
    free (bdbuf_cache.buffers);
    free (bdbuf_cache.groups);
    free (bdbuf_cache.bds);
    rtems_semaphore_delete (bdbuf_cache.transfer);
    rtems_semaphore_delete (bdbuf_cache.access);
    rtems_semaphore_delete (bdbuf_cache.sync_lock);
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
    bdbuf_cache.initialised = false;
    return sc;
  }

  rtems_bdbuf_unlock_cache ();
  
  return RTEMS_SUCCESSFUL;
}

/**
 * Get a buffer for this device and block. This function returns a buffer once
 * placed into the AVL tree. If no buffer is available and it is not a read
 * ahead request and no buffers are waiting to the written to disk wait until a
 * buffer is available. If buffers are waiting to be written to disk and none
 * are available expire the hold timer's of the queued buffers and wake the
 * swap out task. If the buffer is for a read ahead transfer return NULL if
 * there are no buffers available or the buffer is already in the cache.
 *
 * The AVL tree of buffers for the cache is searched and if not found obtain a
 * buffer and insert it into the AVL tree. Buffers are first obtained from the
 * ready list until all empty/ready buffers are used. Once all buffers are in
 * use the LRU list is searched for a buffer of the same group size or a group
 * that has no active buffers in use. A buffer taken from the LRU list is
 * removed from the AVL tree and assigned the new block number. The ready or
 * LRU list buffer is initialised to this device and block. If no buffers are
 * available due to the ready and LRU lists being empty a check is made of the
 * modified list. Buffers may be queued waiting for the hold timer to
 * expire. These buffers should be written to disk and returned to the LRU list
 * where they can be used. If buffers are on the modified list the max. write
 * block size of buffers have their hold timer's expired and the swap out task
 * woken. The caller then blocks on the waiting semaphore and counter. When
 * buffers return from the upper layers (access) or lower driver (transfer) the
 * blocked caller task is woken and this procedure is repeated. The repeat
 * handles a case of a another thread pre-empting getting a buffer first and
 * adding it to the AVL tree.
 *
 * A buffer located in the AVL tree means it is already in the cache and maybe
 * in use somewhere. The buffer can be either:
 *
 * # Cached. Not being accessed or part of a media transfer.
 * # Access or modifed access. Is with an upper layer being accessed.
 * # Transfer. Is with the driver and part of a media transfer.
 *
 * If cached we assign the new state, extract it from any list it maybe part of
 * and return to the user.
 *
 * This function assumes the cache the buffer is being taken from is locked and
 * it will make sure the cache is locked when it returns. The cache will be
 * unlocked if the call could block.
 *
 * Variable sized buffer is handled by groups. A group is the size of the
 * maximum buffer that can be allocated. The group can size in multiples of the
 * minimum buffer size where the mulitples are 1,2,4,8, etc. If the buffer is
 * found in the AVL tree the number of BDs in the group is check and if
 * different the buffer size for the block has changed. The buffer needs to be
 * invalidated.
 *
 * @param dd The disk device. Has the configured block size.
 * @param bds_per_group The number of BDs in a group for this block.
 * @param block Absolute media block number for the device
 * @param read_ahead The get is for a read ahead buffer if true
 * @return RTEMS status code (if operation completed successfully or error
 *         code if error is occured)
 */
static rtems_bdbuf_buffer*
rtems_bdbuf_get_buffer (rtems_disk_device* dd,
                        size_t             bds_per_group,
                        rtems_blkdev_bnum  block,
                        bool               read_ahead)
{
  dev_t               device = dd->dev;
  rtems_bdbuf_buffer* bd;
  bool                available;
  
  /*
   * Loop until we get a buffer. Under load we could find no buffers are
   * available requiring this task to wait until some become available before
   * proceeding. There is no timeout. If this call is to block and the buffer
   * is for a read ahead buffer return NULL. The read ahead is nice but not
   * that important.
   *
   * The search procedure is repeated as another thread could have pre-empted
   * us while we waited for a buffer, obtained an empty buffer and loaded the
   * AVL tree with the one we are after. In this case we move down and wait for
   * the buffer to return to the cache.
   */
  do
  {
    /*
     * Search for buffer descriptor for this dev/block key.
     */
    bd = rtems_bdbuf_avl_search (&bdbuf_cache.tree, device, block);

    /*
     * No buffer in the cache for this block. We need to obtain a buffer and
     * this means take a buffer that is ready to use. If all buffers are in use
     * take the least recently used buffer. If there are none then the cache is
     * empty. All the buffers are either queued to be written to disk or with
     * the user. We cannot do much with the buffers with the user how-ever with
     * the modified buffers waiting to be written to disk flush the maximum
     * number transfered in a block to disk. After this all that can be done is
     * to wait for a buffer to return to the cache.
     */
    if (!bd)
    {
      /*
       * Assign new buffer descriptor from the ready list if one is present. If
       * the ready queue is empty get the oldest buffer from LRU list. If the
       * LRU list is empty there are no available buffers check the modified
       * list.
       */
      bd = rtems_bdbuf_get_next_bd (bds_per_group, &bdbuf_cache.ready);

      if (!bd)
      {
        /*
         * No unused or read-ahead buffers.
         *
         * If this is a read ahead buffer just return. No need to place further
         * pressure on the cache by reading something that may be needed when
         * we have data in the cache that was needed and may still be in the
         * future.
         */
        if (read_ahead)
          return NULL;

        /*
         * Check the LRU list.
         */
        bd = rtems_bdbuf_get_next_bd (bds_per_group, &bdbuf_cache.lru);
        
        if (bd)
        {
          /*
           * Remove the buffer from the AVL tree.
           */
          if (rtems_bdbuf_avl_remove (&bdbuf_cache.tree, bd) != 0)
            rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);
        }
        else
        {
          /*
           * If there are buffers on the modified list expire the hold timer
           * and wake the swap out task then wait else just go and wait.
           *
           * The check for an empty list is made so the swapper is only woken
           * when if timers are changed.
           */
          if (!rtems_chain_is_empty (&bdbuf_cache.modified))
          {
            rtems_chain_node* node = rtems_chain_first (&bdbuf_cache.modified);
            uint32_t          write_blocks = 0;
            
            while ((write_blocks < bdbuf_config.max_write_blocks) &&
                   !rtems_chain_is_tail (&bdbuf_cache.modified, node))
            {
              rtems_bdbuf_buffer* bd = (rtems_bdbuf_buffer*) node;
              bd->hold_timer = 0;
              write_blocks++;
              node = rtems_chain_next (node);
            }

            rtems_bdbuf_wake_swapper ();
          }
          
          /*
           * Wait for a buffer to be returned to the cache. The buffer will be
           * placed on the LRU list.
           */
          rtems_bdbuf_wait (&bdbuf_cache.waiting, &bdbuf_cache.wait_waiters);
        }
      }
      else
      {
        /*
         * We have a new buffer for this block.
         */
        if ((bd->state != RTEMS_BDBUF_STATE_EMPTY) &&
            (bd->state != RTEMS_BDBUF_STATE_READ_AHEAD))
          rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);

        if (bd->state == RTEMS_BDBUF_STATE_READ_AHEAD)
        {
          if (rtems_bdbuf_avl_remove (&bdbuf_cache.tree, bd) != 0)
            rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);
        }
      }

      if (bd)
      {
        bd->dev       = device;
        bd->block     = block;
        bd->avl.left  = NULL;
        bd->avl.right = NULL;
        bd->state     = RTEMS_BDBUF_STATE_EMPTY;
        bd->error     = 0;
        bd->waiters   = 0;

        if (rtems_bdbuf_avl_insert (&bdbuf_cache.tree, bd) != 0)
          rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);

        return bd;
      }
    }
    else
    {
      /*
       * We have the buffer for the block from the cache. Check if the buffer
       * in the cache is the same size and the requested size we are after.
       */
      if (bd->group->bds_per_group != bds_per_group)
      {
        bd->state = RTEMS_BDBUF_STATE_EMPTY;
        rtems_chain_extract (&bd->link);
        rtems_chain_prepend (&bdbuf_cache.ready, &bd->link);
        bd = NULL;
      }
    }
  }
  while (!bd);

  /*
   * If the buffer is for read ahead and it exists in the AVL cache or is being
   * accessed or being transfered then return NULL.
   */
  if (read_ahead)
    return NULL;

  /*
   * Loop waiting for the buffer to enter the cached state. If the buffer is in
   * the access or transfer state then wait until it is not.
   */
  available = false;
  while (!available)
  {
    switch (bd->state)
    {
      case RTEMS_BDBUF_STATE_CACHED:
      case RTEMS_BDBUF_STATE_MODIFIED:
      case RTEMS_BDBUF_STATE_READ_AHEAD:
        available = true;
        break;

      case RTEMS_BDBUF_STATE_ACCESS:
      case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
        bd->waiters++;
        rtems_bdbuf_wait (&bdbuf_cache.access,
                          &bdbuf_cache.access_waiters);
        bd->waiters--;
        break;

      case RTEMS_BDBUF_STATE_SYNC:
      case RTEMS_BDBUF_STATE_TRANSFER:
        bd->waiters++;
        rtems_bdbuf_wait (&bdbuf_cache.transfer,
                          &bdbuf_cache.transfer_waiters);
        bd->waiters--;
        break;

      default:
        rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);
    }
  }

  /*
   * Buffer is linked to the LRU, modifed, or sync lists. Remove it from there.
   */
  rtems_chain_extract (&bd->link);

  return bd;
}

rtems_status_code
rtems_bdbuf_get (dev_t                device,
                 rtems_blkdev_bnum    block,
                 rtems_bdbuf_buffer** bdp)
{
  rtems_disk_device*  dd;
  rtems_bdbuf_buffer* bd;
  size_t              bds_per_group;

  if (!bdbuf_cache.initialised)
    return RTEMS_NOT_CONFIGURED;

  /*
   * Do not hold the cache lock when obtaining the disk table.
   */
  dd = rtems_disk_obtain (device);
  if (!dd)
    return RTEMS_INVALID_ID;

  if (block >= dd->size)
  {
    rtems_disk_release (dd);
    return RTEMS_INVALID_ADDRESS;
  }

  bds_per_group = rtems_bdbuf_bds_per_group (dd->block_size);
  if (!bds_per_group)
  {
    rtems_disk_release (dd);
    return RTEMS_INVALID_NUMBER;
  }
  
  rtems_bdbuf_lock_cache ();

#if RTEMS_BDBUF_TRACE
  /* Print the block index relative to the physical disk */
  rtems_bdbuf_printf ("get: %d (dev = %08x)\n", block + dd->start, device);
#endif

  bd = rtems_bdbuf_get_buffer (dd, bds_per_group, block + dd->start, false);

  if (bd->state == RTEMS_BDBUF_STATE_MODIFIED)
    bd->state = RTEMS_BDBUF_STATE_ACCESS_MODIFIED;
  else
    bd->state = RTEMS_BDBUF_STATE_ACCESS;

  rtems_bdbuf_unlock_cache ();

  rtems_disk_release(dd);

  *bdp = bd;

  return RTEMS_SUCCESSFUL;
}

/**
 * Call back handler called by the low level driver when the transfer has
 * completed. This function may be invoked from interrupt handler.
 *
 * @param arg Arbitrary argument specified in block device request
 *            structure (in this case - pointer to the appropriate
 *            block device request structure).
 * @param status I/O completion status
 * @param error errno error code if status != RTEMS_SUCCESSFUL
 */
static void
rtems_bdbuf_read_done (void* arg, rtems_status_code status, int error)
{
  rtems_blkdev_request* req = (rtems_blkdev_request*) arg;

  req->error = error;
  req->status = status;

  rtems_event_send (req->io_task, RTEMS_BDBUF_TRANSFER_SYNC);
}

rtems_status_code
rtems_bdbuf_read (dev_t                device,
                  rtems_blkdev_bnum    block,
                  rtems_bdbuf_buffer** bdp)
{
  rtems_disk_device*    dd;
  rtems_bdbuf_buffer*   bd = NULL;
  uint32_t              read_ahead_count;
  rtems_blkdev_request* req;
  size_t                bds_per_group;
  
  if (!bdbuf_cache.initialised)
    return RTEMS_NOT_CONFIGURED;

  /*
   * @todo This type of request structure is wrong and should be removed.
   */
#define bdbuf_alloc(size) __builtin_alloca (size)

  req = bdbuf_alloc (sizeof (rtems_blkdev_request) +
                     (sizeof ( rtems_blkdev_sg_buffer) *
                      rtems_bdbuf_configuration.max_read_ahead_blocks));

  /*
   * Do not hold the cache lock when obtaining the disk table.
   */
  dd = rtems_disk_obtain (device);
  if (!dd)
    return RTEMS_INVALID_ID;
  
  if (block >= dd->size) {
    rtems_disk_release(dd);
    return RTEMS_INVALID_NUMBER;
  }
  
  bds_per_group = rtems_bdbuf_bds_per_group (dd->block_size);
  if (!bds_per_group)
  {
    rtems_disk_release (dd);
    return RTEMS_INVALID_NUMBER;
  }
  
#if RTEMS_BDBUF_TRACE
  /* Print the block index relative to the physical disk */
  rtems_bdbuf_printf ("read: %d (dev = %08x)\n", block + dd->start, device);
#endif

  req->bufnum = 0;

  /*
   * Read the block plus the required number of blocks ahead. The number of
   * blocks to read ahead is configured by the user and limited by the size of
   * the disk or reaching a read ahead block that is also cached.
   *
   * Limit the blocks read by the size of the disk.
   */
  if ((rtems_bdbuf_configuration.max_read_ahead_blocks + block) < dd->size)
    read_ahead_count = rtems_bdbuf_configuration.max_read_ahead_blocks;
  else
    read_ahead_count = dd->size - block;

  rtems_bdbuf_lock_cache ();

  while (req->bufnum < read_ahead_count)
  {
    /*
     * Get the buffer for the requested block. If the block is cached then
     * return it. If it is not cached transfer the block from the disk media
     * into memory.
     *
     * We need to clean up any buffers allocated and not passed back to the
     * caller.
     */
    bd = rtems_bdbuf_get_buffer (dd, bds_per_group,
                                 block + dd->start + req->bufnum,
                                 req->bufnum == 0 ? false : true);

    /*
     * Read ahead buffer is in the cache or none available. Read what we
     * can.
     */
    if (!bd)
      break;

    /*
     * Is the block we are interested in the cache ?
     */
    if ((bd->state == RTEMS_BDBUF_STATE_CACHED) ||
        (bd->state == RTEMS_BDBUF_STATE_MODIFIED))
      break;

    bd->state = RTEMS_BDBUF_STATE_TRANSFER;
    bd->error = 0;

    /*
     * @todo The use of these req blocks is not a great design. The req is a
     *       struct with a single 'bufs' declared in the req struct and the
     *       others are added in the outer level struct. This relies on the
     *       structs joining as a single array and that assumes the compiler
     *       packs the structs. Why not just place on a list ? The BD has a
     *       node that can be used.
     */
    req->bufs[req->bufnum].user   = bd;
    req->bufs[req->bufnum].block  = bd->block;
    req->bufs[req->bufnum].length = dd->block_size;
    req->bufs[req->bufnum].buffer = bd->buffer;
    req->bufnum++;
  }

  /*
   * Transfer any requested buffers. If the request count is 0 we have found
   * the block in the cache so return it.
   */
  if (req->bufnum)
  {
    /*
     * Unlock the cache. We have the buffer for the block and it will be in the
     * access or transfer state. We may also have a number of read ahead blocks
     * if we need to transfer data. At this point any other threads can gain
     * access to the cache and if they are after any of the buffers we have
     * they will block and be woken when the buffer is returned to the cache.
     *
     * If a transfer is needed the I/O operation will occur with pre-emption
     * enabled and the cache unlocked. This is a change to the previous version
     * of the bdbuf code.
     */
    rtems_event_set out;
    int             result;
    uint32_t        b;

    /*
     * Flush any events.
     */
    rtems_event_receive (RTEMS_BDBUF_TRANSFER_SYNC,
                         RTEMS_EVENT_ALL | RTEMS_NO_WAIT,
                         0, &out);
                         
    rtems_bdbuf_unlock_cache ();

    req->req = RTEMS_BLKDEV_REQ_READ;
    req->req_done = rtems_bdbuf_read_done;
    req->done_arg = req;
    req->io_task = rtems_task_self ();
    req->status = RTEMS_RESOURCE_IN_USE;
    req->error = 0;
  
    result = dd->ioctl (dd->phys_dev->dev, RTEMS_BLKIO_REQUEST, req);

    /*
     * Inspection of the DOS FS code shows the result from this function is
     * handled and a buffer must be returned.
     */
    if (result < 0)
    {
      req->error = errno;
      req->status = RTEMS_IO_ERROR;
    }
    else
    {
      rtems_status_code sc;
      
      sc = rtems_event_receive (RTEMS_BDBUF_TRANSFER_SYNC,
                                RTEMS_EVENT_ALL | RTEMS_WAIT,
                                0, &out);

      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (BLKDEV_FATAL_BDBUF_SWAPOUT_RE);
    }

    rtems_bdbuf_lock_cache ();

    for (b = 1; b < req->bufnum; b++)
    {
      bd = req->bufs[b].user;
      bd->error = req->error;
      bd->state = RTEMS_BDBUF_STATE_READ_AHEAD;
      rtems_bdbuf_release (bd);
    }

    bd = req->bufs[0].user;
  }

  /*
   * The data for this block is cached in the buffer.
   */
  if (bd->state == RTEMS_BDBUF_STATE_MODIFIED)
    bd->state = RTEMS_BDBUF_STATE_ACCESS_MODIFIED;
  else
    bd->state = RTEMS_BDBUF_STATE_ACCESS;

  rtems_bdbuf_unlock_cache ();
  rtems_disk_release (dd);

  *bdp = bd;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_bdbuf_release (rtems_bdbuf_buffer* bd)
{
  if (!bdbuf_cache.initialised)
    return RTEMS_NOT_CONFIGURED;

  if (bd == NULL)
    return RTEMS_INVALID_ADDRESS;

  rtems_bdbuf_lock_cache ();

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("release: %d\n", bd->block);
#endif
  
  if (bd->state == RTEMS_BDBUF_STATE_ACCESS_MODIFIED)
  {
    rtems_bdbuf_append_modified (bd);
  }
  else
  {
    /*
     * If this is a read ahead buffer place the ready queue. Buffers are taken
     * from here first. If we prepend then get from the queue the buffers
     * furthermost from the read buffer will be used.
     */
    if (bd->state == RTEMS_BDBUF_STATE_READ_AHEAD)
      rtems_chain_prepend (&bdbuf_cache.ready, &bd->link);
    else
    {
      bd->state = RTEMS_BDBUF_STATE_CACHED;
      rtems_chain_append (&bdbuf_cache.lru, &bd->link);
    }

    /*
     * One less user for the group of bds.
     */
    bd->group->users--;
  }
  
  /*
   * If there are threads waiting to access the buffer wake them. Wake any
   * waiters if this is the first buffer to placed back onto the queue.
   */
  if (bd->waiters)
    rtems_bdbuf_wake (bdbuf_cache.access, &bdbuf_cache.access_waiters);
  else
  {
    if (bd->state == RTEMS_BDBUF_STATE_READ_AHEAD)
    {
      if (rtems_chain_has_only_one_node (&bdbuf_cache.ready))
        rtems_bdbuf_wake (bdbuf_cache.waiting, &bdbuf_cache.wait_waiters);
    }
    else
    {
      if (rtems_chain_has_only_one_node (&bdbuf_cache.lru))
        rtems_bdbuf_wake (bdbuf_cache.waiting, &bdbuf_cache.wait_waiters);
    }
  }
  
  rtems_bdbuf_unlock_cache ();

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_bdbuf_release_modified (rtems_bdbuf_buffer* bd)
{
  if (!bdbuf_cache.initialised)
    return RTEMS_NOT_CONFIGURED;

  if (!bd)
    return RTEMS_INVALID_ADDRESS;

  rtems_bdbuf_lock_cache ();

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("release modified: %d\n", bd->block);
#endif

  bd->hold_timer = rtems_bdbuf_configuration.swap_block_hold;
  
  rtems_bdbuf_append_modified (bd);

  if (bd->waiters)
    rtems_bdbuf_wake (bdbuf_cache.access, &bdbuf_cache.access_waiters);
  
  rtems_bdbuf_unlock_cache ();

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_bdbuf_sync (rtems_bdbuf_buffer* bd)
{
  bool available;

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("sync: %d\n", bd->block);
#endif
  
  if (!bdbuf_cache.initialised)
    return RTEMS_NOT_CONFIGURED;

  if (!bd)
    return RTEMS_INVALID_ADDRESS;

  rtems_bdbuf_lock_cache ();

  bd->state = RTEMS_BDBUF_STATE_SYNC;

  rtems_chain_append (&bdbuf_cache.sync, &bd->link);

  rtems_bdbuf_wake_swapper ();

  available = false;
  while (!available)
  {
    switch (bd->state)
    {
      case RTEMS_BDBUF_STATE_CACHED:
      case RTEMS_BDBUF_STATE_READ_AHEAD:
      case RTEMS_BDBUF_STATE_MODIFIED:
      case RTEMS_BDBUF_STATE_ACCESS:
      case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
        available = true;
        break;

      case RTEMS_BDBUF_STATE_SYNC:
      case RTEMS_BDBUF_STATE_TRANSFER:
        bd->waiters++;
        rtems_bdbuf_wait (&bdbuf_cache.transfer, &bdbuf_cache.transfer_waiters);
        bd->waiters--;
        break;

      default:
        rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);
    }
  }

  rtems_bdbuf_unlock_cache ();
  
  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_bdbuf_syncdev (dev_t dev)
{
  rtems_disk_device*  dd;
  rtems_status_code   sc;
  rtems_event_set     out;

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("syncdev: %08x\n", dev);
#endif

  if (!bdbuf_cache.initialised)
    return RTEMS_NOT_CONFIGURED;

  /*
   * Do not hold the cache lock when obtaining the disk table.
   */
  dd = rtems_disk_obtain (dev);
  if (!dd)
    return RTEMS_INVALID_ID;

  /*
   * Take the sync lock before locking the cache. Once we have the sync lock we
   * can lock the cache. If another thread has the sync lock it will cause this
   * thread to block until it owns the sync lock then it can own the cache. The
   * sync lock can only be obtained with the cache unlocked.
   */
  
  rtems_bdbuf_lock_sync ();
  rtems_bdbuf_lock_cache ();  

  /*
   * Set the cache to have a sync active for a specific device and let the swap
   * out task know the id of the requester to wake when done.
   *
   * The swap out task will negate the sync active flag when no more buffers
   * for the device are held on the "modified for sync" queues.
   */
  bdbuf_cache.sync_active    = true;
  bdbuf_cache.sync_requester = rtems_task_self ();
  bdbuf_cache.sync_device    = dev;
  
  rtems_bdbuf_wake_swapper ();
  rtems_bdbuf_unlock_cache ();
  
  sc = rtems_event_receive (RTEMS_BDBUF_TRANSFER_SYNC,
                            RTEMS_EVENT_ALL | RTEMS_WAIT,
                            0, &out);

  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (BLKDEV_FATAL_BDBUF_SWAPOUT_RE);
      
  rtems_bdbuf_unlock_sync ();
  
  return rtems_disk_release (dd);
}

/**
 * Call back handler called by the low level driver when the transfer has
 * completed. This function may be invoked from interrupt handler.
 *
 * @param arg Arbitrary argument specified in block device request
 *            structure (in this case - pointer to the appropriate
 *            block device request structure).
 * @param status I/O completion status
 * @param error errno error code if status != RTEMS_SUCCESSFUL
 */
static void
rtems_bdbuf_write_done(void *arg, rtems_status_code status, int error)
{
  rtems_blkdev_request* req = (rtems_blkdev_request*) arg;

  req->error = error;
  req->status = status;

  rtems_event_send (req->io_task, RTEMS_BDBUF_TRANSFER_SYNC);
}

/**
 * Swapout transfer to the driver. The driver will break this I/O into groups
 * of consecutive write requests is multiple consecutive buffers are required
 * by the driver.
 *
 * @param transfer The transfer transaction.
 */
static void
rtems_bdbuf_swapout_write (rtems_bdbuf_swapout_transfer* transfer)
{
  rtems_disk_device*  dd;
  
#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("swapout transfer: %08x\n", transfer->dev);
#endif

  /*
   * If there are buffers to transfer to the media transfer them.
   */
  if (!rtems_chain_is_empty (&transfer->bds))
  {
    /*
     * Obtain the disk device. The cache's mutex has been released to avoid a
     * dead lock.
     */
    dd = rtems_disk_obtain (transfer->dev);
    if (dd)
    {
      /*
       * The last block number used when the driver only supports
       * continuous blocks in a single request.
       */
      uint32_t last_block = 0;
      
      /*
       * Take as many buffers as configured and pass to the driver. Note, the
       * API to the drivers has an array of buffers and if a chain was passed
       * we could have just passed the list. If the driver API is updated it
       * should be possible to make this change with little effect in this
       * code. The array that is passed is broken in design and should be
       * removed. Merging members of a struct into the first member is
       * trouble waiting to happen.
       */
      transfer->write_req->status = RTEMS_RESOURCE_IN_USE;
      transfer->write_req->error = 0;
      transfer->write_req->bufnum = 0;

      while (!rtems_chain_is_empty (&transfer->bds))
      {
        rtems_bdbuf_buffer* bd =
          (rtems_bdbuf_buffer*) rtems_chain_get (&transfer->bds);

        bool write = false;
        
        /*
         * If the device only accepts sequential buffers and this is not the
         * first buffer (the first is always sequential, and the buffer is not
         * sequential then put the buffer back on the transfer chain and write
         * the committed buffers.
         */
        
#if RTEMS_BDBUF_TRACE
        rtems_bdbuf_printf ("swapout write: bd:%d, bufnum:%d mode:%s\n",
                            bd->block, transfer->write_req->bufnum,
                            dd->phys_dev->capabilities &
                            RTEMS_BLKDEV_CAP_MULTISECTOR_CONT ? "MULIT" : "SCAT");
#endif

        if ((dd->phys_dev->capabilities & RTEMS_BLKDEV_CAP_MULTISECTOR_CONT) &&
            transfer->write_req->bufnum &&
            (bd->block != (last_block + 1)))
        {
          rtems_chain_prepend (&transfer->bds, &bd->link);
          write = true;
        }
        else
        {
          rtems_blkdev_sg_buffer* buf;
          buf = &transfer->write_req->bufs[transfer->write_req->bufnum];
          transfer->write_req->bufnum++;
          buf->user   = bd;
          buf->block  = bd->block;
          buf->length = dd->block_size;
          buf->buffer = bd->buffer;
          last_block  = bd->block;
        }

        /*
         * Perform the transfer if there are no more buffers, or the transfer
         * size has reached the configured max. value.
         */

        if (rtems_chain_is_empty (&transfer->bds) ||
            (transfer->write_req->bufnum >= rtems_bdbuf_configuration.max_write_blocks))
          write = true;

        if (write)
        {
          int result;
          uint32_t b;

#if RTEMS_BDBUF_TRACE
          rtems_bdbuf_printf ("swapout write: writing bufnum:%d\n",
                              transfer->write_req->bufnum);
#endif
          /*
           * Perform the transfer. No cache locks, no preemption, only the disk
           * device is being held.
           */
          result = dd->phys_dev->ioctl (dd->phys_dev->dev,
                                        RTEMS_BLKIO_REQUEST, transfer->write_req);

          if (result < 0)
          {
            rtems_bdbuf_lock_cache ();
              
            for (b = 0; b < transfer->write_req->bufnum; b++)
            {
              bd = transfer->write_req->bufs[b].user;
              bd->state  = RTEMS_BDBUF_STATE_MODIFIED;
              bd->error = errno;

              /*
               * Place back on the cache's modified queue and try again.
               *
               * @warning Not sure this is the best option but I do not know
               *          what else can be done.
               */
              rtems_chain_append (&bdbuf_cache.modified, &bd->link);
            }
          }
          else
          {
            rtems_status_code sc = 0;
            rtems_event_set   out;

            sc = rtems_event_receive (RTEMS_BDBUF_TRANSFER_SYNC,
                                      RTEMS_EVENT_ALL | RTEMS_WAIT,
                                      0, &out);

            if (sc != RTEMS_SUCCESSFUL)
              rtems_fatal_error_occurred (BLKDEV_FATAL_BDBUF_SWAPOUT_RE);

            rtems_bdbuf_lock_cache ();

            for (b = 0; b < transfer->write_req->bufnum; b++)
            {
              bd = transfer->write_req->bufs[b].user;
              bd->state = RTEMS_BDBUF_STATE_CACHED;
              bd->error = 0;
              bd->group->users--;
              
              rtems_chain_append (&bdbuf_cache.lru, &bd->link);
              
              if (bd->waiters)
                rtems_bdbuf_wake (bdbuf_cache.transfer, &bdbuf_cache.transfer_waiters);
              else
              {
                if (rtems_chain_has_only_one_node (&bdbuf_cache.lru))
                  rtems_bdbuf_wake (bdbuf_cache.waiting, &bdbuf_cache.wait_waiters);
              }
            }
          }
              
          rtems_bdbuf_unlock_cache ();

          transfer->write_req->status = RTEMS_RESOURCE_IN_USE;
          transfer->write_req->error = 0;
          transfer->write_req->bufnum = 0;
        }
      }
          
      rtems_disk_release (dd);
    }
    else
    {
      /*
       * We have buffers but no device. Put the BDs back onto the
       * ready queue and exit.
       */
      /* @todo fixme */
    }
  }
}

/**
 * Process the modified list of buffers. There is a sync or modified list that
 * needs to be handled so we have a common function to do the work.
 *
 * @param dev The device to handle. If -1 no device is selected so select the
 *            device of the first buffer to be written to disk.
 * @param chain The modified chain to process.
 * @param transfer The chain to append buffers to be written too.
 * @param sync_active If true this is a sync operation so expire all timers.
 * @param update_timers If true update the timers.
 * @param timer_delta It update_timers is true update the timers by this
 *                    amount.
 */
static void
rtems_bdbuf_swapout_modified_processing (dev_t*               dev,
                                         rtems_chain_control* chain,
                                         rtems_chain_control* transfer,
                                         bool                 sync_active,
                                         bool                 update_timers,
                                         uint32_t             timer_delta)
{
  if (!rtems_chain_is_empty (chain))
  {
    rtems_chain_node* node = rtems_chain_head (chain);
    node = node->next;

    while (!rtems_chain_is_tail (chain, node))
    {
      rtems_bdbuf_buffer* bd = (rtems_bdbuf_buffer*) node;
    
      /*
       * Check if the buffer's hold timer has reached 0. If a sync is active
       * force all the timers to 0.
       *
       * @note Lots of sync requests will skew this timer. It should be based
       *       on TOD to be accurate. Does it matter ?
       */
      if (sync_active)
        bd->hold_timer = 0;
  
      if (bd->hold_timer)
      {
        if (update_timers)
        {
          if (bd->hold_timer > timer_delta)
            bd->hold_timer -= timer_delta;
          else
            bd->hold_timer = 0;
        }

        if (bd->hold_timer)
        {
          node = node->next;
          continue;
        }
      }

      /*
       * This assumes we can set dev_t to -1 which is just an
       * assumption. Cannot use the transfer list being empty the sync dev
       * calls sets the dev to use.
       */
      if (*dev == (dev_t)-1)
        *dev = bd->dev;

      if (bd->dev == *dev)
      {
        rtems_chain_node* next_node = node->next;
        rtems_chain_node* tnode = rtems_chain_tail (transfer);
    
        /*
         * The blocks on the transfer list are sorted in block order. This
         * means multi-block transfers for drivers that require consecutive
         * blocks perform better with sorted blocks and for real disks it may
         * help lower head movement.
         */

        bd->state = RTEMS_BDBUF_STATE_TRANSFER;

        rtems_chain_extract (node);

        tnode = tnode->previous;
          
        while (node && !rtems_chain_is_head (transfer, tnode))
        {
          rtems_bdbuf_buffer* tbd = (rtems_bdbuf_buffer*) tnode;

          if (bd->block > tbd->block)
          {
            rtems_chain_insert (tnode, node);
            node = NULL;
          }
          else
            tnode = tnode->previous;
        }
        
        if (node)
          rtems_chain_prepend (transfer, node);
          
        node = next_node;
      }
      else
      {
        node = node->next;
      }
    }
  }
}

/**
 * Process the cache's modified buffers. Check the sync list first then the
 * modified list extracting the buffers suitable to be written to disk. We have
 * a device at a time. The task level loop will repeat this operation while
 * there are buffers to be written. If the transfer fails place the buffers
 * back on the modified list and try again later. The cache is unlocked while
 * the buffers are being written to disk.
 *
 * @param timer_delta It update_timers is true update the timers by this
 *                    amount.
 * @param update_timers If true update the timers.
 * @param transfer The transfer transaction data.
 *
 * @retval true Buffers where written to disk so scan again.
 * @retval false No buffers where written to disk.
 */
static bool
rtems_bdbuf_swapout_processing (unsigned long                 timer_delta,
                                bool                          update_timers,
                                rtems_bdbuf_swapout_transfer* transfer)
{
  rtems_bdbuf_swapout_worker* worker;
  bool                        transfered_buffers = false;

  rtems_bdbuf_lock_cache ();

  /*
   * If a sync is active do not use a worker because the current code does not
   * cleaning up after. We need to know the buffers have been written when
   * syncing to release sync lock and currently worker threads do not return to
   * here. We do not know the worker is the last in a sequence of sync writes
   * until after we have it running so we do not know to tell it to release the
   * lock. The simplest solution is to get the main swap out task perform all
   * sync operations.
   */
  if (bdbuf_cache.sync_active)
    worker = NULL;
  else
  {
    worker = (rtems_bdbuf_swapout_worker*)
      rtems_chain_get (&bdbuf_cache.swapout_workers);
    if (worker)
      transfer = &worker->transfer;
  }
  
  rtems_chain_initialize_empty (&transfer->bds);
  transfer->dev = -1;
  
  /*
   * When the sync is for a device limit the sync to that device. If the sync
   * is for a buffer handle process the devices in the order on the sync
   * list. This means the dev is -1.
   */
  if (bdbuf_cache.sync_active)
    transfer->dev = bdbuf_cache.sync_device;
  
  /*
   * If we have any buffers in the sync queue move them to the modified
   * list. The first sync buffer will select the device we use.
   */
  rtems_bdbuf_swapout_modified_processing (&transfer->dev,
                                           &bdbuf_cache.sync,
                                           &transfer->bds,
                                           true, false,
                                           timer_delta);

  /*
   * Process the cache's modified list.
   */
  rtems_bdbuf_swapout_modified_processing (&transfer->dev,
                                           &bdbuf_cache.modified,
                                           &transfer->bds,
                                           bdbuf_cache.sync_active,
                                           update_timers,
                                           timer_delta);

  /*
   * We have all the buffers that have been modified for this device so the
   * cache can be unlocked because the state of each buffer has been set to
   * TRANSFER.
   */
  rtems_bdbuf_unlock_cache ();

  /*
   * If there are buffers to transfer to the media transfer them.
   */
  if (!rtems_chain_is_empty (&transfer->bds))
  {
    if (worker)
    {
      rtems_status_code sc = rtems_event_send (worker->id,
                                               RTEMS_BDBUF_SWAPOUT_SYNC);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_WAKE);
    }
    else
    {
      rtems_bdbuf_swapout_write (transfer);
    }
    
    transfered_buffers = true;
  }
   
  if (bdbuf_cache.sync_active && !transfered_buffers)
  {
    rtems_id sync_requester;
    rtems_bdbuf_lock_cache ();
    sync_requester = bdbuf_cache.sync_requester;
    bdbuf_cache.sync_active = false;
    bdbuf_cache.sync_requester = 0;
    rtems_bdbuf_unlock_cache ();
    if (sync_requester)
      rtems_event_send (sync_requester, RTEMS_BDBUF_TRANSFER_SYNC);
  }
  
  return transfered_buffers;
}

/**
 * Allocate the write request and initialise it for good measure.
 *
 * @return rtems_blkdev_request* The write reference memory.
 */
static rtems_blkdev_request*
rtems_bdbuf_swapout_writereq_alloc (void)
{
  /*
   * @note chrisj The rtems_blkdev_request and the array at the end is a hack.
   * I am disappointment at finding code like this in RTEMS. The request should
   * have been a rtems_chain_control. Simple, fast and less storage as the node
   * is already part of the buffer structure.
   */
  rtems_blkdev_request* write_req =
    malloc (sizeof (rtems_blkdev_request) +
            (rtems_bdbuf_configuration.max_write_blocks *
             sizeof (rtems_blkdev_sg_buffer)));

  if (!write_req)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_NOMEM);

  write_req->req = RTEMS_BLKDEV_REQ_WRITE;
  write_req->req_done = rtems_bdbuf_write_done;
  write_req->done_arg = write_req;
  write_req->io_task = rtems_task_self ();

  return write_req;
}

/**
 * The swapout worker thread body.
 *
 * @param arg A pointer to the worker thread's private data.
 * @return rtems_task Not used.
 */
static rtems_task
rtems_bdbuf_swapout_worker_task (rtems_task_argument arg)
{
  rtems_bdbuf_swapout_worker* worker = (rtems_bdbuf_swapout_worker*) arg;

  while (worker->enabled)
  {
    rtems_event_set   out;
    rtems_status_code sc;
    
    sc = rtems_event_receive (RTEMS_BDBUF_SWAPOUT_SYNC,
                              RTEMS_EVENT_ALL | RTEMS_WAIT,
                              RTEMS_NO_TIMEOUT,
                              &out);

    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (BLKDEV_FATAL_BDBUF_SWAPOUT_RE);

    rtems_bdbuf_swapout_write (&worker->transfer);

    rtems_bdbuf_lock_cache ();

    rtems_chain_initialize_empty (&worker->transfer.bds);
    worker->transfer.dev = -1;

    rtems_chain_append (&bdbuf_cache.swapout_workers, &worker->link);
    
    rtems_bdbuf_unlock_cache ();
  }

  free (worker->transfer.write_req);
  free (worker);

  rtems_task_delete (RTEMS_SELF);
}

/**
 * Open the swapout worker threads.
 */
static void
rtems_bdbuf_swapout_workers_open (void)
{
  rtems_status_code sc;
  int               w;
  
  rtems_bdbuf_lock_cache ();
  
  for (w = 0; w < rtems_bdbuf_configuration.swapout_workers; w++)
  {
    rtems_bdbuf_swapout_worker* worker;

    worker = malloc (sizeof (rtems_bdbuf_swapout_worker));
    if (!worker)
      rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_NOMEM);

    rtems_chain_append (&bdbuf_cache.swapout_workers, &worker->link);
    worker->enabled = true;
    worker->transfer.write_req = rtems_bdbuf_swapout_writereq_alloc ();
    
    rtems_chain_initialize_empty (&worker->transfer.bds);
    worker->transfer.dev = -1;

    sc = rtems_task_create (rtems_build_name('B', 'D', 'o', 'a' + w),
                            (rtems_bdbuf_configuration.swapout_priority ?
                             rtems_bdbuf_configuration.swapout_priority :
                             RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT),
                            SWAPOUT_TASK_STACK_SIZE,
                            RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR,
                            RTEMS_LOCAL | RTEMS_NO_FLOATING_POINT,
                            &worker->id);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_WK_CREATE);

    sc = rtems_task_start (worker->id,
                           rtems_bdbuf_swapout_worker_task,
                           (rtems_task_argument) worker);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_WK_START);
  }
  
  rtems_bdbuf_unlock_cache ();
}

/**
 * Close the swapout worker threads.
 */
static void
rtems_bdbuf_swapout_workers_close (void)
{
  rtems_chain_node* node;
  
  rtems_bdbuf_lock_cache ();
  
  node = rtems_chain_first (&bdbuf_cache.swapout_workers);
  while (!rtems_chain_is_tail (&bdbuf_cache.swapout_workers, node))
  {
    rtems_bdbuf_swapout_worker* worker = (rtems_bdbuf_swapout_worker*) node;
    worker->enabled = false;
    rtems_event_send (worker->id, RTEMS_BDBUF_SWAPOUT_SYNC);
    node = rtems_chain_next (node);
  }
  
  rtems_bdbuf_unlock_cache ();
}

/**
 * Body of task which takes care on flushing modified buffers to the disk.
 *
 * @param arg A pointer to the global cache data. Use the global variable and
 *            not this.
 * @return rtems_task Not used.
 */
static rtems_task
rtems_bdbuf_swapout_task (rtems_task_argument arg)
{
  rtems_bdbuf_swapout_transfer transfer;
  uint32_t                     period_in_ticks;
  const uint32_t               period_in_msecs = bdbuf_config.swapout_period;;
  uint32_t                     timer_delta;

  transfer.write_req = rtems_bdbuf_swapout_writereq_alloc ();
  rtems_chain_initialize_empty (&transfer.bds);
  transfer.dev = -1;

  /*
   * Localise the period.
   */
  period_in_ticks = RTEMS_MICROSECONDS_TO_TICKS (period_in_msecs * 1000);

  /*
   * This is temporary. Needs to be changed to use the real time clock.
   */
  timer_delta = period_in_msecs;

  /*
   * Create the worker threads.
   */
  rtems_bdbuf_swapout_workers_open ();
  
  while (bdbuf_cache.swapout_enabled)
  {
    rtems_event_set   out;
    rtems_status_code sc;

    /*
     * Only update the timers once in the processing cycle.
     */
    bool update_timers = true;
    
    /*
     * If we write buffers to any disk perform a check again. We only write a
     * single device at a time and the cache may have more than one device's
     * buffers modified waiting to be written.
     */
    bool transfered_buffers;

    do
    {
      transfered_buffers = false;

      /*
       * Extact all the buffers we find for a specific device. The device is
       * the first one we find on a modified list. Process the sync queue of
       * buffers first.
       */
      if (rtems_bdbuf_swapout_processing (timer_delta,
                                          update_timers,
                                          &transfer))
      {
        transfered_buffers = true;
      }
      
      /*
       * Only update the timers once.
       */
      update_timers = false;
    }
    while (transfered_buffers);

    sc = rtems_event_receive (RTEMS_BDBUF_SWAPOUT_SYNC,
                              RTEMS_EVENT_ALL | RTEMS_WAIT,
                              period_in_ticks,
                              &out);

    if ((sc != RTEMS_SUCCESSFUL) && (sc != RTEMS_TIMEOUT))
      rtems_fatal_error_occurred (BLKDEV_FATAL_BDBUF_SWAPOUT_RE);
  }

  rtems_bdbuf_swapout_workers_close ();
  
  free (transfer.write_req);

  rtems_task_delete (RTEMS_SELF);
}

