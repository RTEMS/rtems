/*
 * Disk I/O buffering
 * Buffer managment
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Peterburg, Russia
 * Author: Andrey G. Ivanov <Andrey.Ivanov@oktet.ru>
 *         Victor V. Vengerov <vvv@oktet.ru>
 *         Alexander Kukuta <kam@oktet.ru>
 *
 * Copyright (C) 2008 Chris Johns <chrisj@rtems.org>
 *    Rewritten to remove score mutex access. Fixes many performance
 *    issues.
 * 
 * @(#) bdbuf.c,v 1.14 2004/04/17 08:15:17 ralf Exp
 */

/**
 * @file
 *
 * The Buffer Descriptor Buffer code implements a cache between the disk
 * devices and file systems. The code provides read ahead and write queuing to
 * the drivers and fast cache look up using an AVL tree.
 *
 * The buffers are held in pools based on size. Each pool has buffers and the
 * buffers follow this state machine:
 *                                  
 *                   read/read ahead
 *          +-------------------------------+
 *          |                               v
 *     +-----------+ read ahead      +------------+
 *     | READY,    |  complete       |            |---------+
 *     |  READ     |<----------------|  TRANSFER  |         |
 *     |   AHEAD   |   +-------------|            |<--+     |
 *     +-----------+   | read/write  +------------+   |     |
 *              | get  v complete                swap |     |
 *              |    +-----------+ modified  +------------+ |
 *              +--->| ACCESSED, |---------->|  MODIFIED, | |
 *                   | ACCESSED  |<----------|  SYNC      | |
 *              +----|  MODIFIED |<--+   get |            | |
 *              |    +-----------+   |       +------------+ |
 *              | release        get |                      |
 *              |    +-----------+   |                      |
 *              +--->|           |---+        read complete |
 *                   |   CACHED  |           write complete |
 *                   |           |<-------------------------+
 *                   +-----------+
 *         
 * Empty buffers are added to the ready list and removed from this queue when a
 * caller requests a buffer. This is referred to as getting a buffer in the
 * code and the event get in the state diagram. The buffer is assigned to a
 * block and inserted to the AVL based on the block/device key. If the block is
 * to be read by the user and not in the cache (ready) it is transfered from
 * the disk into memory. If no ready buffers exist the buffer is taken from the
 * LRU list. If no buffers are on the LRU list the modified list is check. If
 * no buffers are on the modified list the request blocks. If buffers are on
 * the modified list the buffers hold timer is expired and the swap out task
 * woken.
 *
 * A block being accessed is given to the file system layer and not accessable
 * to another requester until released back to the cache. The same goes to a
 * buffer in the transfer state. The transfer state means being read or
 * written. If the file system has modifed the block and releases it as
 * modified it placed on the pool's modified list and a hold timer
 * initialised. The buffer is held for the hold time before being written to
 * disk. Buffers are held for a configurable period of time on the modified
 * list as a write sets the state to transfer and this locks the buffer out
 * from the file system until the write complete. Buffers are often repeatable
 * accessed and modified in a series of small updates so if sent to the disk
 * when released as modified the user would have to block waiting until it had
 * been written. This would be a performance problem.
 *
 * The code performs mulitple block reads and writes. Multiple block reads or
 * read ahead increases performance with hardware that supports it. It also
 * helps with a large cache as the disk head movement is reduced. It how-ever
 * is a speculative operation so excessive use can remove valuable and needed
 * blocks from the cache. The get call knows if a read is a for the file system
 * or if it is a read ahead get. If the get is for a read ahead block and the
 * block is already in the cache or no ready buffers are available the read
 * ahead is stopped. The transfer occurs with the blocks so far. If a buffer is
 * in the read ahead state and release it is placed on the ready list rather
 * than the LRU list. This means these buffers are used before buffers used by
 * the file system.
 *
 * The pool have the following lists of buffers:
 *
 *   ready        - Empty buffers created when the pool is initialised.
 *   modified     - Buffers waiting to be written to disk.
 *   sync         - Buffers to be synced to disk.
 *   lru          - Accessed buffers released in least recently used order.
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
#include <limits.h>
#include <errno.h>
#include <assert.h>

#if RTEMS_BDBUF_TRACE
#include <stdio.h>
#endif

#include "rtems/bdbuf.h"

/**
 * The BD buffer context.
 */
typedef struct rtems_bdbuf_context {
  rtems_bdbuf_pool* pool;      /*< Table of buffer pools */
  int               npools;    /*< Number of entries in pool table */
  rtems_id          swapout;   /*< Swapout task ID */
  bool              swapout_enabled;
} rtems_bdbuf_context;

/**
 * Fatal errors
 */
#define RTEMS_BLKDEV_FATAL_ERROR(n) \
  (((uint32_t)'B' << 24) | ((uint32_t)(n) & (uint32_t)0x00FFFFFF))

