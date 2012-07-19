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
 * Copyright (c) 2009-2012 embedded brains GmbH.
 */

/**
 * Set to 1 to enable debug tracing.
 */
#define RTEMS_BDBUF_TRACE 0

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/malloc.h>

#include "rtems/bdbuf.h"

#define BDBUF_INVALID_DEV NULL

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
  rtems_chain_control   bds;         /**< The transfer list of BDs. */
  rtems_disk_device    *dd;          /**< The device the transfer is for. */
  bool                  syncing;     /**< The data is a sync'ing. */
  rtems_blkdev_request* write_req;   /**< The write request array. */
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
  bool                         enabled;  /**< The worker is enabled. */
  rtems_bdbuf_swapout_transfer transfer; /**< The transfer data for this
                                          * thread. */
} rtems_bdbuf_swapout_worker;

/**
 * Buffer waiters synchronization.
 */
typedef struct rtems_bdbuf_waiters {
  unsigned count;
  rtems_id sema;
} rtems_bdbuf_waiters;

/**
 * The BD buffer cache.
 */
typedef struct rtems_bdbuf_cache
{
  rtems_id            swapout;           /**< Swapout task ID */
  bool                swapout_enabled;   /**< Swapout is only running if
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
  bool                sync_active;       /**< True if a sync is active. */
  rtems_id            sync_requester;    /**< The sync requester. */
  rtems_disk_device  *sync_device;       /**< The device to sync and
                                          * BDBUF_INVALID_DEV not a device
                                          * sync. */

  rtems_bdbuf_buffer* tree;              /**< Buffer descriptor lookup AVL tree
                                          * root. There is only one. */
  rtems_chain_control lru;               /**< Least recently used list */
  rtems_chain_control modified;          /**< Modified buffers list */
  rtems_chain_control sync;              /**< Buffers to sync list */

  rtems_bdbuf_waiters access_waiters;    /**< Wait for a buffer in
                                          * ACCESS_CACHED, ACCESS_MODIFIED or
                                          * ACCESS_EMPTY
                                          * state. */
  rtems_bdbuf_waiters transfer_waiters;  /**< Wait for a buffer in TRANSFER
                                          * state. */
  rtems_bdbuf_waiters buffer_waiters;    /**< Wait for a buffer and no one is
                                          * available. */

  size_t              group_count;       /**< The number of groups. */
  rtems_bdbuf_group*  groups;            /**< The groups. */
  rtems_id            read_ahead_task;   /**< Read-ahead task */
  rtems_chain_control read_ahead_chain;  /**< Read-ahead request chain */
  bool                read_ahead_enabled; /**< Read-ahead enabled */

  bool                initialised;       /**< Initialised state. */
} rtems_bdbuf_cache;

/**
 * Fatal errors
 */
#define RTEMS_BLKDEV_FATAL_ERROR(n) \
  (((uint32_t)'B' << 24) | ((uint32_t)(n) & (uint32_t)0x00FFFFFF))

#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_11      RTEMS_BLKDEV_FATAL_ERROR(1)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_4       RTEMS_BLKDEV_FATAL_ERROR(2)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_5       RTEMS_BLKDEV_FATAL_ERROR(3)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_6       RTEMS_BLKDEV_FATAL_ERROR(4)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_7       RTEMS_BLKDEV_FATAL_ERROR(5)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_8       RTEMS_BLKDEV_FATAL_ERROR(6)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_9       RTEMS_BLKDEV_FATAL_ERROR(7)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_10      RTEMS_BLKDEV_FATAL_ERROR(8)
#define RTEMS_BLKDEV_FATAL_BDBUF_TREE_RM       RTEMS_BLKDEV_FATAL_ERROR(9)
#define RTEMS_BLKDEV_FATAL_BDBUF_SWAPOUT       RTEMS_BLKDEV_FATAL_ERROR(10)

/*
 * The lock/unlock fatal errors occur in case the bdbuf is not initialized with
 * rtems_bdbuf_init().  General system corruption like stack overflow etc. may
 * also trigger these fatal errors.
 */
#define RTEMS_BLKDEV_FATAL_BDBUF_SYNC_LOCK     RTEMS_BLKDEV_FATAL_ERROR(11)
#define RTEMS_BLKDEV_FATAL_BDBUF_SYNC_UNLOCK   RTEMS_BLKDEV_FATAL_ERROR(12)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_LOCK    RTEMS_BLKDEV_FATAL_ERROR(13)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_UNLOCK  RTEMS_BLKDEV_FATAL_ERROR(14)

#define RTEMS_BLKDEV_FATAL_BDBUF_PREEMPT_DIS   RTEMS_BLKDEV_FATAL_ERROR(15)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_2  RTEMS_BLKDEV_FATAL_ERROR(16)
#define RTEMS_BLKDEV_FATAL_BDBUF_PREEMPT_RST   RTEMS_BLKDEV_FATAL_ERROR(17)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_TO RTEMS_BLKDEV_FATAL_ERROR(18)
#define RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAKE    RTEMS_BLKDEV_FATAL_ERROR(19)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_WAKE       RTEMS_BLKDEV_FATAL_ERROR(20)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_NOMEM      RTEMS_BLKDEV_FATAL_ERROR(21)
#define RTEMS_BLKDEV_FATAL_BDBUF_SO_WK_CREATE  RTEMS_BLKDEV_FATAL_ERROR(22)
#define BLKDEV_FATAL_BDBUF_SWAPOUT_RE          RTEMS_BLKDEV_FATAL_ERROR(24)
#define BLKDEV_FATAL_BDBUF_SWAPOUT_TS          RTEMS_BLKDEV_FATAL_ERROR(25)
#define RTEMS_BLKDEV_FATAL_BDBUF_WAIT_EVNT     RTEMS_BLKDEV_FATAL_ERROR(26)
#define RTEMS_BLKDEV_FATAL_BDBUF_RECYCLE       RTEMS_BLKDEV_FATAL_ERROR(27)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_0       RTEMS_BLKDEV_FATAL_ERROR(28)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_1       RTEMS_BLKDEV_FATAL_ERROR(29)
#define RTEMS_BLKDEV_FATAL_BDBUF_STATE_2       RTEMS_BLKDEV_FATAL_ERROR(30)
#define RTEMS_BLKDEV_FATAL_BDBUF_RA_WAKE_UP    RTEMS_BLKDEV_FATAL_ERROR(31)

/**
 * The events used in this code. These should be system events rather than
 * application events.
 */
#define RTEMS_BDBUF_TRANSFER_SYNC  RTEMS_EVENT_1
#define RTEMS_BDBUF_SWAPOUT_SYNC   RTEMS_EVENT_2
#define RTEMS_BDBUF_READ_AHEAD_WAKE_UP RTEMS_EVENT_1

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

/**
 * Waiter timeout. Set to non-zero to find some info on a waiter that is
 * waiting too long.
 */
#define RTEMS_BDBUF_WAIT_TIMEOUT RTEMS_NO_TIMEOUT
#if !defined (RTEMS_BDBUF_WAIT_TIMEOUT)
#define RTEMS_BDBUF_WAIT_TIMEOUT \
  (TOD_MICROSECONDS_TO_TICKS (20000000))
#endif

static rtems_task rtems_bdbuf_swapout_task(rtems_task_argument arg);

static rtems_task rtems_bdbuf_read_ahead_task(rtems_task_argument arg);

/**
 * The Buffer Descriptor cache.
 */
static rtems_bdbuf_cache bdbuf_cache;

#if RTEMS_BDBUF_TRACE
/**
 * If true output the trace message.
 */
bool rtems_bdbuf_tracer;

/**
 * Return the number of items on the list.
 *
 * @param list The chain control.
 * @return uint32_t The number of items on the list.
 */
uint32_t
rtems_bdbuf_list_count (rtems_chain_control* list)
{
  rtems_chain_node* node = rtems_chain_first (list);
  uint32_t          count = 0;
  while (!rtems_chain_is_tail (list, node))
  {
    count++;
    node = rtems_chain_next (node);
  }
  return count;
}

/**
 * Show the usage for the bdbuf cache.
 */
void
rtems_bdbuf_show_usage (void)
{
  uint32_t group;
  uint32_t total = 0;
  uint32_t val;

  for (group = 0; group < bdbuf_cache.group_count; group++)
    total += bdbuf_cache.groups[group].users;
  printf ("bdbuf:group users=%lu", total);
  val = rtems_bdbuf_list_count (&bdbuf_cache.lru);
  printf (", lru=%lu", val);
  total = val;
  val = rtems_bdbuf_list_count (&bdbuf_cache.modified);
  printf (", mod=%lu", val);
  total += val;
  val = rtems_bdbuf_list_count (&bdbuf_cache.sync);
  printf (", sync=%lu", val);
  total += val;
  printf (", total=%lu\n", total);
}

