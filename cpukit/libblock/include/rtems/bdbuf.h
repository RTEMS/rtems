/**
 * @file rtems/bdbuf.h
 *
 * block device buffer management
 */
 
/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) bdbuf.h,v 1.9 2005/02/02 00:06:18 joel Exp
 */

#ifndef _RTEMS_BDBUF_H
#define _RTEMS_BDBUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/chain.h>

#include "rtems/blkdev.h"
#include "rtems/diskdevs.h"

/**
 * State of a buffer in the cache.
 */
typedef enum
{
  RTEMS_BDBUF_STATE_EMPTY = 0,            /* Not in use. */
  RTEMS_BDBUF_STATE_READ_AHEAD = 1,       /* Holds read ahead data only */
  RTEMS_BDBUF_STATE_CACHED = 2,           /* In the cache and available */
  RTEMS_BDBUF_STATE_ACCESS = 3,           /* The user has the buffer */
  RTEMS_BDBUF_STATE_MODIFIED = 4,         /* In the cache but modified */
  RTEMS_BDBUF_STATE_ACCESS_MODIFIED = 5,  /* With the user but modified */
  RTEMS_BDBUF_STATE_SYNC = 6,             /* Requested to be sync'ed */
  RTEMS_BDBUF_STATE_TRANSFER = 7          /* Being transferred to or from disk */
} rtems_bdbuf_buf_state;

/**
 * To manage buffers we using buffer descriptors (BD). A BD holds a buffer plus
 * a range of other information related to managing the buffer in the cache. To
 * speed-up buffer lookup descriptors are organized in AVL-Tree.  The fields
 * 'dev' and 'block' are search keys.
 */
typedef struct rtems_bdbuf_buffer
{
  rtems_chain_node link;       /* Link in the BD onto a number of lists. */

  struct rtems_bdbuf_avl_node
  {
    signed char                cache;  /* Cache */
    struct rtems_bdbuf_buffer* left;   /* Left Child */
    struct rtems_bdbuf_buffer* right;  /* Right Child */
    signed char                bal;    /* The balance of the sub-tree */
  } avl;

  dev_t             dev;        /* device number */
  rtems_blkdev_bnum block;      /* block number on the device */

  unsigned char*    buffer;     /* Pointer to the buffer memory area */
  int               error;      /* If not 0 indicate an error value (errno)
                                 * which can be used by user later */

  volatile rtems_bdbuf_buf_state state;  /* State of the buffer. */

  volatile uint32_t waiters;    /* The number of threads waiting on this
                                 * buffer. */
  rtems_bdpool_id pool;         /* Identifier of buffer pool to which this buffer
                                    belongs */

  volatile uint32_t hold_timer; /* Timer to indicate how long a buffer
                                 * has been held in the cache modified. */
} rtems_bdbuf_buffer;

/**
 * The groups of the blocks with the same size are collected in a pool. Note
 * that a several of the buffer's groups with the same size can exists.
 */
typedef struct rtems_bdbuf_pool
{
  int                 blksize;           /* The size of the blocks (in bytes) */
  int                 nblks;             /* Number of blocks in this pool */

  uint32_t            flags;             /* Configuration flags */

  rtems_id            lock;              /* The pool lock. Lock this data and
                                          * all BDs. */
  rtems_id            sync_lock;         /* Sync calls lock writes. */
  boolean             sync_active;       /* True if a sync is active. */
  rtems_id            sync_requester;    /* The sync requester. */
  dev_t               sync_device;       /* The device to sync */

  rtems_bdbuf_buffer* tree;             /* Buffer descriptor lookup AVL tree
                                         * root */
  rtems_chain_control ready;            /* Free buffers list (or read-ahead) */
  rtems_chain_control lru;              /* Last recently used list */
  rtems_chain_control modified;         /* Modified buffers list */
  rtems_chain_control sync;             /* Buffers to sync list */

  rtems_id            access;           /* Obtain if waiting for a buffer in the
                                         * ACCESS state. */
  volatile uint32_t   access_waiters;   /* Count of access blockers. */
  rtems_id            transfer;         /* Obtain if waiting for a buffer in the
                                         * TRANSFER state. */
  volatile uint32_t   transfer_waiters; /* Count of transfer blockers. */
  rtems_id            waiting;          /* Obtain if waiting for a buffer and the
                                         * none are available. */
  volatile uint32_t   wait_waiters;     /* Count of waiting blockers. */

  rtems_bdbuf_buffer* bds;              /* Pointer to table of buffer descriptors
                                         * allocated for this buffer pool. */
  void*               buffers;          /* The buffer's memory. */
} rtems_bdbuf_pool;