#define RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY RTEMS_BLKDEV_FATAL_ERROR(1)
#define RTEMS_BLKDEV_FATAL_BDBUF_SWAPOUT     RTEMS_BLKDEV_FATAL_ERROR(2)
#define RTEMS_BLKDEV_FATAL_BDBUF_SYNC_LOCK   RTEMS_BLKDEV_FATAL_ERROR(3)
#define RTEMS_BLKDEV_FATAL_BDBUF_SYNC_UNLOCK RTEMS_BLKDEV_FATAL_ERROR(4)
#define RTEMS_BLKDEV_FATAL_BDBUF_POOL_LOCK   RTEMS_BLKDEV_FATAL_ERROR(5)
#define RTEMS_BLKDEV_FATAL_BDBUF_POOL_UNLOCK RTEMS_BLKDEV_FATAL_ERROR(6)
#define RTEMS_BLKDEV_FATAL_BDBUF_POOL_WAIT   RTEMS_BLKDEV_FATAL_ERROR(7)
#define RTEMS_BLKDEV_FATAL_BDBUF_POOL_WAKE   RTEMS_BLKDEV_FATAL_ERROR(8)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_WAKE     RTEMS_BLKDEV_FATAL_ERROR(9)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_NOMEM    RTEMS_BLKDEV_FATAL_ERROR(10)
#define BLKDEV_FATAL_BDBUF_SWAPOUT_RE        RTEMS_BLKDEV_FATAL_ERROR(11)
#define BLKDEV_FATAL_BDBUF_SWAPOUT_TS        RTEMS_BLKDEV_FATAL_ERROR(12)

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
#define RTEMS_BDBUF_POOL_LOCK_ATTRIBS \
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
#define RTEMS_BDBUF_POOL_WAITER_ATTRIBS \
  (RTEMS_PRIORITY | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
   RTEMS_NO_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

/*
 * The swap out task.
 */
static rtems_task rtems_bdbuf_swapout_task(rtems_task_argument arg);

/**
 * The context of buffering layer.
 */
static rtems_bdbuf_context rtems_bdbuf_ctx;

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
 * @param root_addr Pointer to pointer to the root node
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
 * Get the pool for the device.
 *
 * @param pdd Physical disk device.
 */
static rtems_bdbuf_pool*
rtems_bdbuf_get_pool (const rtems_bdpool_id pid)
{
  return &rtems_bdbuf_ctx.pool[pid];
}

/**
 * Lock the pool. A single task can nest calls.
 *
 * @param pool The pool to lock.
 */
static void
rtems_bdbuf_lock_pool (rtems_bdbuf_pool* pool)
{
  rtems_status_code sc = rtems_semaphore_obtain (pool->lock,
                                                 RTEMS_WAIT,
                                                 RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_POOL_LOCK);
}

/**
 * Unlock the pool.
 *
 * @param pool The pool to unlock.
 */
static void
rtems_bdbuf_unlock_pool (rtems_bdbuf_pool* pool)
{
  rtems_status_code sc = rtems_semaphore_release (pool->lock);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_POOL_UNLOCK);
}

/**
 * Lock the pool's sync. A single task can nest calls.
 *
 * @param pool The pool's sync to lock.
 */
static void
rtems_bdbuf_lock_sync (rtems_bdbuf_pool* pool)
{
  rtems_status_code sc = rtems_semaphore_obtain (pool->sync_lock,
                                                 RTEMS_WAIT,
                                                 RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SYNC_LOCK);
}

/**
 * Unlock the pool's sync.
 *
 * @param pool The pool's sync to unlock.
 */
static void
rtems_bdbuf_unlock_sync (rtems_bdbuf_pool* pool)
{
  rtems_status_code sc = rtems_semaphore_release (pool->sync_lock);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SYNC_UNLOCK);
}

/**
 * Wait until woken. Semaphores are used so a number of tasks can wait and can
 * be woken at once. Task events would require we maintain a list of tasks to
 * be woken and this would require storgage and we do not know the number of
 * tasks that could be waiting.
 *
 * While we have the pool locked we can try and claim the semaphore and
 * therefore know when we release the lock to the pool we will block until the
 * semaphore is released. This may even happen before we get to block.
 *
 * A counter is used to save the release call when no one is waiting.
 *
 * The function assumes the pool is locked on entry and it will be locked on
 * exit.
 *
 * @param pool The pool to wait for a buffer to return.
 * @param sema The semaphore to block on and wait.
 * @param waiters The wait counter for this semaphore.
 */
static void
rtems_bdbuf_wait (rtems_bdbuf_pool* pool, rtems_id* sema,
                  volatile uint32_t* waiters)
{
  rtems_status_code sc;
  rtems_mode        prev_mode;
  
  /*
   * Indicate we are waiting.
   */
  *waiters += 1;

  /*
   * Disable preemption then unlock the pool and block.
   * There is no POSIX condition variable in the core API so
   * this is a work around.
   *
   * The issue is a task could preempt after the pool is unlocked
   * because it is blocking or just hits that window, and before
   * this task has blocked on the semaphore. If the preempting task
   * flushes the queue this task will not see the flush and may
   * block for ever or until another transaction flushes this
   * semaphore.
   */
  sc = rtems_task_mode (RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &prev_mode);

  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_POOL_WAIT);
  
  /*
   * Unlock the pool, wait, and lock the pool when we return.
   */
  rtems_bdbuf_unlock_pool (pool);

  sc = rtems_semaphore_obtain (*sema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  
  if (sc != RTEMS_UNSATISFIED)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_POOL_WAIT);
  
  rtems_bdbuf_lock_pool (pool);

  sc = rtems_task_mode (prev_mode, RTEMS_ALL_MODE_MASKS, &prev_mode);

  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_POOL_WAIT);
  
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
      rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_POOL_WAKE);
  }
}

/**
 * Add a buffer descriptor to the modified list. This modified list is treated
 * a litte differently to the other lists. To access it you must have the pool
 * locked and this is assumed to be the case on entry to this call.
 *
 * If the pool has a device being sync'ed and the bd is for that device the
 * call must block and wait until the sync is over before adding the bd to the
 * modified list. Once a sync happens for a device no bd's can be added the
 * modified list. The disk image is forced to be snapshot at that moment in
 * time.
 *
 * and you must
 * hold the sync lock. The sync lock is used to block writes while a sync is
 * active.
 *
 * @param pool The pool the bd belongs to.
 * @param bd The bd to queue to the pool's modified list.
 */
static void
rtems_bdbuf_append_modified (rtems_bdbuf_pool* pool, rtems_bdbuf_buffer* bd)
{
  /*
   * If the pool has a device being sync'ed check if this bd is for that
   * device. If it is unlock the pool and block on the sync lock. once we have
   * the sync lock reelase it.
   *
   * If the 
   */
  if (pool->sync_active && (pool->sync_device == bd->dev))
  {
    rtems_bdbuf_unlock_pool (pool);
    rtems_bdbuf_lock_sync (pool);
    rtems_bdbuf_unlock_sync (pool);
    rtems_bdbuf_lock_pool (pool);
  }
      
  bd->state = RTEMS_BDBUF_STATE_MODIFIED;

  rtems_chain_append (&pool->modified, &bd->link);
}

/**
 * Wait the swapper task.
 */
static void
rtems_bdbuf_wake_swapper (void)
{
  rtems_status_code sc = rtems_event_send (rtems_bdbuf_ctx.swapout,
                                           RTEMS_BDBUF_SWAPOUT_SYNC);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_WAKE);
}

/**
 * Initialize single buffer pool.
 *
 * @param config Buffer pool configuration
 * @param pid Pool number
 *
 * @return RTEMS_SUCCESSFUL, if buffer pool initialized successfully, or error
 *         code if error occured.
 */