/**
 * Show the users for a group of a bd.
 *
 * @param where A label to show the context of output.
 * @param bd The bd to show the users of.
 */
void
rtems_bdbuf_show_users (const char* where, rtems_bdbuf_buffer* bd)
{
  const char* states[] =
    { "FR", "EM", "CH", "AC", "AM", "AE", "AP", "MD", "SY", "TR", "TP" };

  printf ("bdbuf:users: %15s: [%" PRIu32 " (%s)] %td:%td = %" PRIu32 " %s\n",
          where,
          bd->block, states[bd->state],
          bd->group - bdbuf_cache.groups,
          bd - bdbuf_cache.bds,
          bd->group->users,
          bd->group->users > 8 ? "<<<<<<<" : "");
}
#else
#define rtems_bdbuf_tracer (0)
#define rtems_bdbuf_show_usage() ((void) 0)
#define rtems_bdbuf_show_users(_w, _b) ((void) 0)
#endif

/**
 * The default maximum height of 32 allows for AVL trees having between
 * 5,704,880 and 4,294,967,295 nodes, depending on order of insertion.  You may
 * change this compile-time constant as you wish.
 */
#ifndef RTEMS_BDBUF_AVL_MAX_HEIGHT
#define RTEMS_BDBUF_AVL_MAX_HEIGHT (32)
#endif

static void
rtems_bdbuf_fatal (rtems_bdbuf_buf_state state, uint32_t error)
{
  rtems_fatal_error_occurred ((((uint32_t) state) << 16) | error);
}

/**
 * Searches for the node with specified dd/block.
 *
 * @param root pointer to the root node of the AVL-Tree
 * @param dd disk device search key
 * @param block block search key
 * @retval NULL node with the specified dd/block is not found
 * @return pointer to the node with specified dd/block
 */
static rtems_bdbuf_buffer *
rtems_bdbuf_avl_search (rtems_bdbuf_buffer** root,
                        const rtems_disk_device *dd,
                        rtems_blkdev_bnum    block)
{
  rtems_bdbuf_buffer* p = *root;

  while ((p != NULL) && ((p->dd != dd) || (p->block != block)))
  {
    if (((uintptr_t) p->dd < (uintptr_t) dd)
        || ((p->dd == dd) && (p->block < block)))
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
  const rtems_disk_device *dd = node->dd;
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

    if (((uintptr_t) p->dd < (uintptr_t) dd)
        || ((p->dd == dd) && (p->block < block)))
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
    else if ((p->dd != dd) || (p->block != block))
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
  const rtems_disk_device *dd = node->dd;
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

    if (((uintptr_t) p->dd < (uintptr_t) dd)
        || ((p->dd == dd) && (p->block < block)))
    {
      p->avl.cache = 1;
      p = p->avl.right;
    }
    else if ((p->dd != dd) || (p->block != block))
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

static void
rtems_bdbuf_set_state (rtems_bdbuf_buffer *bd, rtems_bdbuf_buf_state state)
{
  bd->state = state;
}

static rtems_blkdev_bnum
rtems_bdbuf_media_block (const rtems_disk_device *dd, rtems_blkdev_bnum block)
{
  if (dd->block_to_media_block_shift >= 0)
    return block << dd->block_to_media_block_shift;
  else
    /*
     * Change the block number for the block size to the block number for the media
     * block size. We have to use 64bit maths. There is no short cut here.
     */
    return (rtems_blkdev_bnum)
      ((((uint64_t) block) * dd->block_size) / dd->media_block_size);
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

static void
rtems_bdbuf_group_obtain (rtems_bdbuf_buffer *bd)
{
  ++bd->group->users;
}

static void
rtems_bdbuf_group_release (rtems_bdbuf_buffer *bd)
{
  --bd->group->users;
}

static rtems_mode
rtems_bdbuf_disable_preemption (void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_mode prev_mode = 0;

  sc = rtems_task_mode (RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &prev_mode);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_PREEMPT_DIS);

  return prev_mode;
}

static void
rtems_bdbuf_restore_preemption (rtems_mode prev_mode)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_task_mode (prev_mode, RTEMS_ALL_MODE_MASKS, &prev_mode);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_PREEMPT_RST);
}

/**
 * Wait until woken. Semaphores are used so a number of tasks can wait and can
 * be woken at once. Task events would require we maintain a list of tasks to
 * be woken and this would require storage and we do not know the number of
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
 */
static void
rtems_bdbuf_anonymous_wait (rtems_bdbuf_waiters *waiters)
{
  rtems_status_code sc;
  rtems_mode        prev_mode;

  /*
   * Indicate we are waiting.
   */
  ++waiters->count;

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
  prev_mode = rtems_bdbuf_disable_preemption ();

  /*
   * Unlock the cache, wait, and lock the cache when we return.
   */
  rtems_bdbuf_unlock_cache ();

  sc = rtems_semaphore_obtain (waiters->sema, RTEMS_WAIT, RTEMS_BDBUF_WAIT_TIMEOUT);

  if (sc == RTEMS_TIMEOUT)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_TO);

  if (sc != RTEMS_UNSATISFIED)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAIT_2);

  rtems_bdbuf_lock_cache ();

  rtems_bdbuf_restore_preemption (prev_mode);

  --waiters->count;
}

static void
rtems_bdbuf_wait (rtems_bdbuf_buffer *bd, rtems_bdbuf_waiters *waiters)
{
  rtems_bdbuf_group_obtain (bd);
  ++bd->waiters;
  rtems_bdbuf_anonymous_wait (waiters);
  --bd->waiters;
  rtems_bdbuf_group_release (bd);
}

/**
 * Wake a blocked resource. The resource has a counter that lets us know if
 * there are any waiters.
 */
static void
rtems_bdbuf_wake (const rtems_bdbuf_waiters *waiters)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (waiters->count > 0)
  {
    sc = rtems_semaphore_flush (waiters->sema);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_CACHE_WAKE);
  }
}

static void
rtems_bdbuf_wake_swapper (void)
{
  rtems_status_code sc = rtems_event_send (bdbuf_cache.swapout,
                                           RTEMS_BDBUF_SWAPOUT_SYNC);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_WAKE);
}

static bool
rtems_bdbuf_has_buffer_waiters (void)
{
  return bdbuf_cache.buffer_waiters.count;
}

static void
rtems_bdbuf_remove_from_tree (rtems_bdbuf_buffer *bd)
{
  if (rtems_bdbuf_avl_remove (&bdbuf_cache.tree, bd) != 0)
    rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_TREE_RM);
}

static void
rtems_bdbuf_remove_from_tree_and_lru_list (rtems_bdbuf_buffer *bd)
{
  switch (bd->state)
  {
    case RTEMS_BDBUF_STATE_FREE:
      break;
    case RTEMS_BDBUF_STATE_CACHED:
      rtems_bdbuf_remove_from_tree (bd);
      break;
    default:
      rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_10);
  }

  rtems_chain_extract_unprotected (&bd->link);
}

static void
rtems_bdbuf_make_free_and_add_to_lru_list (rtems_bdbuf_buffer *bd)
{
  rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_FREE);
  rtems_chain_prepend_unprotected (&bdbuf_cache.lru, &bd->link);
}

static void
rtems_bdbuf_make_empty (rtems_bdbuf_buffer *bd)
{
  rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_EMPTY);
}

static void
rtems_bdbuf_make_cached_and_add_to_lru_list (rtems_bdbuf_buffer *bd)
{
  rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_CACHED);
  rtems_chain_append_unprotected (&bdbuf_cache.lru, &bd->link);
}

static void
rtems_bdbuf_discard_buffer (rtems_bdbuf_buffer *bd)
{
  rtems_bdbuf_make_empty (bd);

  if (bd->waiters == 0)
  {
    rtems_bdbuf_remove_from_tree (bd);
    rtems_bdbuf_make_free_and_add_to_lru_list (bd);
  }
}