/**
 * Configuration structure describes block configuration (size, amount, memory
 * location) for buffering layer pool.
 */
typedef struct rtems_bdbuf_pool_config {
  int            size;      /* Size of block */
  int            num;       /* Number of blocks of appropriate size */
  unsigned char* mem_area;  /* Pointer to the blocks location or NULL, in this
                             * case memory for blocks will be allocated by
                             * Buffering Layer with the help of RTEMS partition
                             * manager */
} rtems_bdbuf_pool_config;

/**
 * External references provided by the user for each pool in the system.
 */
extern rtems_bdbuf_pool_config rtems_bdbuf_pool_configuration[];
extern int                     rtems_bdbuf_pool_configuration_size;

/**
 * Buffering configuration definition. See confdefs.h for support on using this
 * structure.
 */
typedef struct rtems_bdbuf_config {
  int                 max_read_ahead_blocks; /*<< Number of blocks to read ahead. */
  int                 max_write_blocks;      /*<< Number of blocks to write at once. */
  rtems_task_priority swapout_priority;      /*<< Priority of the swap out task. */
  uint32_t            swapout_period;        /*<< Period swapout checks buf timers. */
  uint32_t            swap_block_hold;       /*<< Period a buffer is held. */
} rtems_bdbuf_config;

/**
 * External referernce to the configuration. The configuration is provided by
 * the user.
 */
extern rtems_bdbuf_config rtems_bdbuf_configuration;

/**
 * The max_read_ahead_blocks value is altered if there are fewer buffers
 * than this defined max. This stops thrashing in the cache.
 */
#define RTEMS_BDBUF_MAX_READ_AHEAD_BLOCKS_DEFAULT    32
#define RTEMS_BDBUF_MAX_WRITE_BLOCKS_DEFAULT         16
#define RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT    15
#define RTEMS_BDBUF_SWAPOUT_TASK_SWAP_PERIOD_DEFAULT 250  /* milli-seconds */
#define RTEMS_BDBUF_SWAPOUT_TASK_BLOCK_HOLD_DEFAULT  1000 /* milli-seconds */

/* rtems_bdbuf_init --
 *     Prepare buffering layer to work - initialize buffer descritors
 *     and (if it is neccessary) buffers. Buffers will be allocated accoriding
 *     to the configuration table, each entry describes kind of block and
 *     amount requested. After initialization all blocks is placed into
 *     free elements lists.
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_init ();

/* rtems_bdbuf_get --
 *     Obtain block buffer. If specified block already cached (i.e. there's
 *     block in the _modified_, or _recently_used_), return address
 *     of appropriate buffer descriptor and increment reference counter to 1.
 *     If block is not cached, allocate new buffer and return it. Data
 *     shouldn't be read to the buffer from media; buffer may contains
 *     arbitrary data. This primitive may be blocked if there are no free
 *     buffer descriptors available and there are no unused non-modified
 *     (or synchronized with media) buffers available.
 *
 * PARAMETERS:
 *     device - device number (constructed of major and minor device number)
 *     block  - linear media block number
 *     bd     - address of variable to store pointer to the buffer descriptor
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     bufget_sema semaphore obtained by this primitive.
 */
  rtems_status_code
  rtems_bdbuf_get(dev_t device, rtems_blkdev_bnum block, rtems_bdbuf_buffer** bd);

/* rtems_bdbuf_read --
 *     (Similar to the rtems_bdbuf_get, except reading data from media)
 *     Obtain block buffer. If specified block already cached, return address
 *     of appropriate buffer and increment reference counter to 1. If block is
 *     not cached, allocate new buffer and read data to it from the media.
 *     This primitive may be blocked on waiting until data to be read from
 *     media, if there are no free buffer descriptors available and there are
 *     no unused non-modified (or synchronized with media) buffers available.
 *
 * PARAMETERS:
 *     device - device number (consists of major and minor device number)
 *     block  - linear media block number
 *     bd     - address of variable to store pointer to the buffer descriptor
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     bufget_sema and transfer_sema semaphores obtained by this primitive.
 */
  rtems_status_code
  rtems_bdbuf_read(dev_t device, rtems_blkdev_bnum block, rtems_bdbuf_buffer** bd);