static rtems_status_code
rtems_bdbuf_initialize_pool (rtems_bdbuf_pool_config* config,
                             rtems_bdpool_id          pid)
{
  unsigned char*      buffer = config->mem_area;
  rtems_bdbuf_pool*   pool;
  rtems_bdbuf_buffer* bd;
  rtems_status_code   sc;
  uint32_t            b;

  pool = rtems_bdbuf_get_pool (pid);
  
  pool->blksize        = config->size;
  pool->nblks          = config->num;
  pool->flags          = 0;
  pool->sync_active    = false;
  pool->sync_device    = -1;
  pool->sync_requester = 0;
  pool->tree           = NULL;
  pool->buffers        = NULL;

  rtems_chain_initialize_empty (&pool->ready);
  rtems_chain_initialize_empty (&pool->lru);
  rtems_chain_initialize_empty (&pool->modified);
  rtems_chain_initialize_empty (&pool->sync);

  pool->access           = 0;
  pool->access_waiters   = 0;
  pool->transfer         = 0;
  pool->transfer_waiters = 0;
  pool->waiting          = 0;
  pool->wait_waiters     = 0;
  
  /*
   * Allocate memory for buffer descriptors
   */
  pool->bds = calloc (config->num, sizeof (rtems_bdbuf_buffer));
  
  if (!pool->bds)
    return RTEMS_NO_MEMORY;

  /*
   * Allocate memory for buffers if required.
   */
  if (buffer == NULL)
  {
    buffer = pool->buffers = malloc (config->num * config->size);
    if (!pool->buffers)
    {
      free (pool->bds);
      return RTEMS_NO_MEMORY;
    }
  }

  for (b = 0, bd = pool->bds;
       b < pool->nblks;
       b++, bd++, buffer += pool->blksize)
  {
    bd->dev        = -1;
    bd->block      = 0;
    bd->buffer     = buffer;
    bd->avl.left   = NULL;
    bd->avl.right  = NULL;
    bd->state      = RTEMS_BDBUF_STATE_EMPTY;
    bd->pool       = pid;
    bd->error      = 0;
    bd->waiters    = 0;
    bd->hold_timer = 0;
    
    rtems_chain_append (&pool->ready, &bd->link);
  }

  sc = rtems_semaphore_create (rtems_build_name ('B', 'P', '0' + pid, 'L'),
                               1, RTEMS_BDBUF_POOL_LOCK_ATTRIBS, 0,
                               &pool->lock);
  if (sc != RTEMS_SUCCESSFUL)
  {
    free (pool->buffers);
    free (pool->bds);
    return sc;
  }

  sc = rtems_semaphore_create (rtems_build_name ('B', 'P', '0' + pid, 'S'),
                               1, RTEMS_BDBUF_POOL_LOCK_ATTRIBS, 0,
                               &pool->sync_lock);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_semaphore_delete (pool->lock);
    free (pool->buffers);
    free (pool->bds);
    return sc;
  }
  
  sc = rtems_semaphore_create (rtems_build_name ('B', 'P', '0' + pid, 'a'),
                               0, RTEMS_BDBUF_POOL_WAITER_ATTRIBS, 0,
                               &pool->access);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_semaphore_delete (pool->sync_lock);
    rtems_semaphore_delete (pool->lock);
    free (pool->buffers);
    free (pool->bds);
    return sc;
  }

  sc = rtems_semaphore_create (rtems_build_name ('B', 'P', '0' + pid, 't'),
                               0, RTEMS_BDBUF_POOL_WAITER_ATTRIBS, 0,
                               &pool->transfer);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_semaphore_delete (pool->access);
    rtems_semaphore_delete (pool->sync_lock);
    rtems_semaphore_delete (pool->lock);
    free (pool->buffers);
    free (pool->bds);
    return sc;
  }

  sc = rtems_semaphore_create (rtems_build_name ('B', 'P', '0' + pid, 'w'),
                               0, RTEMS_BDBUF_POOL_WAITER_ATTRIBS, 0,
                               &pool->waiting);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_semaphore_delete (pool->transfer);
    rtems_semaphore_delete (pool->access);
    rtems_semaphore_delete (pool->sync_lock);
    rtems_semaphore_delete (pool->lock);
    free (pool->buffers);
    free (pool->bds);
    return sc;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 * Free resources allocated for buffer pool with specified number.
 *
 * @param pid Buffer pool number
 *
 * @retval RTEMS_SUCCESSFUL
 */
static rtems_status_code
rtems_bdbuf_release_pool (rtems_bdpool_id pid)
{
  rtems_bdbuf_pool* pool = rtems_bdbuf_get_pool (pid);
  
  rtems_bdbuf_lock_pool (pool);

  rtems_semaphore_delete (pool->waiting);
  rtems_semaphore_delete (pool->transfer);
  rtems_semaphore_delete (pool->access);
  rtems_semaphore_delete (pool->lock);
  
  free (pool->buffers);
  free (pool->bds);
  
  return RTEMS_SUCCESSFUL;
}