static void
rtems_bdbuf_add_to_modified_list_after_access (rtems_bdbuf_buffer *bd)
{
  if (bdbuf_cache.sync_active && bdbuf_cache.sync_device == bd->dd)
  {
    rtems_bdbuf_unlock_cache ();

    /*
     * Wait for the sync lock.
     */
    rtems_bdbuf_lock_sync ();

    rtems_bdbuf_unlock_sync ();
    rtems_bdbuf_lock_cache ();
  }

  /*
   * Only the first modified release sets the timer and any further user
   * accesses do not change the timer value which should move down. This
   * assumes the user's hold of the buffer is much less than the time on the
   * modified list. Resetting the timer on each access which could result in a
   * buffer never getting to 0 and never being forced onto disk. This raises a
   * difficult question. Is a snapshot of a block that is changing better than
   * nothing being written? We have tended to think we should hold changes for
   * only a specific period of time even if still changing and get onto disk
   * and letting the file system try and recover this position if it can.
   */
  if (bd->state == RTEMS_BDBUF_STATE_ACCESS_CACHED
        || bd->state == RTEMS_BDBUF_STATE_ACCESS_EMPTY)
    bd->hold_timer = bdbuf_config.swap_block_hold;

  rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_MODIFIED);
  rtems_chain_append_unprotected (&bdbuf_cache.modified, &bd->link);

  if (bd->waiters)
    rtems_bdbuf_wake (&bdbuf_cache.access_waiters);
  else if (rtems_bdbuf_has_buffer_waiters ())
    rtems_bdbuf_wake_swapper ();
}

static void
rtems_bdbuf_add_to_lru_list_after_access (rtems_bdbuf_buffer *bd)
{
  rtems_bdbuf_group_release (bd);
  rtems_bdbuf_make_cached_and_add_to_lru_list (bd);

  if (bd->waiters)
    rtems_bdbuf_wake (&bdbuf_cache.access_waiters);
  else
    rtems_bdbuf_wake (&bdbuf_cache.buffer_waiters);
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

  if (size > bdbuf_config.buffer_max)
    return 0;

  bufs_per_size = ((size - 1) / bdbuf_config.buffer_min) + 1;

  for (bds_per_size = 1;
       bds_per_size < bufs_per_size;
       bds_per_size <<= 1)
    ;

  return bdbuf_cache.max_bds_per_group / bds_per_size;
}

static void
rtems_bdbuf_discard_buffer_after_access (rtems_bdbuf_buffer *bd)
{
  rtems_bdbuf_group_release (bd);
  rtems_bdbuf_discard_buffer (bd);

  if (bd->waiters)
    rtems_bdbuf_wake (&bdbuf_cache.access_waiters);
  else
    rtems_bdbuf_wake (&bdbuf_cache.buffer_waiters);
}

/**
 * Reallocate a group. The BDs currently allocated in the group are removed
 * from the ALV tree and any lists then the new BD's are prepended to the ready
 * list of the cache.
 *
 * @param group The group to reallocate.
 * @param new_bds_per_group The new count of BDs per group.
 * @return A buffer of this group.
 */
static rtems_bdbuf_buffer *
rtems_bdbuf_group_realloc (rtems_bdbuf_group* group, size_t new_bds_per_group)
{
  rtems_bdbuf_buffer* bd;
  size_t              b;
  size_t              bufs_per_bd;

  if (rtems_bdbuf_tracer)
    printf ("bdbuf:realloc: %tu: %zd -> %zd\n",
            group - bdbuf_cache.groups, group->bds_per_group,
            new_bds_per_group);

  bufs_per_bd = bdbuf_cache.max_bds_per_group / group->bds_per_group;

  for (b = 0, bd = group->bdbuf;
       b < group->bds_per_group;
       b++, bd += bufs_per_bd)
    rtems_bdbuf_remove_from_tree_and_lru_list (bd);

  group->bds_per_group = new_bds_per_group;
  bufs_per_bd = bdbuf_cache.max_bds_per_group / new_bds_per_group;

  for (b = 1, bd = group->bdbuf + bufs_per_bd;
       b < group->bds_per_group;
       b++, bd += bufs_per_bd)
    rtems_bdbuf_make_free_and_add_to_lru_list (bd);

  if (b > 1)
    rtems_bdbuf_wake (&bdbuf_cache.buffer_waiters);

  return group->bdbuf;
}

static void
rtems_bdbuf_setup_empty_buffer (rtems_bdbuf_buffer *bd,
                                rtems_disk_device  *dd,
                                rtems_blkdev_bnum   block)
{
  bd->dd        = dd ;
  bd->block     = block;
  bd->avl.left  = NULL;
  bd->avl.right = NULL;
  bd->waiters   = 0;

  if (rtems_bdbuf_avl_insert (&bdbuf_cache.tree, bd) != 0)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_RECYCLE);

  rtems_bdbuf_make_empty (bd);
}

static rtems_bdbuf_buffer *
rtems_bdbuf_get_buffer_from_lru_list (rtems_disk_device *dd,
                                      rtems_blkdev_bnum  block)
{
  rtems_chain_node *node = rtems_chain_first (&bdbuf_cache.lru);

  while (!rtems_chain_is_tail (&bdbuf_cache.lru, node))
  {
    rtems_bdbuf_buffer *bd = (rtems_bdbuf_buffer *) node;
    rtems_bdbuf_buffer *empty_bd = NULL;

    if (rtems_bdbuf_tracer)
      printf ("bdbuf:next-bd: %tu (%td:%" PRId32 ") %zd -> %zd\n",
              bd - bdbuf_cache.bds,
              bd->group - bdbuf_cache.groups, bd->group->users,
              bd->group->bds_per_group, dd->bds_per_group);

    /*
     * If nobody waits for this BD, we may recycle it.
     */
    if (bd->waiters == 0)
    {
      if (bd->group->bds_per_group == dd->bds_per_group)
      {
        rtems_bdbuf_remove_from_tree_and_lru_list (bd);

        empty_bd = bd;
      }
      else if (bd->group->users == 0)
        empty_bd = rtems_bdbuf_group_realloc (bd->group, dd->bds_per_group);
    }

    if (empty_bd != NULL)
    {
      rtems_bdbuf_setup_empty_buffer (empty_bd, dd, block);

      return empty_bd;
    }

    node = rtems_chain_next (node);
  }

  return NULL;
}