/* rtems_bdbuf_release --
 *     Release buffer allocated before. This primitive decrease the
 *     usage counter. If it is zero, further destiny of buffer depends on
 *     'modified' status. If buffer was modified, it is placed to the end of
 *     mod list and flush task waken up. If buffer was not modified,
 *     it is placed to the end of lru list, and bufget_sema released, allowing
 *     to reuse this buffer.
 *
 * PARAMETERS:
 *     bd_buf - pointer to the bdbuf_buffer structure previously obtained using
 *              get/read primitive.
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     flush_sema and bufget_sema semaphores may be released by this primitive.
 */
  rtems_status_code
  rtems_bdbuf_release(rtems_bdbuf_buffer* bd);

/* rtems_bdbuf_release_modified --
 *     Release buffer allocated before, assuming that it is _modified_ by
 *     it's owner. This primitive decrease usage counter for buffer, mark
 *     buffer descriptor as modified. If usage counter is 0, insert it at
 *     end of mod chain and release flush_sema semaphore to activate the
 *     flush task.
 *
 * PARAMETERS:
 *     bd_buf - pointer to the bdbuf_buffer structure previously obtained using
 *              get/read primitive.
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     flush_sema semaphore may be released by this primitive.
 */
  rtems_status_code
  rtems_bdbuf_release_modified(rtems_bdbuf_buffer* bd);

/* rtems_bdbuf_sync --
 *     Wait until specified buffer synchronized with disk. Invoked on exchanges
 *     critical for data consistency on the media. This primitive mark owned
 *     block as modified, decrease usage counter. If usage counter is 0,
 *     block inserted to the mod chain and flush_sema semaphore released.
 *     Finally, primitives blocked on transfer_sema semaphore.
 *
 * PARAMETERS:
 *     bd_buf - pointer to the bdbuf_buffer structure previously obtained using
 *              get/read primitive.
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     Primitive may be blocked on transfer_sema semaphore.
 */
  rtems_status_code
  rtems_bdbuf_sync(rtems_bdbuf_buffer* bd);

/* rtems_bdbuf_syncdev --
 *     Synchronize with disk all buffers containing the blocks belonging to
 *     specified device.
 *
 * PARAMETERS:
 *     dev - block device number
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 */
  rtems_status_code
  rtems_bdbuf_syncdev(dev_t dev);

/* rtems_bdbuf_find_pool --
 *     Find first appropriate buffer pool. This primitive returns the index
 *     of first buffer pool which block size is greater than or equal to
 *     specified size.
 *
 * PARAMETERS:
 *     block_size - requested block size
 *     pool       - placeholder for result
 *
 * RETURNS:
 *     RTEMS status code: RTEMS_SUCCESSFUL if operation completed successfully,
 *     RTEMS_INVALID_SIZE if specified block size is invalid (not a power
 *     of 2), RTEMS_NOT_DEFINED if buffer pool for this or greater block size
 *     is not configured.
 */
  rtems_status_code
  rtems_bdbuf_find_pool(int block_size, rtems_bdpool_id *pool);

/* rtems_bdbuf_get_pool_info --
 *     Obtain characteristics of buffer pool with specified number.
 *
 * PARAMETERS:
 *     pool       - buffer pool number
 *     block_size - block size for which buffer pool is configured returned
 *                  there
 *     blocks     - number of buffers in buffer pool returned there
 *
 * RETURNS:
 *     RTEMS status code: RTEMS_SUCCESSFUL if operation completed successfully,
 *     RTEMS_INVALID_NUMBER if appropriate buffer pool is not configured.
 *
 * NOTE:
 *     Buffer pools enumerated contiguously starting from 0.
 */
  rtems_status_code
  rtems_bdbuf_get_pool_info(rtems_bdpool_id pool, int *block_size, int *blocks);

#ifdef __cplusplus
}
#endif

#endif