/**
 * Prepare buffering layer to work - initialize buffer descritors and (if it is
 * neccessary) buffers. Buffers will be allocated accoriding to the
 * configuration table, each entry describes the size of block and the size of
 * the pool. After initialization all blocks is placed into the ready state.
 * lists.
 *
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_init (void)
{
  rtems_bdpool_id   p;
  rtems_status_code sc;

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("init\n");
#endif

  if (rtems_bdbuf_pool_configuration_size <= 0)
    return RTEMS_INVALID_SIZE;

  if (rtems_bdbuf_ctx.npools)
    return RTEMS_RESOURCE_IN_USE;

  rtems_bdbuf_ctx.npools = rtems_bdbuf_pool_configuration_size;

  /*
   * Allocate memory for buffer pool descriptors
   */
  rtems_bdbuf_ctx.pool = calloc (rtems_bdbuf_pool_configuration_size,
                                 sizeof (rtems_bdbuf_pool));
  
  if (rtems_bdbuf_ctx.pool == NULL)
    return RTEMS_NO_MEMORY;

  /*
   * Initialize buffer pools and roll out if something failed,
   */
  for (p = 0; p < rtems_bdbuf_ctx.npools; p++)
  {
    sc = rtems_bdbuf_initialize_pool (&rtems_bdbuf_pool_configuration[p], p);
    if (sc != RTEMS_SUCCESSFUL)
    {
      rtems_bdpool_id j;
      for (j = 0; j < p - 1; j++)
        rtems_bdbuf_release_pool (j);
      return sc;
    }
  }

  /*
   * Create and start swapout task
   */

  rtems_bdbuf_ctx.swapout_enabled = true;
  
  sc = rtems_task_create (rtems_build_name('B', 'S', 'W', 'P'),
                          (rtems_bdbuf_configuration.swapout_priority ?
                           rtems_bdbuf_configuration.swapout_priority :
                           RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT),
                          SWAPOUT_TASK_STACK_SIZE,
                          RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR,
                          RTEMS_LOCAL | RTEMS_NO_FLOATING_POINT,
                          &rtems_bdbuf_ctx.swapout);
  if (sc != RTEMS_SUCCESSFUL)
  {
    for (p = 0; p < rtems_bdbuf_ctx.npools; p++)
      rtems_bdbuf_release_pool (p);
    free (rtems_bdbuf_ctx.pool);
    return sc;
  }

  sc = rtems_task_start (rtems_bdbuf_ctx.swapout,
                         rtems_bdbuf_swapout_task,
                         (rtems_task_argument) &rtems_bdbuf_ctx);
  if (sc != RTEMS_SUCCESSFUL)
  {
    rtems_task_delete (rtems_bdbuf_ctx.swapout);
    for (p = 0; p < rtems_bdbuf_ctx.npools; p++)
      rtems_bdbuf_release_pool (p);
    free (rtems_bdbuf_ctx.pool);
    return sc;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 * Get a buffer for this device and block. This function returns a buffer once
 * placed into the AVL tree. If no buffer is available and it is not a read
 * ahead request and no buffers are waiting to the written to disk wait until
 * one is available. If buffers are waiting to be written to disk and non are
 * available expire the hold timer and wake the swap out task. If the buffer is
 * for a read ahead transfer return NULL if there is not buffer or it is in the
 * cache.
 *
 * The AVL tree of buffers for the pool is searched and if not located check
 * obtain a buffer and insert it into the AVL tree. Buffers are first obtained
 * from the ready list until all empty/ready buffers are used. Once all buffers
 * are in use buffers are taken from the LRU list with the least recently used
 * buffer taken first. A buffer taken from the LRU list is removed from the AVL
 * tree. The ready list or LRU list buffer is initialised to this device and
 * block. If no buffers are available due to the ready and LRU lists being
 * empty a check is made of the modified list. Buffers may be queued waiting
 * for the hold timer to expire. These buffers should be written to disk and
 * returned to the LRU list where they can be used rather than this call
 * blocking. If buffers are on the modified list the max. write block size of
 * buffers have their hold timer expired and the swap out task woken. The
 * caller then blocks on the waiting semaphore and counter. When buffers return
 * from the upper layers (access) or lower driver (transfer) the blocked caller
 * task is woken and this procedure is repeated. The repeat handles a case of a
 * another thread pre-empting getting a buffer first and adding it to the AVL
 * tree.
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
 * This function assumes the pool the buffer is being taken from is locked and
 * it will make sure the pool is locked when it returns. The pool will be
 * unlocked if the call could block.
 *
 * @param device The physical disk device
 * @param pool The pool reference
 * @param block Absolute media block number
 * @param read_ahead The get is for a read ahead buffer
 *
 * @return RTEMS status code ( if operation completed successfully or error
 *         code if error is occured)
 */
static rtems_bdbuf_buffer*
rtems_bdbuf_get_buffer (rtems_disk_device* pdd,
                        rtems_bdbuf_pool*  pool,
                        rtems_blkdev_bnum  block,
                        bool               read_ahead)
{
  dev_t               device = pdd->dev;
  rtems_bdbuf_buffer* bd;
  bool                available;

  /*
   * Loop until we get a buffer. Under load we could find no buffers are
   * available requiring this task to wait until some become available before
   * proceeding. There is no timeout. If the call is to block and the buffer is
   * for a read ahead buffer return NULL.
   *
   * The search procedure is repeated as another thread could have pre-empted
   * us while we waited for a buffer, obtained an empty buffer and loaded the
   * AVL tree with the one we are after.
   */
  do
  {
    /*
     * Search for buffer descriptor for this dev/block key.
     */
    bd = rtems_bdbuf_avl_search (&pool->tree, device, block);

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
       * Assign new buffer descriptor from the empty list if one is present. If
       * the empty queue is empty get the oldest buffer from LRU list. If the
       * LRU list is empty there are no available buffers check the modified
       * list.
       */
      if (rtems_chain_is_empty (&pool->ready))
      {
        /*
         * No unsed or read-ahead buffers.
         *
         * If this is a read ahead buffer just return. No need to place further
         * pressure on the cache by reading something that may be needed when
         * we have data in the cache that was needed and may still be.
         */
        if (read_ahead)
          return NULL;

        /*
         * Check the LRU list.
         */
        bd = (rtems_bdbuf_buffer *) rtems_chain_get (&pool->lru);
        
        if (bd)
        {
          /*
           * Remove the buffer from the AVL tree.
           */
          if (rtems_bdbuf_avl_remove (&pool->tree, bd) != 0)
            rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);
        }
        else
        {
          /*
           * If there are buffers on the modified list expire the hold timer
           * and wake the swap out task then wait else just go and wait.
           */
          if (!rtems_chain_is_empty (&pool->modified))
          {
            rtems_chain_node* node = rtems_chain_head (&pool->modified);
            uint32_t          write_blocks = 0;
            
            node = node->next;
            while ((write_blocks < rtems_bdbuf_configuration.max_write_blocks) &&
                   !rtems_chain_is_tail (&pool->modified, node))
            {
              rtems_bdbuf_buffer* bd = (rtems_bdbuf_buffer*) node;
              bd->hold_timer = 0;
              write_blocks++;
              node = node->next;
            }

            rtems_bdbuf_wake_swapper ();
          }
          
          /*
           * Wait for a buffer to be returned to the pool. The buffer will be
           * placed on the LRU list.
           */
          rtems_bdbuf_wait (pool, &pool->waiting, &pool->wait_waiters);
        }
      }
      else
      {
        bd = (rtems_bdbuf_buffer *) rtems_chain_get (&(pool->ready));

        if ((bd->state != RTEMS_BDBUF_STATE_EMPTY) &&
            (bd->state != RTEMS_BDBUF_STATE_READ_AHEAD))
          rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);

        if (bd->state == RTEMS_BDBUF_STATE_READ_AHEAD)
        {
          if (rtems_bdbuf_avl_remove (&pool->tree, bd) != 0)
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

        if (rtems_bdbuf_avl_insert (&pool->tree, bd) != 0)
          rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);

        return bd;
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
        rtems_bdbuf_wait (pool, &pool->access, &pool->access_waiters);
        bd->waiters--;
        break;

      case RTEMS_BDBUF_STATE_SYNC:
      case RTEMS_BDBUF_STATE_TRANSFER:
        bd->waiters++;
        rtems_bdbuf_wait (pool, &pool->transfer, &pool->transfer_waiters);
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

/**
 * Get block buffer for data to be written into. The buffers is set to the
 * access or modifed access state. If the buffer is in the cache and modified
 * the state is access modified else the state is access. This buffer contents
 * are not initialised if the buffer is not already in the cache. If the block
 * is already resident in memory it is returned how-ever if not in memory the
 * buffer is not read from disk. This call is used when writing the whole block
 * on a disk rather than just changing a part of it. If there is no buffers
 * available this call will block. A buffer obtained with this call will not be
 * involved in a transfer request and will not be returned to another user
 * until released. If the buffer is already with a user when this call is made
 * the call is blocked until the buffer is returned. The highest priority
 * waiter will obtain the buffer first.
 *
 * The block number is the linear block number. This is relative to the start
 * of the partition on the media.
 *
 * @param device Device number (constructed of major and minor device number)
 * @param block  Linear media block number
 * @param bd     Reference to the buffer descriptor pointer.
 *
 * @return       RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *               successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_get (dev_t                device,
                 rtems_blkdev_bnum    block,
                 rtems_bdbuf_buffer** bdp)
{
  rtems_disk_device*  dd;
  rtems_bdbuf_pool*   pool;
  rtems_bdbuf_buffer* bd;

  /*
   * Do not hold the pool lock when obtaining the disk table.
   */
  dd = rtems_disk_obtain (device);
  if (dd == NULL)
    return RTEMS_INVALID_ID;

  if (block >= dd->size)
  {
    rtems_disk_release (dd);
    return RTEMS_INVALID_NUMBER;
  }

  block += dd->start;

  pool = rtems_bdbuf_get_pool (dd->phys_dev->pool);
  
  rtems_bdbuf_lock_pool (pool);

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("get: %d (dev = %08x)\n", block, device);
#endif

  bd = rtems_bdbuf_get_buffer (dd->phys_dev, pool, block, false);

  if (bd->state == RTEMS_BDBUF_STATE_MODIFIED)
    bd->state = RTEMS_BDBUF_STATE_ACCESS_MODIFIED;
  else
    bd->state = RTEMS_BDBUF_STATE_ACCESS;
  
  rtems_bdbuf_unlock_pool (pool);

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

/**
 * Get the block buffer and if not already in the cache read from the disk. If
 * specified block already cached return. The buffer is set to the access or
 * modifed access state. If the buffer is in the cache and modified the state
 * is access modified else the state is access. If block is already being read
 * from disk for being written to disk this call blocks. If the buffer is
 * waiting to be written it is removed from modified queue and returned to the
 * user. If the buffer is not in the cache a new buffer is obtained and the
 * data read from disk. The call may block until these operations complete. A
 * buffer obtained with this call will not be involved in a transfer request
 * and will not be returned to another user until released. If the buffer is
 * already with a user when this call is made the call is blocked until the
 * buffer is returned. The highest priority waiter will obtain the buffer
 * first.
 *
 * @note Read ahead always reads buffers in sequence. All multi-block reads
 *       read consecutive blocks.
 *
 * @param device Device number (constructed of major and minor device number)
 * @param block  Linear media block number
 * @param bd     Reference to the buffer descriptor pointer.
 *
 * @return       RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *               successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_read (dev_t                device,
                  rtems_blkdev_bnum    block,
                  rtems_bdbuf_buffer** bdp)
{
  rtems_disk_device*    dd;
  rtems_bdbuf_pool*     pool;
  rtems_bdbuf_buffer*   bd = NULL;
  uint32_t              read_ahead_count;
  rtems_blkdev_request* req;
  
  /*
   * @todo This type of request structure is wrong and should be removed.
   */
#define bdbuf_alloc(size) __builtin_alloca (size)

  req = bdbuf_alloc (sizeof (rtems_blkdev_request) +
                     (sizeof ( rtems_blkdev_sg_buffer) *
                      rtems_bdbuf_configuration.max_read_ahead_blocks));

  /*
   * Do not hold the pool lock when obtaining the disk table.
   */
  dd = rtems_disk_obtain (device);
  if (dd == NULL)
    return RTEMS_INVALID_ID;

  block += dd->start;
  
#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("read: %d (dev = %08x)\n", block, device);
#endif
  
  if (block >= dd->size)
  {
    rtems_disk_release(dd);
    return RTEMS_INVALID_NUMBER;
  }

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

  pool = rtems_bdbuf_get_pool (dd->phys_dev->pool);

  rtems_bdbuf_lock_pool (pool);

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
    bd = rtems_bdbuf_get_buffer (dd->phys_dev, pool,
                                 block + req->bufnum,
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
     * Unlock the pool. We have the buffer for the block and it will be in the
     * access or transfer state. We may also have a number of read ahead blocks
     * if we need to transfer data. At this point any other threads can gain
     * access to the pool and if they are after any of the buffers we have they
     * will block and be woken when the buffer is returned to the pool.
     *
     * If a transfer is needed the I/O operation will occur with pre-emption
     * enabled and the pool unlocked. This is a change to the previous version
     * of the bdbuf code.
     */
    int      result;
    uint32_t b;
    
    rtems_bdbuf_unlock_pool (pool);

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
      rtems_event_set   out;
      sc = rtems_event_receive (RTEMS_BDBUF_TRANSFER_SYNC,
                                RTEMS_EVENT_ALL | RTEMS_WAIT,
                                0, &out);

      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (BLKDEV_FATAL_BDBUF_SWAPOUT_RE);
    }

    rtems_bdbuf_lock_pool (pool);

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

  rtems_bdbuf_unlock_pool (pool);
  rtems_disk_release (dd);

  *bdp = bd;

  return RTEMS_SUCCESSFUL;
}

/**
 * Release the buffer obtained by a read call back to the cache. If the buffer
 * was obtained by a get call and was not already in the cache the release
 * modified call should be used. A buffer released with this call obtained by a
 * get call may not be in sync with the contents on disk. If the buffer was in
 * the cache and modified before this call it will be returned to the modified
 * queue. The buffers is returned to the end of the LRU list.
 *
 * @param bd Reference to the buffer descriptor.
 *
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_release (rtems_bdbuf_buffer* bd)
{
  rtems_bdbuf_pool* pool;

  if (bd == NULL)
    return RTEMS_INVALID_ADDRESS;

  pool = rtems_bdbuf_get_pool (bd->pool);

  rtems_bdbuf_lock_pool (pool);

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("release: %d\n", bd->block);
#endif
  
  if (bd->state == RTEMS_BDBUF_STATE_ACCESS_MODIFIED)
  {
    rtems_bdbuf_append_modified (pool, bd);
  }
  else
  {
    /*
     * If this is a read ahead buffer place the ready queue. Buffers are taken
     * from here first. If we prepend then get from the queue the buffers
     * furthermost from the read buffer will be used.
     */
    if (bd->state == RTEMS_BDBUF_STATE_READ_AHEAD)
      rtems_chain_prepend (&pool->ready, &bd->link);
    else
    {
      bd->state = RTEMS_BDBUF_STATE_CACHED;
      rtems_chain_append (&pool->lru, &bd->link);
    }
  }
  
  /*
   * If there are threads waiting to access the buffer wake them. Wake any
   * waiters if this is the first buffer to placed back onto the queue.
   */
  if (bd->waiters)
    rtems_bdbuf_wake (pool->access, &pool->access_waiters);
  else
  {
    if (bd->state == RTEMS_BDBUF_STATE_READ_AHEAD)
    {
      if (rtems_chain_has_only_one_node (&pool->ready))
        rtems_bdbuf_wake (pool->waiting, &pool->wait_waiters);
    }
    else
    {
      if (rtems_chain_has_only_one_node (&pool->lru))
        rtems_bdbuf_wake (pool->waiting, &pool->wait_waiters);
    }
  }
  
  rtems_bdbuf_unlock_pool (pool);

  return RTEMS_SUCCESSFUL;
}