static rtems_status_code
rtems_bdbuf_create_task(
  rtems_name name,
  rtems_task_priority priority,
  rtems_task_priority default_priority,
  rtems_task_entry entry,
  rtems_task_argument arg,
  rtems_id *id
)
{
  rtems_status_code sc;
  size_t stack_size = bdbuf_config.task_stack_size ?
    bdbuf_config.task_stack_size : RTEMS_BDBUF_TASK_STACK_SIZE_DEFAULT;

  priority = priority != 0 ? priority : default_priority;

  sc = rtems_task_create (name,
                          priority,
                          stack_size,
                          RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR,
                          RTEMS_LOCAL | RTEMS_NO_FLOATING_POINT,
                          id);

  if (sc == RTEMS_SUCCESSFUL)
    sc = rtems_task_start (*id, entry, arg);

  return sc;
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
  size_t              b;
  size_t              cache_aligment;
  rtems_status_code   sc;
  rtems_mode          prev_mode;

  if (rtems_bdbuf_tracer)
    printf ("bdbuf:init\n");

  if (rtems_interrupt_is_in_progress())
    return RTEMS_CALLED_FROM_ISR;

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
  prev_mode = rtems_bdbuf_disable_preemption ();
  if (bdbuf_cache.initialised)
  {
    rtems_bdbuf_restore_preemption (prev_mode);
    return RTEMS_RESOURCE_IN_USE;
  }

  memset(&bdbuf_cache, 0, sizeof(bdbuf_cache));
  bdbuf_cache.initialised = true;
  rtems_bdbuf_restore_preemption (prev_mode);

  /*
   * For unspecified cache alignments we use the CPU alignment.
   */
  cache_aligment = 32; /* FIXME rtems_cache_get_data_line_size() */
  if (cache_aligment <= 0)
    cache_aligment = CPU_ALIGNMENT;

  bdbuf_cache.sync_device = BDBUF_INVALID_DEV;

  rtems_chain_initialize_empty (&bdbuf_cache.swapout_workers);
  rtems_chain_initialize_empty (&bdbuf_cache.lru);
  rtems_chain_initialize_empty (&bdbuf_cache.modified);
  rtems_chain_initialize_empty (&bdbuf_cache.sync);
  rtems_chain_initialize_empty (&bdbuf_cache.read_ahead_chain);

  /*
   * Create the locks for the cache.
   */
  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 'l'),
                               1, RTEMS_BDBUF_CACHE_LOCK_ATTRIBS, 0,
                               &bdbuf_cache.lock);
  if (sc != RTEMS_SUCCESSFUL)
    goto error;

  rtems_bdbuf_lock_cache ();

  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 's'),
                               1, RTEMS_BDBUF_CACHE_LOCK_ATTRIBS, 0,
                               &bdbuf_cache.sync_lock);
  if (sc != RTEMS_SUCCESSFUL)
    goto error;

  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 'a'),
                               0, RTEMS_BDBUF_CACHE_WAITER_ATTRIBS, 0,
                               &bdbuf_cache.access_waiters.sema);
  if (sc != RTEMS_SUCCESSFUL)
    goto error;

  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 't'),
                               0, RTEMS_BDBUF_CACHE_WAITER_ATTRIBS, 0,
                               &bdbuf_cache.transfer_waiters.sema);
  if (sc != RTEMS_SUCCESSFUL)
    goto error;

  sc = rtems_semaphore_create (rtems_build_name ('B', 'D', 'C', 'b'),
                               0, RTEMS_BDBUF_CACHE_WAITER_ATTRIBS, 0,
                               &bdbuf_cache.buffer_waiters.sema);
  if (sc != RTEMS_SUCCESSFUL)
    goto error;

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
  bdbuf_cache.bds = calloc (sizeof (rtems_bdbuf_buffer),
                            bdbuf_cache.buffer_min_count);
  if (!bdbuf_cache.bds)
    goto error;

  /*
   * Allocate the memory for the buffer descriptors.
   */
  bdbuf_cache.groups = calloc (sizeof (rtems_bdbuf_group),
                               bdbuf_cache.group_count);
  if (!bdbuf_cache.groups)
    goto error;

  /*
   * Allocate memory for buffer memory. The buffer memory will be cache
   * aligned. It is possible to free the memory allocated by rtems_memalign()
   * with free(). Return 0 if allocated.
   *
   * The memory allocate allows a
   */
  if (rtems_memalign ((void **) &bdbuf_cache.buffers,
                      cache_aligment,
                      bdbuf_cache.buffer_min_count * bdbuf_config.buffer_min) != 0)
    goto error;

  /*
   * The cache is empty after opening so we need to add all the buffers to it
   * and initialise the groups.
   */
  for (b = 0, group = bdbuf_cache.groups,
         bd = bdbuf_cache.bds, buffer = bdbuf_cache.buffers;
       b < bdbuf_cache.buffer_min_count;
       b++, bd++, buffer += bdbuf_config.buffer_min)
  {
    bd->dd    = BDBUF_INVALID_DEV;
    bd->group  = group;
    bd->buffer = buffer;

    rtems_chain_append_unprotected (&bdbuf_cache.lru, &bd->link);

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
    group->bdbuf = bd;
  }

  /*
   * Create and start swapout task. This task will create and manage the worker
   * threads.
   */
  bdbuf_cache.swapout_enabled = true;

  sc = rtems_bdbuf_create_task (rtems_build_name('B', 'S', 'W', 'P'),
                                bdbuf_config.swapout_priority,
                                RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT,
                                rtems_bdbuf_swapout_task,
                                0,
                                &bdbuf_cache.swapout);
  if (sc != RTEMS_SUCCESSFUL)
    goto error;

  if (bdbuf_config.max_read_ahead_blocks > 0)
  {
    bdbuf_cache.read_ahead_enabled = true;
    sc = rtems_bdbuf_create_task (rtems_build_name('B', 'R', 'D', 'A'),
                                  bdbuf_config.read_ahead_priority,
                                  RTEMS_BDBUF_READ_AHEAD_TASK_PRIORITY_DEFAULT,
                                  rtems_bdbuf_read_ahead_task,
                                  0,
                                  &bdbuf_cache.read_ahead_task);
    if (sc != RTEMS_SUCCESSFUL)
      goto error;
  }

  rtems_bdbuf_unlock_cache ();

  return RTEMS_SUCCESSFUL;

error:

  if (bdbuf_cache.read_ahead_task != 0)
    rtems_task_delete (bdbuf_cache.read_ahead_task);

  if (bdbuf_cache.swapout != 0)
    rtems_task_delete (bdbuf_cache.swapout);

  free (bdbuf_cache.buffers);
  free (bdbuf_cache.groups);
  free (bdbuf_cache.bds);

  rtems_semaphore_delete (bdbuf_cache.buffer_waiters.sema);
  rtems_semaphore_delete (bdbuf_cache.access_waiters.sema);
  rtems_semaphore_delete (bdbuf_cache.transfer_waiters.sema);
  rtems_semaphore_delete (bdbuf_cache.sync_lock);

  if (bdbuf_cache.lock != 0)
  {
    rtems_bdbuf_unlock_cache ();
    rtems_semaphore_delete (bdbuf_cache.lock);
  }

  bdbuf_cache.initialised = false;

  return RTEMS_UNSATISFIED;
}

static void
rtems_bdbuf_wait_for_event (rtems_event_set event)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_event_set   out = 0;

  sc = rtems_event_receive (event,
                            RTEMS_EVENT_ALL | RTEMS_WAIT,
                            RTEMS_NO_TIMEOUT,
                            &out);

  if (sc != RTEMS_SUCCESSFUL || out != event)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_WAIT_EVNT);
}

static void
rtems_bdbuf_wait_for_access (rtems_bdbuf_buffer *bd)
{
  while (true)
  {
    switch (bd->state)
    {
      case RTEMS_BDBUF_STATE_MODIFIED:
        rtems_bdbuf_group_release (bd);
        /* Fall through */
      case RTEMS_BDBUF_STATE_CACHED:
        rtems_chain_extract_unprotected (&bd->link);
        /* Fall through */
      case RTEMS_BDBUF_STATE_EMPTY:
        return;
      case RTEMS_BDBUF_STATE_ACCESS_CACHED:
      case RTEMS_BDBUF_STATE_ACCESS_EMPTY:
      case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
      case RTEMS_BDBUF_STATE_ACCESS_PURGED:
        rtems_bdbuf_wait (bd, &bdbuf_cache.access_waiters);
        break;
      case RTEMS_BDBUF_STATE_SYNC:
      case RTEMS_BDBUF_STATE_TRANSFER:
      case RTEMS_BDBUF_STATE_TRANSFER_PURGED:
        rtems_bdbuf_wait (bd, &bdbuf_cache.transfer_waiters);
        break;
      default:
        rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_7);
    }
  }
}

static void
rtems_bdbuf_request_sync_for_modified_buffer (rtems_bdbuf_buffer *bd)
{
  rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_SYNC);
  rtems_chain_extract_unprotected (&bd->link);
  rtems_chain_append_unprotected (&bdbuf_cache.sync, &bd->link);
  rtems_bdbuf_wake_swapper ();
}

/**
 * @brief Waits until the buffer is ready for recycling.
 *
 * @retval @c true Buffer is valid and may be recycled.
 * @retval @c false Buffer is invalid and has to searched again.
 */
static bool
rtems_bdbuf_wait_for_recycle (rtems_bdbuf_buffer *bd)
{
  while (true)
  {
    switch (bd->state)
    {
      case RTEMS_BDBUF_STATE_FREE:
        return true;
      case RTEMS_BDBUF_STATE_MODIFIED:
        rtems_bdbuf_request_sync_for_modified_buffer (bd);
        break;
      case RTEMS_BDBUF_STATE_CACHED:
      case RTEMS_BDBUF_STATE_EMPTY:
        if (bd->waiters == 0)
          return true;
        else
        {
          /*
           * It is essential that we wait here without a special wait count and
           * without the group in use.  Otherwise we could trigger a wait ping
           * pong with another recycle waiter.  The state of the buffer is
           * arbitrary afterwards.
           */
          rtems_bdbuf_anonymous_wait (&bdbuf_cache.buffer_waiters);
          return false;
        }
      case RTEMS_BDBUF_STATE_ACCESS_CACHED:
      case RTEMS_BDBUF_STATE_ACCESS_EMPTY:
      case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
      case RTEMS_BDBUF_STATE_ACCESS_PURGED:
        rtems_bdbuf_wait (bd, &bdbuf_cache.access_waiters);
        break;
      case RTEMS_BDBUF_STATE_SYNC:
      case RTEMS_BDBUF_STATE_TRANSFER:
      case RTEMS_BDBUF_STATE_TRANSFER_PURGED:
        rtems_bdbuf_wait (bd, &bdbuf_cache.transfer_waiters);
        break;
      default:
        rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_8);
    }
  }
}

static void
rtems_bdbuf_wait_for_sync_done (rtems_bdbuf_buffer *bd)
{
  while (true)
  {
    switch (bd->state)
    {
      case RTEMS_BDBUF_STATE_CACHED:
      case RTEMS_BDBUF_STATE_EMPTY:
      case RTEMS_BDBUF_STATE_MODIFIED:
      case RTEMS_BDBUF_STATE_ACCESS_CACHED:
      case RTEMS_BDBUF_STATE_ACCESS_EMPTY:
      case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
      case RTEMS_BDBUF_STATE_ACCESS_PURGED:
        return;
      case RTEMS_BDBUF_STATE_SYNC:
      case RTEMS_BDBUF_STATE_TRANSFER:
      case RTEMS_BDBUF_STATE_TRANSFER_PURGED:
        rtems_bdbuf_wait (bd, &bdbuf_cache.transfer_waiters);
        break;
      default:
        rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_9);
    }
  }
}