/**
 * Release the buffer allocated with a get or read call placing it on the
 * modidied list.  If the buffer was not released modified before the hold
 * timer is set to the configuration value. If the buffer had been released
 * modified before but not written to disk the hold timer is not updated. The
 * buffer will be written to disk when the hold timer has expired, there are
 * not more buffers available in the cache and a get or read buffer needs one
 * or a sync call has been made. If the buffer is obtained with a get or read
 * before the hold timer has expired the buffer will be returned to the user.
 *
 * @param bd Reference to the buffer descriptor.
 *
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_release_modified (rtems_bdbuf_buffer* bd)
{
  rtems_bdbuf_pool* pool;

  if (bd == NULL)
    return RTEMS_INVALID_ADDRESS;

  pool = rtems_bdbuf_get_pool (bd->pool);

  rtems_bdbuf_lock_pool (pool);

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("release modified: %d\n", bd->block);
#endif

  bd->hold_timer = rtems_bdbuf_configuration.swap_block_hold;
  
  rtems_bdbuf_append_modified (pool, bd);

  if (bd->waiters)
    rtems_bdbuf_wake (pool->access, &pool->access_waiters);
  
  rtems_bdbuf_unlock_pool (pool);

  return RTEMS_SUCCESSFUL;
}

/**
 * Release the buffer as modified and wait until it has been synchronized with
 * the disk by writing it. This buffer will be the first to be transfer to disk
 * and other buffers may also be written if the maximum number of blocks in a
 * requests allows it.
 *
 * @note This code does not lock the sync mutex and stop additions to the
 *       modified queue.
 *
 * @param bd Reference to the buffer descriptor.
 *
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_sync (rtems_bdbuf_buffer* bd)
{
  rtems_bdbuf_pool* pool;
  bool              available;

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("sync: %d\n", bd->block);
#endif
  
  if (bd == NULL)
    return RTEMS_INVALID_ADDRESS;

  pool = rtems_bdbuf_get_pool (bd->pool);

  rtems_bdbuf_lock_pool (pool);

  bd->state = RTEMS_BDBUF_STATE_SYNC;

  rtems_chain_append (&pool->sync, &bd->link);

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
        rtems_bdbuf_wait (pool, &pool->transfer, &pool->transfer_waiters);
        bd->waiters--;
        break;

      default:
        rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CONSISTENCY);
    }
  }

  rtems_bdbuf_unlock_pool (pool);
  
  return RTEMS_SUCCESSFUL;
}

/**
 * Synchronize all modified buffers for this device with the disk and wait
 * until the transfers have completed. The sync mutex for the pool is locked
 * stopping the addition of any further modifed buffers. It is only the
 * currently modified buffers that are written.
 *
 * @param dev Block device number
 *
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_syncdev (dev_t dev)
{
  rtems_disk_device*  dd;
  rtems_bdbuf_pool*   pool;
  rtems_status_code   sc;
  rtems_event_set     out;

#if RTEMS_BDBUF_TRACE
  rtems_bdbuf_printf ("syncdev: %08x\n", dev);
#endif

  /*
   * Do not hold the pool lock when obtaining the disk table.
   */
  dd = rtems_disk_obtain (dev);
  if (dd == NULL)
    return RTEMS_INVALID_ID;

  pool = rtems_bdbuf_get_pool (dd->pool);

  /*
   * Take the sync lock before locking the pool. Once we have the sync lock
   * we can lock the pool. If another thread has the sync lock it will cause
   * this thread to block until it owns the sync lock then it can own the
   * pool. The sync lock can only be obtained with the pool unlocked.
   */
  
  rtems_bdbuf_lock_sync (pool);
  rtems_bdbuf_lock_pool (pool);  

  /*
   * Set the pool to have a sync active for a specific device and let the swap
   * out task know the id of the requester to wake when done.
   *
   * The swap out task will negate the sync active flag when no more buffers
   * for the device are held on the modified for sync queues.
   */
  pool->sync_active    = true;
  pool->sync_requester = rtems_task_self ();
  pool->sync_device    = dev;
  
  rtems_bdbuf_wake_swapper ();
  rtems_bdbuf_unlock_pool (pool);
  
  sc = rtems_event_receive (RTEMS_BDBUF_TRANSFER_SYNC,
                            RTEMS_EVENT_ALL | RTEMS_WAIT,
                            0, &out);

  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (BLKDEV_FATAL_BDBUF_SWAPOUT_RE);
      
  rtems_bdbuf_unlock_sync (pool);
  
  return rtems_disk_release(dd);
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
 * Process the modified list of buffers. There us a sync or modified list that
 * needs to be handled.
 *
 * @param pid The pool id to process modified buffers on.
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
rtems_bdbuf_swapout_modified_processing (rtems_bdpool_id      pid,
                                         dev_t*               dev,
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
    
      if (bd->pool == pid)
      {
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
}

/**
 * Process a pool's modified buffers. Check the sync list first then the
 * modified list extracting the buffers suitable to be written to disk. We have
 * a device at a time. The task level loop will repeat this operation while
 * there are buffers to be written. If the transfer fails place the buffers
 * back on the modified list and try again later. The pool is unlocked while
 * the buffers are beign written to disk.
 *
 * @param pid The pool id to process modified buffers on.
 * @param timer_delta It update_timers is true update the timers by this
 *                    amount.
 * @param update_timers If true update the timers.
 * @param write_req The write request structure. There is only one.
 *
 * @retval true Buffers where written to disk so scan again.
 * @retval false No buffers where written to disk.
 */
static bool
rtems_bdbuf_swapout_pool_processing (rtems_bdpool_id       pid,
                                     unsigned long         timer_delta,
                                     bool                  update_timers,
                                     rtems_blkdev_request* write_req)
{
  rtems_bdbuf_pool*   pool = rtems_bdbuf_get_pool (pid);
  rtems_chain_control transfer;
  dev_t               dev = -1;
  rtems_disk_device*  dd;
  bool                transfered_buffers = true;

  rtems_chain_initialize_empty (&transfer);
    
  rtems_bdbuf_lock_pool (pool);

  /*
   * When the sync is for a device limit the sync to that device. If the sync
   * is for a buffer handle the devices in the order on the sync list. This
   * means the dev is -1.
   */
  if (pool->sync_active)
    dev = pool->sync_device;

  /*
   * If we have any buffers in the sync queue move then to the modified
   * list. The first sync buffer will select the device we use.
   */
  rtems_bdbuf_swapout_modified_processing (pid, &dev,
                                           &pool->sync, &transfer,
                                           true, false,
                                           timer_delta);

  /*
   * Process the pool's modified list.
   */
  rtems_bdbuf_swapout_modified_processing (pid, &dev,
                                           &pool->modified, &transfer,
                                           pool->sync_active,
                                           update_timers,
                                           timer_delta);

  /*
   * We have all the buffers that have been modified for this device so
   * the pool can be unlocked because the state is set to TRANSFER.
   */

  rtems_bdbuf_unlock_pool (pool);

  /*
   * If there are buffers to transfer to the media tranfer them.
   */
  if (rtems_chain_is_empty (&transfer))
    transfered_buffers = false;
  else
  {
    /*
     * Obtain the disk device. Release the pool mutex to avoid a dead
     * lock.
     */
    dd = rtems_disk_obtain (dev);
    if (dd == NULL)
       transfered_buffers = false;
    else
    {
      /*
       * The last block number used when the driver only supports
       * continuous blocks in a single request.
       */
      uint32_t last_block = 0;
      
      /*
       * Take as many buffers as configured and pass to the driver. Note, the
       * API to the drivers has the array of buffers and if a chain was passed
       * we could have just passed the list. If the driver API is updated it
       * should be possible to make this change with little effect in this
       * code. The array that is passed is broken in design and should be
       * removed. Merging to members of a struct into the first member is
       * trouble waiting to happen.
       */

      write_req->status = RTEMS_RESOURCE_IN_USE;
      write_req->error = 0;
      write_req->bufnum = 0;

      while (!rtems_chain_is_empty (&transfer))
      {
        rtems_bdbuf_buffer* bd =
          (rtems_bdbuf_buffer*) rtems_chain_get (&transfer);

        bool write = false;
        
        /*
         * If the device only accepts sequential buffers and this is not the
         * first buffer (the first is always sequential, and the buffer is not
         * sequential then put the buffer back on the transfer chain and write
         * the committed buffers.
         */
        
        if ((dd->capabilities & RTEMS_BLKDEV_CAP_MULTISECTOR_CONT) &&
            write_req->bufnum &&
            (bd->block != (last_block + 1)))
        {
          rtems_chain_prepend (&transfer, &bd->link);
          write = true;
        }
        else
        {
          write_req->bufs[write_req->bufnum].user   = bd;
          write_req->bufs[write_req->bufnum].block  = bd->block;
          write_req->bufs[write_req->bufnum].length = dd->block_size;
          write_req->bufs[write_req->bufnum].buffer = bd->buffer;
          write_req->bufnum++;
          last_block = bd->block;
        }

        /*
         * Perform the transfer if there are no more buffers, or the transfer
         * size has reached the configured max. value.
         */

        if (rtems_chain_is_empty (&transfer) ||
            (write_req->bufnum >= rtems_bdbuf_configuration.max_write_blocks))
          write = true;

        if (write)
        {
          int result;
          uint32_t b;

          /*
           * Perform the transfer. No pool locks, no preemption, only the disk
           * device is being held.
           */
          result = dd->ioctl (dd->phys_dev->dev,
                              RTEMS_BLKIO_REQUEST, write_req);

          if (result < 0)
          {
            rtems_bdbuf_lock_pool (pool);
              
            for (b = 0; b < write_req->bufnum; b++)
            {
              bd = write_req->bufs[b].user;
              bd->state  = RTEMS_BDBUF_STATE_MODIFIED;
              bd->error = errno;

              /*
               * Place back on the pools modified queue and try again.
               *
               * @warning Not sure this is the best option but I do not know
               *          what else can be done.
               */
              rtems_chain_append (&pool->modified, &bd->link);
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

            rtems_bdbuf_lock_pool (pool);

            for (b = 0; b < write_req->bufnum; b++)
            {
              bd = write_req->bufs[b].user;
              bd->state = RTEMS_BDBUF_STATE_CACHED;
              bd->error = 0;

              rtems_chain_append (&pool->lru, &bd->link);
              
              if (bd->waiters)
                rtems_bdbuf_wake (pool->transfer, &pool->transfer_waiters);
              else
              {
                if (rtems_chain_has_only_one_node (&pool->lru))
                  rtems_bdbuf_wake (pool->waiting, &pool->wait_waiters);
              }
            }
          }

          rtems_bdbuf_unlock_pool (pool);

          write_req->status = RTEMS_RESOURCE_IN_USE;
          write_req->error = 0;
          write_req->bufnum = 0;
        }
      }
          
      rtems_disk_release (dd);
    }
  }

  if (pool->sync_active && !  transfered_buffers)
  {
    rtems_id sync_requester = pool->sync_requester;
    pool->sync_active = false;
    pool->sync_requester = 0;
    if (sync_requester)
      rtems_event_send (sync_requester, RTEMS_BDBUF_TRANSFER_SYNC);
  }
  
  return  transfered_buffers;
}

/**
 * Body of task which takes care on flushing modified buffers to the disk.
 *
 * @param arg The task argument which is the context.
 */
static rtems_task
rtems_bdbuf_swapout_task (rtems_task_argument arg)
{
  rtems_bdbuf_context*  context = (rtems_bdbuf_context*) arg;
  rtems_blkdev_request* write_req;
  uint32_t              period_in_ticks;
  const uint32_t        period_in_msecs = rtems_bdbuf_configuration.swapout_period;
  uint32_t              timer_delta;
  rtems_status_code     sc;

  /*
   * @note chrisj The rtems_blkdev_request and the array at the end is a hack.
   * I am disappointment at finding code like this in RTEMS. The request should
   * have been a rtems_chain_control. Simple, fast and less storage as the node
   * is already part of the buffer structure.
   */
  write_req =
    malloc (sizeof (rtems_blkdev_request) +
            (rtems_bdbuf_configuration.max_write_blocks *
             sizeof (rtems_blkdev_sg_buffer)));

  if (!write_req)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_NOMEM);

  write_req->req = RTEMS_BLKDEV_REQ_WRITE;
  write_req->req_done = rtems_bdbuf_write_done;
  write_req->done_arg = write_req;
  write_req->io_task = rtems_task_self ();

  period_in_ticks = TOD_MICROSECONDS_TO_TICKS (period_in_msecs * 1000);

  /*
   * This is temporary. Needs to be changed to use the real time clock.
   */
  timer_delta = period_in_msecs;

  while (context->swapout_enabled)
  {
    rtems_event_set out;

    /*
     * Only update the timers once in the processing cycle.
     */
    bool update_timers = true;
    
    /*
     * If we write buffers to any disk perform a check again. We only write a
     * single device at a time and a pool may have more than one devices
     * buffers modified waiting to be written.
     */
    bool transfered_buffers;

    do
    {
      rtems_bdpool_id pid;
    
      transfered_buffers = false;

      /*
       * Loop over each pool extacting all the buffers we find for a specific
       * device. The device is the first one we find on a modified list of a
       * pool. Process the sync queue of buffers first.
       */
      for (pid = 0; pid < context->npools; pid++)
      {
        if (rtems_bdbuf_swapout_pool_processing (pid,
                                                 timer_delta,
                                                 update_timers,
                                                 write_req))
        {
          transfered_buffers = true;
        }
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

  free (write_req);

  rtems_task_delete (RTEMS_SELF);
}

/**
 * Find first appropriate buffer pool. This primitive returns the index of
 * first buffer pool which block size is greater than or equal to specified
 * size.
 *
 * @param block_size Requested block size
 * @param pool The pool to use for the requested pool size.
 *
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 * @retval RTEMS_INVALID_SIZE The specified block size is invalid (not a power
 *         of 2)
 * @retval RTEMS_NOT_DEFINED The buffer pool for this or greater block size
 *         is not configured.
 */
rtems_status_code
rtems_bdbuf_find_pool (uint32_t block_size, rtems_bdpool_id *pool)
{
  rtems_bdbuf_pool* p;
  rtems_bdpool_id   i;
  rtems_bdpool_id   curid = -1;
  bool              found = false;
  uint32_t          cursize = UINT_MAX;
  int               j;

  for (j = block_size; (j != 0) && ((j & 1) == 0); j >>= 1);
  if (j != 1)
    return RTEMS_INVALID_SIZE;

  for (i = 0; i < rtems_bdbuf_ctx.npools; i++)
  {
    p = rtems_bdbuf_get_pool (i);
    if ((p->blksize >= block_size) &&
        (p->blksize < cursize))
    {
      curid = i;
      cursize = p->blksize;
      found = true;
    }
  }

  if (found)
  {
    if (pool != NULL)
      *pool = curid;
    return RTEMS_SUCCESSFUL;
  }
  else
  {
    return RTEMS_NOT_DEFINED;
  }
}

/**
 * Obtain characteristics of buffer pool with specified number.
 *
 * @param pool Buffer pool number
 * @param block_size Block size for which buffer pool is configured returned
 *                   there
 * @param blocks Number of buffers in buffer pool.
 *
 * RETURNS:
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 * @retval RTEMS_INVALID_SIZE The appropriate buffer pool is not configured.
 *
 * @note Buffer pools enumerated continuously starting from 0.
 */
rtems_status_code
rtems_bdbuf_get_pool_info (rtems_bdpool_id pool, int* block_size, int* blocks)
{
  if (pool >= rtems_bdbuf_ctx.npools)
    return RTEMS_INVALID_NUMBER;

  if (block_size != NULL)
  {
    *block_size = rtems_bdbuf_ctx.pool[pool].blksize;
  }

  if (blocks != NULL)
  {
    *blocks = rtems_bdbuf_ctx.pool[pool].nblks;
  }

  return RTEMS_SUCCESSFUL;
}