static void
rtems_bdbuf_wait_for_buffer (void)
{
  if (!rtems_chain_is_empty (&bdbuf_cache.modified))
    rtems_bdbuf_wake_swapper ();

  rtems_bdbuf_anonymous_wait (&bdbuf_cache.buffer_waiters);
}

static void
rtems_bdbuf_sync_after_access (rtems_bdbuf_buffer *bd)
{
  rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_SYNC);

  rtems_chain_append_unprotected (&bdbuf_cache.sync, &bd->link);

  if (bd->waiters)
    rtems_bdbuf_wake (&bdbuf_cache.access_waiters);

  rtems_bdbuf_wake_swapper ();
  rtems_bdbuf_wait_for_sync_done (bd);

  /*
   * We may have created a cached or empty buffer which may be recycled.
   */
  if (bd->waiters == 0
        && (bd->state == RTEMS_BDBUF_STATE_CACHED
          || bd->state == RTEMS_BDBUF_STATE_EMPTY))
  {
    if (bd->state == RTEMS_BDBUF_STATE_EMPTY)
    {
      rtems_bdbuf_remove_from_tree (bd);
      rtems_bdbuf_make_free_and_add_to_lru_list (bd);
    }
    rtems_bdbuf_wake (&bdbuf_cache.buffer_waiters);
  }
}

static rtems_bdbuf_buffer *
rtems_bdbuf_get_buffer_for_read_ahead (rtems_disk_device *dd,
                                       rtems_blkdev_bnum  block)
{
  rtems_bdbuf_buffer *bd = NULL;

  bd = rtems_bdbuf_avl_search (&bdbuf_cache.tree, dd, block);

  if (bd == NULL)
  {
    bd = rtems_bdbuf_get_buffer_from_lru_list (dd, block);

    if (bd != NULL)
      rtems_bdbuf_group_obtain (bd);
  }
  else
    /*
     * The buffer is in the cache.  So it is already available or in use, and
     * thus no need for a read ahead.
     */
    bd = NULL;

  return bd;
}

static rtems_bdbuf_buffer *
rtems_bdbuf_get_buffer_for_access (rtems_disk_device *dd,
                                   rtems_blkdev_bnum  block)
{
  rtems_bdbuf_buffer *bd = NULL;

  do
  {
    bd = rtems_bdbuf_avl_search (&bdbuf_cache.tree, dd, block);

    if (bd != NULL)
    {
      if (bd->group->bds_per_group != dd->bds_per_group)
      {
        if (rtems_bdbuf_wait_for_recycle (bd))
        {
          rtems_bdbuf_remove_from_tree_and_lru_list (bd);
          rtems_bdbuf_make_free_and_add_to_lru_list (bd);
          rtems_bdbuf_wake (&bdbuf_cache.buffer_waiters);
        }
        bd = NULL;
      }
    }
    else
    {
      bd = rtems_bdbuf_get_buffer_from_lru_list (dd, block);

      if (bd == NULL)
        rtems_bdbuf_wait_for_buffer ();
    }
  }
  while (bd == NULL);

  rtems_bdbuf_wait_for_access (bd);
  rtems_bdbuf_group_obtain (bd);

  return bd;
}

static rtems_status_code
rtems_bdbuf_get_media_block (const rtems_disk_device *dd,
                             rtems_blkdev_bnum        block,
                             rtems_blkdev_bnum       *media_block_ptr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (block < dd->block_count)
  {
    /*
     * Compute the media block number. Drivers work with media block number not
     * the block number a BD may have as this depends on the block size set by
     * the user.
     */
    *media_block_ptr = rtems_bdbuf_media_block (dd, block) + dd->start;
  }
  else
  {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code
rtems_bdbuf_get (rtems_disk_device   *dd,
                 rtems_blkdev_bnum    block,
                 rtems_bdbuf_buffer **bd_ptr)
{
  rtems_status_code   sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer *bd = NULL;
  rtems_blkdev_bnum   media_block;

  rtems_bdbuf_lock_cache ();

  sc = rtems_bdbuf_get_media_block (dd, block, &media_block);
  if (sc == RTEMS_SUCCESSFUL)
  {
    /*
     * Print the block index relative to the physical disk.
     */
    if (rtems_bdbuf_tracer)
      printf ("bdbuf:get: %" PRIu32 " (%" PRIu32 ") (dev = %08x)\n",
              media_block, block, (unsigned) dd->dev);

    bd = rtems_bdbuf_get_buffer_for_access (dd, media_block);

    switch (bd->state)
    {
      case RTEMS_BDBUF_STATE_CACHED:
        rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_ACCESS_CACHED);
        break;
      case RTEMS_BDBUF_STATE_EMPTY:
        rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_ACCESS_EMPTY);
        break;
      case RTEMS_BDBUF_STATE_MODIFIED:
        /*
         * To get a modified buffer could be considered a bug in the caller
         * because you should not be getting an already modified buffer but
         * user may have modified a byte in a block then decided to seek the
         * start and write the whole block and the file system will have no
         * record of this so just gets the block to fill.
         */
        rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_ACCESS_MODIFIED);
        break;
      default:
        rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_2);
        break;
    }

    if (rtems_bdbuf_tracer)
    {
      rtems_bdbuf_show_users ("get", bd);
      rtems_bdbuf_show_usage ();
    }
  }

  rtems_bdbuf_unlock_cache ();

  *bd_ptr = bd;

  return sc;
}

/**
 * Call back handler called by the low level driver when the transfer has
 * completed. This function may be invoked from interrupt handler.
 *
 * @param arg Arbitrary argument specified in block device request
 *            structure (in this case - pointer to the appropriate
 *            block device request structure).
 * @param status I/O completion status
 */
static void
rtems_bdbuf_transfer_done (void* arg, rtems_status_code status)
{
  rtems_blkdev_request* req = (rtems_blkdev_request*) arg;

  req->status = status;

  rtems_event_send (req->io_task, RTEMS_BDBUF_TRANSFER_SYNC);
}

static rtems_status_code
rtems_bdbuf_execute_transfer_request (rtems_disk_device    *dd,
                                      rtems_blkdev_request *req,
                                      bool                  cache_locked)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int result = 0;
  uint32_t transfer_index = 0;
  bool wake_transfer_waiters = false;
  bool wake_buffer_waiters = false;

  if (cache_locked)
    rtems_bdbuf_unlock_cache ();

  result = dd->ioctl (dd->phys_dev, RTEMS_BLKIO_REQUEST, req);

  if (result == 0)
  {
    rtems_bdbuf_wait_for_event (RTEMS_BDBUF_TRANSFER_SYNC);
    sc = req->status;
  }
  else
    sc = RTEMS_IO_ERROR;

  rtems_bdbuf_lock_cache ();

  /* Statistics */
  if (req->req == RTEMS_BLKDEV_REQ_READ)
  {
    dd->stats.read_blocks += req->bufnum;
    if (sc != RTEMS_SUCCESSFUL)
      ++dd->stats.read_errors;
  }
  else
  {
    dd->stats.write_blocks += req->bufnum;
    ++dd->stats.write_transfers;
    if (sc != RTEMS_SUCCESSFUL)
      ++dd->stats.write_errors;
  }

  for (transfer_index = 0; transfer_index < req->bufnum; ++transfer_index)
  {
    rtems_bdbuf_buffer *bd = req->bufs [transfer_index].user;
    bool waiters = bd->waiters;

    if (waiters)
      wake_transfer_waiters = true;
    else
      wake_buffer_waiters = true;

    rtems_bdbuf_group_release (bd);

    if (sc == RTEMS_SUCCESSFUL && bd->state == RTEMS_BDBUF_STATE_TRANSFER)
      rtems_bdbuf_make_cached_and_add_to_lru_list (bd);
    else
      rtems_bdbuf_discard_buffer (bd);

    if (rtems_bdbuf_tracer)
      rtems_bdbuf_show_users ("transfer", bd);
  }

  if (wake_transfer_waiters)
    rtems_bdbuf_wake (&bdbuf_cache.transfer_waiters);

  if (wake_buffer_waiters)
    rtems_bdbuf_wake (&bdbuf_cache.buffer_waiters);

  if (!cache_locked)
    rtems_bdbuf_unlock_cache ();

  if (sc == RTEMS_SUCCESSFUL || sc == RTEMS_UNSATISFIED)
    return sc;
  else
    return RTEMS_IO_ERROR;
}

static rtems_status_code
rtems_bdbuf_execute_read_request (rtems_disk_device  *dd,
                                  rtems_bdbuf_buffer *bd,
                                  uint32_t            transfer_count)
{
  rtems_blkdev_request *req = NULL;
  rtems_blkdev_bnum media_block = bd->block;
  uint32_t media_blocks_per_block = dd->media_blocks_per_block;
  uint32_t block_size = dd->block_size;
  uint32_t transfer_index = 1;

  /*
   * TODO: This type of request structure is wrong and should be removed.
   */
#define bdbuf_alloc(size) __builtin_alloca (size)

  req = bdbuf_alloc (sizeof (rtems_blkdev_request) +
                     sizeof (rtems_blkdev_sg_buffer) * transfer_count);

  req->req = RTEMS_BLKDEV_REQ_READ;
  req->req_done = rtems_bdbuf_transfer_done;
  req->done_arg = req;
  req->io_task = rtems_task_self ();
  req->status = RTEMS_RESOURCE_IN_USE;
  req->bufnum = 0;

  rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_TRANSFER);

  req->bufs [0].user   = bd;
  req->bufs [0].block  = media_block;
  req->bufs [0].length = block_size;
  req->bufs [0].buffer = bd->buffer;

  if (rtems_bdbuf_tracer)
    rtems_bdbuf_show_users ("read", bd);

  while (transfer_index < transfer_count)
  {
    media_block += media_blocks_per_block;

    bd = rtems_bdbuf_get_buffer_for_read_ahead (dd, media_block);

    if (bd == NULL)
      break;

    rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_TRANSFER);

    req->bufs [transfer_index].user   = bd;
    req->bufs [transfer_index].block  = media_block;
    req->bufs [transfer_index].length = block_size;
    req->bufs [transfer_index].buffer = bd->buffer;

    if (rtems_bdbuf_tracer)
      rtems_bdbuf_show_users ("read", bd);

    ++transfer_index;
  }

  req->bufnum = transfer_index;

  return rtems_bdbuf_execute_transfer_request (dd, req, true);
}

static bool
rtems_bdbuf_is_read_ahead_active (const rtems_disk_device *dd)
{
  return !rtems_chain_is_node_off_chain (&dd->read_ahead.node);
}

static void
rtems_bdbuf_read_ahead_cancel (rtems_disk_device *dd)
{
  if (rtems_bdbuf_is_read_ahead_active (dd))
  {
    rtems_chain_extract_unprotected (&dd->read_ahead.node);
    rtems_chain_set_off_chain (&dd->read_ahead.node);
  }
}

static void
rtems_bdbuf_read_ahead_reset (rtems_disk_device *dd)
{
  rtems_bdbuf_read_ahead_cancel (dd);
  dd->read_ahead.trigger = RTEMS_DISK_READ_AHEAD_NO_TRIGGER;
}

static void
rtems_bdbuf_check_read_ahead_trigger (rtems_disk_device *dd,
                                      rtems_blkdev_bnum  block)
{
  if (bdbuf_cache.read_ahead_task != 0
      && dd->read_ahead.trigger == block
      && !rtems_bdbuf_is_read_ahead_active (dd))
  {
    rtems_status_code sc;
    rtems_chain_control *chain = &bdbuf_cache.read_ahead_chain;

    if (rtems_chain_is_empty (chain))
    {
      sc = rtems_event_send (bdbuf_cache.read_ahead_task,
                             RTEMS_BDBUF_READ_AHEAD_WAKE_UP);
      if (sc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_RA_WAKE_UP);
    }

    rtems_chain_append_unprotected (chain, &dd->read_ahead.node);
  }
}

static void
rtems_bdbuf_set_read_ahead_trigger (rtems_disk_device *dd,
                                    rtems_blkdev_bnum  block)
{
  if (dd->read_ahead.trigger != block)
  {
    rtems_bdbuf_read_ahead_cancel (dd);
    dd->read_ahead.trigger = block + 1;
    dd->read_ahead.next = block + 2;
  }
}

rtems_status_code
rtems_bdbuf_read (rtems_disk_device   *dd,
                  rtems_blkdev_bnum    block,
                  rtems_bdbuf_buffer **bd_ptr)
{
  rtems_status_code     sc = RTEMS_SUCCESSFUL;
  rtems_bdbuf_buffer   *bd = NULL;
  rtems_blkdev_bnum     media_block;

  rtems_bdbuf_lock_cache ();

  sc = rtems_bdbuf_get_media_block (dd, block, &media_block);
  if (sc == RTEMS_SUCCESSFUL)
  {
    if (rtems_bdbuf_tracer)
      printf ("bdbuf:read: %" PRIu32 " (%" PRIu32 ") (dev = %08x)\n",
              media_block + dd->start, block, (unsigned) dd->dev);

    bd = rtems_bdbuf_get_buffer_for_access (dd, media_block);
    switch (bd->state)
    {
      case RTEMS_BDBUF_STATE_CACHED:
        ++dd->stats.read_hits;
        rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_ACCESS_CACHED);
        break;
      case RTEMS_BDBUF_STATE_MODIFIED:
        ++dd->stats.read_hits;
        rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_ACCESS_MODIFIED);
        break;
      case RTEMS_BDBUF_STATE_EMPTY:
        ++dd->stats.read_misses;
        rtems_bdbuf_set_read_ahead_trigger (dd, block);
        sc = rtems_bdbuf_execute_read_request (dd, bd, 1);
        if (sc == RTEMS_SUCCESSFUL)
        {
          rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_ACCESS_CACHED);
          rtems_chain_extract_unprotected (&bd->link);
          rtems_bdbuf_group_obtain (bd);
        }
        else
        {
          bd = NULL;
        }
        break;
      default:
        rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_4);
        break;
    }

    rtems_bdbuf_check_read_ahead_trigger (dd, block);
  }

  rtems_bdbuf_unlock_cache ();

  *bd_ptr = bd;

  return sc;
}

static rtems_status_code
rtems_bdbuf_check_bd_and_lock_cache (rtems_bdbuf_buffer *bd, const char *kind)
{
  if (bd == NULL)
    return RTEMS_INVALID_ADDRESS;
  if (rtems_bdbuf_tracer)
  {
    printf ("bdbuf:%s: %" PRIu32 "\n", kind, bd->block);
    rtems_bdbuf_show_users (kind, bd);
  }
  rtems_bdbuf_lock_cache();

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_bdbuf_release (rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_bdbuf_check_bd_and_lock_cache (bd, "release");
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  switch (bd->state)
  {
    case RTEMS_BDBUF_STATE_ACCESS_CACHED:
      rtems_bdbuf_add_to_lru_list_after_access (bd);
      break;
    case RTEMS_BDBUF_STATE_ACCESS_EMPTY:
    case RTEMS_BDBUF_STATE_ACCESS_PURGED:
      rtems_bdbuf_discard_buffer_after_access (bd);
      break;
    case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
      rtems_bdbuf_add_to_modified_list_after_access (bd);
      break;
    default:
      rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_0);
      break;
  }

  if (rtems_bdbuf_tracer)
    rtems_bdbuf_show_usage ();

  rtems_bdbuf_unlock_cache ();

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_bdbuf_release_modified (rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_bdbuf_check_bd_and_lock_cache (bd, "release modified");
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  switch (bd->state)
  {
    case RTEMS_BDBUF_STATE_ACCESS_CACHED:
    case RTEMS_BDBUF_STATE_ACCESS_EMPTY:
    case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
      rtems_bdbuf_add_to_modified_list_after_access (bd);
      break;
    case RTEMS_BDBUF_STATE_ACCESS_PURGED:
      rtems_bdbuf_discard_buffer_after_access (bd);
      break;
    default:
      rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_6);
      break;
  }

  if (rtems_bdbuf_tracer)
    rtems_bdbuf_show_usage ();

  rtems_bdbuf_unlock_cache ();

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_bdbuf_sync (rtems_bdbuf_buffer *bd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_bdbuf_check_bd_and_lock_cache (bd, "sync");
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  switch (bd->state)
  {
    case RTEMS_BDBUF_STATE_ACCESS_CACHED:
    case RTEMS_BDBUF_STATE_ACCESS_EMPTY:
    case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
      rtems_bdbuf_sync_after_access (bd);
      break;
    case RTEMS_BDBUF_STATE_ACCESS_PURGED:
      rtems_bdbuf_discard_buffer_after_access (bd);
      break;
    default:
      rtems_bdbuf_fatal (bd->state, RTEMS_BLKDEV_FATAL_BDBUF_STATE_5);
      break;
  }

  if (rtems_bdbuf_tracer)
    rtems_bdbuf_show_usage ();

  rtems_bdbuf_unlock_cache ();

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_bdbuf_syncdev (rtems_disk_device *dd)
{
  if (rtems_bdbuf_tracer)
    printf ("bdbuf:syncdev: %08x\n", (unsigned) dd->dev);

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
  bdbuf_cache.sync_device    = dd;

  rtems_bdbuf_wake_swapper ();
  rtems_bdbuf_unlock_cache ();
  rtems_bdbuf_wait_for_event (RTEMS_BDBUF_TRANSFER_SYNC);
  rtems_bdbuf_unlock_sync ();

  return RTEMS_SUCCESSFUL;
}

/**
 * Swapout transfer to the driver. The driver will break this I/O into groups
 * of consecutive write requests is multiple consecutive buffers are required
 * by the driver. The cache is not locked.
 *
 * @param transfer The transfer transaction.
 */
static void
rtems_bdbuf_swapout_write (rtems_bdbuf_swapout_transfer* transfer)
{
  rtems_chain_node *node;

  if (rtems_bdbuf_tracer)
    printf ("bdbuf:swapout transfer: %08x\n", (unsigned) transfer->dd->dev);

  /*
   * If there are buffers to transfer to the media transfer them.
   */
  if (!rtems_chain_is_empty (&transfer->bds))
  {
    /*
     * The last block number used when the driver only supports
     * continuous blocks in a single request.
     */
    uint32_t last_block = 0;

    rtems_disk_device *dd = transfer->dd;
    uint32_t media_blocks_per_block = dd->media_blocks_per_block;
    bool need_continuous_blocks =
      (dd->phys_dev->capabilities & RTEMS_BLKDEV_CAP_MULTISECTOR_CONT) != 0;

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
    transfer->write_req->bufnum = 0;

    while ((node = rtems_chain_get_unprotected(&transfer->bds)) != NULL)
    {
      rtems_bdbuf_buffer* bd = (rtems_bdbuf_buffer*) node;
      bool                write = false;

      /*
       * If the device only accepts sequential buffers and this is not the
       * first buffer (the first is always sequential, and the buffer is not
       * sequential then put the buffer back on the transfer chain and write
       * the committed buffers.
       */

      if (rtems_bdbuf_tracer)
        printf ("bdbuf:swapout write: bd:%" PRIu32 ", bufnum:%" PRIu32 " mode:%s\n",
                bd->block, transfer->write_req->bufnum,
                need_continuous_blocks ? "MULTI" : "SCAT");

      if (need_continuous_blocks && transfer->write_req->bufnum &&
          bd->block != last_block + media_blocks_per_block)
      {
        rtems_chain_prepend_unprotected (&transfer->bds, &bd->link);
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
          (transfer->write_req->bufnum >= bdbuf_config.max_write_blocks))
        write = true;

      if (write)
      {
        rtems_bdbuf_execute_transfer_request (dd, transfer->write_req, false);

        transfer->write_req->status = RTEMS_RESOURCE_IN_USE;
        transfer->write_req->bufnum = 0;
      }
    }

    /*
     * If sync'ing and the deivce is capability of handling a sync IO control
     * call perform the call.
     */
    if (transfer->syncing &&
        (dd->phys_dev->capabilities & RTEMS_BLKDEV_CAP_SYNC))
    {
      /* int result = */ dd->ioctl (dd->phys_dev, RTEMS_BLKDEV_REQ_SYNC, NULL);
      /* How should the error be handled ? */
    }
  }
}

/**
 * Process the modified list of buffers. There is a sync or modified list that
 * needs to be handled so we have a common function to do the work.
 *
 * @param dd_ptr Pointer to the device to handle. If BDBUF_INVALID_DEV no
 * device is selected so select the device of the first buffer to be written to
 * disk.
 * @param chain The modified chain to process.
 * @param transfer The chain to append buffers to be written too.
 * @param sync_active If true this is a sync operation so expire all timers.
 * @param update_timers If true update the timers.
 * @param timer_delta It update_timers is true update the timers by this
 *                    amount.
 */
static void
rtems_bdbuf_swapout_modified_processing (rtems_disk_device  **dd_ptr,
                                         rtems_chain_control* chain,
                                         rtems_chain_control* transfer,
                                         bool                 sync_active,
                                         bool                 update_timers,
                                         uint32_t             timer_delta)
{
  if (!rtems_chain_is_empty (chain))
  {
    rtems_chain_node* node = rtems_chain_head (chain);
    bool              sync_all;
    
    node = node->next;

    /*
     * A sync active with no valid dev means sync all.
     */
    if (sync_active && (*dd_ptr == BDBUF_INVALID_DEV))
      sync_all = true;
    else
      sync_all = false;
    
    while (!rtems_chain_is_tail (chain, node))
    {
      rtems_bdbuf_buffer* bd = (rtems_bdbuf_buffer*) node;

      /*
       * Check if the buffer's hold timer has reached 0. If a sync is active
       * or someone waits for a buffer written force all the timers to 0.
       *
       * @note Lots of sync requests will skew this timer. It should be based
       *       on TOD to be accurate. Does it matter ?
       */
      if (sync_all || (sync_active && (*dd_ptr == bd->dd))
          || rtems_bdbuf_has_buffer_waiters ())
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
       * This assumes we can set it to BDBUF_INVALID_DEV which is just an
       * assumption. Cannot use the transfer list being empty the sync dev
       * calls sets the dev to use.
       */
      if (*dd_ptr == BDBUF_INVALID_DEV)
        *dd_ptr = bd->dd;

      if (bd->dd == *dd_ptr)
      {
        rtems_chain_node* next_node = node->next;
        rtems_chain_node* tnode = rtems_chain_tail (transfer);

        /*
         * The blocks on the transfer list are sorted in block order. This
         * means multi-block transfers for drivers that require consecutive
         * blocks perform better with sorted blocks and for real disks it may
         * help lower head movement.
         */

        rtems_bdbuf_set_state (bd, RTEMS_BDBUF_STATE_TRANSFER);

        rtems_chain_extract_unprotected (node);

        tnode = tnode->previous;

        while (node && !rtems_chain_is_head (transfer, tnode))
        {
          rtems_bdbuf_buffer* tbd = (rtems_bdbuf_buffer*) tnode;

          if (bd->block > tbd->block)
          {
            rtems_chain_insert_unprotected (tnode, node);
            node = NULL;
          }
          else
            tnode = tnode->previous;
        }

        if (node)
          rtems_chain_prepend_unprotected (transfer, node);

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
      rtems_chain_get_unprotected (&bdbuf_cache.swapout_workers);
    if (worker)
      transfer = &worker->transfer;
  }

  rtems_chain_initialize_empty (&transfer->bds);
  transfer->dd = BDBUF_INVALID_DEV;
  transfer->syncing = bdbuf_cache.sync_active;
  
  /*
   * When the sync is for a device limit the sync to that device. If the sync
   * is for a buffer handle process the devices in the order on the sync
   * list. This means the dev is BDBUF_INVALID_DEV.
   */
  if (bdbuf_cache.sync_active)
    transfer->dd = bdbuf_cache.sync_device;
    
  /*
   * If we have any buffers in the sync queue move them to the modified
   * list. The first sync buffer will select the device we use.
   */
  rtems_bdbuf_swapout_modified_processing (&transfer->dd,
                                           &bdbuf_cache.sync,
                                           &transfer->bds,
                                           true, false,
                                           timer_delta);

  /*
   * Process the cache's modified list.
   */
  rtems_bdbuf_swapout_modified_processing (&transfer->dd,
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
            (bdbuf_config.max_write_blocks * sizeof (rtems_blkdev_sg_buffer)));

  if (!write_req)
    rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_NOMEM);

  write_req->req = RTEMS_BLKDEV_REQ_WRITE;
  write_req->req_done = rtems_bdbuf_transfer_done;
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
    rtems_bdbuf_wait_for_event (RTEMS_BDBUF_SWAPOUT_SYNC);

    rtems_bdbuf_swapout_write (&worker->transfer);

    rtems_bdbuf_lock_cache ();

    rtems_chain_initialize_empty (&worker->transfer.bds);
    worker->transfer.dd = BDBUF_INVALID_DEV;

    rtems_chain_append_unprotected (&bdbuf_cache.swapout_workers, &worker->link);

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
  size_t            w;

  rtems_bdbuf_lock_cache ();

  for (w = 0; w < bdbuf_config.swapout_workers; w++)
  {
    rtems_bdbuf_swapout_worker* worker;

    worker = malloc (sizeof (rtems_bdbuf_swapout_worker));
    if (!worker)
      rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_NOMEM);

    rtems_chain_append_unprotected (&bdbuf_cache.swapout_workers, &worker->link);
    worker->enabled = true;
    worker->transfer.write_req = rtems_bdbuf_swapout_writereq_alloc ();

    rtems_chain_initialize_empty (&worker->transfer.bds);
    worker->transfer.dd = BDBUF_INVALID_DEV;

    sc = rtems_bdbuf_create_task (rtems_build_name('B', 'D', 'o', 'a' + w),
                                  bdbuf_config.swapout_worker_priority,
                                  RTEMS_BDBUF_SWAPOUT_WORKER_TASK_PRIORITY_DEFAULT,
                                  rtems_bdbuf_swapout_worker_task,
                                  (rtems_task_argument) worker,
                                  &worker->id);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_SO_WK_CREATE);
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
  const uint32_t               period_in_msecs = bdbuf_config.swapout_period;
  uint32_t                     timer_delta;

  transfer.write_req = rtems_bdbuf_swapout_writereq_alloc ();
  rtems_chain_initialize_empty (&transfer.bds);
  transfer.dd = BDBUF_INVALID_DEV;
  transfer.syncing = false;

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

static void
rtems_bdbuf_purge_list (rtems_chain_control *purge_list)
{
  bool wake_buffer_waiters = false;
  rtems_chain_node *node = NULL;

  while ((node = rtems_chain_get_unprotected (purge_list)) != NULL)
  {
    rtems_bdbuf_buffer *bd = (rtems_bdbuf_buffer *) node;

    if (bd->waiters == 0)
      wake_buffer_waiters = true;

    rtems_bdbuf_discard_buffer (bd);
  }

  if (wake_buffer_waiters)
    rtems_bdbuf_wake (&bdbuf_cache.buffer_waiters);
}

static void
rtems_bdbuf_gather_for_purge (rtems_chain_control *purge_list,
                              const rtems_disk_device *dd)
{
  rtems_bdbuf_buffer *stack [RTEMS_BDBUF_AVL_MAX_HEIGHT];
  rtems_bdbuf_buffer **prev = stack;
  rtems_bdbuf_buffer *cur = bdbuf_cache.tree;

  *prev = NULL;

  while (cur != NULL)
  {
    if (cur->dd == dd)
    {
      switch (cur->state)
      {
        case RTEMS_BDBUF_STATE_FREE:
        case RTEMS_BDBUF_STATE_EMPTY:
        case RTEMS_BDBUF_STATE_ACCESS_PURGED:
        case RTEMS_BDBUF_STATE_TRANSFER_PURGED:
          break;
        case RTEMS_BDBUF_STATE_SYNC:
          rtems_bdbuf_wake (&bdbuf_cache.transfer_waiters);
          /* Fall through */
        case RTEMS_BDBUF_STATE_MODIFIED:
          rtems_bdbuf_group_release (cur);
          /* Fall through */
        case RTEMS_BDBUF_STATE_CACHED:
          rtems_chain_extract_unprotected (&cur->link);
          rtems_chain_append_unprotected (purge_list, &cur->link);
          break;
        case RTEMS_BDBUF_STATE_TRANSFER:
          rtems_bdbuf_set_state (cur, RTEMS_BDBUF_STATE_TRANSFER_PURGED);
          break;
        case RTEMS_BDBUF_STATE_ACCESS_CACHED:
        case RTEMS_BDBUF_STATE_ACCESS_EMPTY:
        case RTEMS_BDBUF_STATE_ACCESS_MODIFIED:
          rtems_bdbuf_set_state (cur, RTEMS_BDBUF_STATE_ACCESS_PURGED);
          break;
        default:
          rtems_fatal_error_occurred (RTEMS_BLKDEV_FATAL_BDBUF_STATE_11);
      }
    }

    if (cur->avl.left != NULL)
    {
      /* Left */
      ++prev;
      *prev = cur;
      cur = cur->avl.left;
    }
    else if (cur->avl.right != NULL)
    {
      /* Right */
      ++prev;
      *prev = cur;
      cur = cur->avl.right;
    }
    else
    {
      while (*prev != NULL
             && (cur == (*prev)->avl.right || (*prev)->avl.right == NULL))
      {
        /* Up */
        cur = *prev;
        --prev;
      }
      if (*prev != NULL)
        /* Right */
        cur = (*prev)->avl.right;
      else
        /* Finished */
        cur = NULL;
    }
  }
}

void
rtems_bdbuf_purge_dev (rtems_disk_device *dd)
{
  rtems_chain_control purge_list;

  rtems_chain_initialize_empty (&purge_list);
  rtems_bdbuf_lock_cache ();
  rtems_bdbuf_read_ahead_reset (dd);
  rtems_bdbuf_gather_for_purge (&purge_list, dd);
  rtems_bdbuf_purge_list (&purge_list);
  rtems_bdbuf_unlock_cache ();
}

rtems_status_code
rtems_bdbuf_set_block_size (rtems_disk_device *dd, uint32_t block_size)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  rtems_bdbuf_lock_cache ();

  if (block_size > 0)
  {
    size_t bds_per_group = rtems_bdbuf_bds_per_group (block_size);

    if (bds_per_group != 0)
    {
      int block_to_media_block_shift = 0;
      uint32_t media_blocks_per_block = block_size / dd->media_block_size;
      uint32_t one = 1;

      while ((one << block_to_media_block_shift) < media_blocks_per_block)
      {
        ++block_to_media_block_shift;
      }

      if ((dd->media_block_size << block_to_media_block_shift) != block_size)
        block_to_media_block_shift = -1;

      dd->block_size = block_size;
      dd->block_count = dd->size / media_blocks_per_block;
      dd->media_blocks_per_block = media_blocks_per_block;
      dd->block_to_media_block_shift = block_to_media_block_shift;
      dd->bds_per_group = bds_per_group;

      rtems_bdbuf_read_ahead_reset (dd);
    }
    else
    {
      sc = RTEMS_INVALID_NUMBER;
    }
  }
  else
  {
    sc = RTEMS_INVALID_NUMBER;
  }

  rtems_bdbuf_unlock_cache ();

  return sc;
}

static rtems_task
rtems_bdbuf_read_ahead_task (rtems_task_argument arg)
{
  rtems_chain_control *chain = &bdbuf_cache.read_ahead_chain;

  while (bdbuf_cache.read_ahead_enabled)
  {
    rtems_chain_node *node;

    rtems_bdbuf_wait_for_event (RTEMS_BDBUF_READ_AHEAD_WAKE_UP);
    rtems_bdbuf_lock_cache ();

    while ((node = rtems_chain_get_unprotected (chain)) != NULL)
    {
      rtems_disk_device *dd = (rtems_disk_device *)
        ((char *) node - offsetof (rtems_disk_device, read_ahead.node));
      rtems_blkdev_bnum block = dd->read_ahead.next;
      rtems_blkdev_bnum media_block = 0;
      rtems_status_code sc =
        rtems_bdbuf_get_media_block (dd, block, &media_block);

      rtems_chain_set_off_chain (&dd->read_ahead.node);

      if (sc == RTEMS_SUCCESSFUL)
      {
        rtems_bdbuf_buffer *bd =
          rtems_bdbuf_get_buffer_for_read_ahead (dd, media_block);

        if (bd != NULL)
        {
          uint32_t transfer_count = dd->block_count - block;
          uint32_t max_transfer_count = bdbuf_config.max_read_ahead_blocks;

          if (transfer_count >= max_transfer_count)
          {
            transfer_count = max_transfer_count;
            dd->read_ahead.trigger = block + transfer_count / 2;
            dd->read_ahead.next = block + transfer_count;
          }
          else
          {
            dd->read_ahead.trigger = RTEMS_DISK_READ_AHEAD_NO_TRIGGER;
          }

          ++dd->stats.read_ahead_transfers;
          rtems_bdbuf_execute_read_request (dd, bd, transfer_count);
        }
      }
      else
      {
        dd->read_ahead.trigger = RTEMS_DISK_READ_AHEAD_NO_TRIGGER;
      }
    }

    rtems_bdbuf_unlock_cache ();
  }

  rtems_task_delete (RTEMS_SELF);
}

void rtems_bdbuf_get_device_stats (const rtems_disk_device *dd,
                                   rtems_blkdev_stats      *stats)
{
  rtems_bdbuf_lock_cache ();
  *stats = dd->stats;
  rtems_bdbuf_unlock_cache ();
}

void rtems_bdbuf_reset_device_stats (rtems_disk_device *dd)
{
  rtems_bdbuf_lock_cache ();
  memset (&dd->stats, 0, sizeof(dd->stats));
  rtems_bdbuf_unlock_cache ();
}
