/**
 * @file rtems/bdbuf.h
 *
 * Block Device Buffer Management
 */
 
/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * Copyright (C) 2008 Chris Johns <chrisj@rtems.org>
 *    Rewritten to remove score mutex access. Fixes many performance
 *    issues.
 *
 * @(#) bdbuf.h,v 1.9 2005/02/02 00:06:18 joel Exp
 */

#ifndef _RTEMS_BDBUF_H
#define _RTEMS_BDBUF_H

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/chain.h>

#include <rtems/blkdev.h>
#include <rtems/diskdevs.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * State of a buffer in the cache.
 */
typedef enum
{
  RTEMS_BDBUF_STATE_EMPTY = 0,            /*< Not in use. */
  RTEMS_BDBUF_STATE_READ_AHEAD = 1,       /*< Holds read ahead data only */
  RTEMS_BDBUF_STATE_CACHED = 2,           /*< In the cache and available */
  RTEMS_BDBUF_STATE_ACCESS = 3,           /*< The user has the buffer */
  RTEMS_BDBUF_STATE_MODIFIED = 4,         /*< In the cache but modified */
  RTEMS_BDBUF_STATE_ACCESS_MODIFIED = 5,  /*< With the user but modified */
  RTEMS_BDBUF_STATE_SYNC = 6,             /*< Requested to be sync'ed */
  RTEMS_BDBUF_STATE_TRANSFER = 7          /*< Being transferred to or from disk */
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
    signed char                cache;  /*< Cache */
    struct rtems_bdbuf_buffer* left;   /*< Left Child */
    struct rtems_bdbuf_buffer* right;  /*< Right Child */
    signed char                bal;    /*< The balance of the sub-tree */
  } avl;

  dev_t             dev;        /*< device number */
  rtems_blkdev_bnum block;      /*< block number on the device */

  unsigned char*    buffer;     /*< Pointer to the buffer memory area */
  int               error;      /*< If not 0 indicate an error value (errno)
                                 * which can be used by user later */

  volatile rtems_bdbuf_buf_state state;  /*< State of the buffer. */

  volatile uint32_t waiters;    /*< The number of threads waiting on this
                                 * buffer. */
  rtems_bdpool_id pool;         /*< Identifier of buffer pool to which this buffer
                                    belongs */

  volatile uint32_t hold_timer; /*< Timer to indicate how long a buffer
                                 * has been held in the cache modified. */
} rtems_bdbuf_buffer;

/**
 * The groups of the blocks with the same size are collected in a pool. Note
 * that a several of the buffer's groups with the same size can exists.
 */
typedef struct rtems_bdbuf_pool
{
  uint32_t            blksize;           /*< The size of the blocks (in bytes) */
  uint32_t            nblks;             /*< Number of blocks in this pool */

  uint32_t            flags;             /*< Configuration flags */

  rtems_id            lock;              /*< The pool lock. Lock this data and
                                          * all BDs. */
  rtems_id            sync_lock;         /*< Sync calls lock writes. */
  bool                sync_active;       /*< True if a sync is active. */
  rtems_id            sync_requester;    /*< The sync requester. */
  dev_t               sync_device;       /*< The device to sync */

  rtems_bdbuf_buffer* tree;             /*< Buffer descriptor lookup AVL tree
                                         * root */
  rtems_chain_control ready;            /*< Free buffers list (or read-ahead) */
  rtems_chain_control lru;              /*< Last recently used list */
  rtems_chain_control modified;         /*< Modified buffers list */
  rtems_chain_control sync;             /*< Buffers to sync list */

  rtems_id            access;           /*< Obtain if waiting for a buffer in the
                                         * ACCESS state. */
  volatile uint32_t   access_waiters;   /*< Count of access blockers. */
  rtems_id            transfer;         /*< Obtain if waiting for a buffer in the
                                         * TRANSFER state. */
  volatile uint32_t   transfer_waiters; /*< Count of transfer blockers. */
  rtems_id            waiting;          /*< Obtain if waiting for a buffer and the
                                         * none are available. */
  volatile uint32_t   wait_waiters;     /*< Count of waiting blockers. */

  rtems_bdbuf_buffer* bds;              /*< Pointer to table of buffer descriptors
                                         * allocated for this buffer pool. */
  void*               buffers;          /*< The buffer's memory. */
} rtems_bdbuf_pool;

/**
 * Configuration structure describes block configuration (size, amount, memory
 * location) for buffering layer pool.
 */
typedef struct rtems_bdbuf_pool_config {
  int            size;      /*< Size of block */
  int            num;       /*< Number of blocks of appropriate size */
  unsigned char* mem_area;  /*< Pointer to the blocks location or NULL, in this
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
  uint32_t            max_read_ahead_blocks; /*< Number of blocks to read ahead. */
  uint32_t            max_write_blocks;      /*< Number of blocks to write at once. */
  rtems_task_priority swapout_priority;      /*< Priority of the swap out task. */
  uint32_t            swapout_period;        /*< Period swapout checks buf timers. */
  uint32_t            swap_block_hold;       /*< Period a buffer is held. */
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
rtems_bdbuf_init (void);

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
rtems_bdbuf_get (dev_t device, rtems_blkdev_bnum block, rtems_bdbuf_buffer** bd);

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
 * @param device Device number (constructed of major and minor device number)
 * @param block  Linear media block number
 * @param bd     Reference to the buffer descriptor pointer.
 *
 * @return       RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *               successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_read (dev_t device, rtems_blkdev_bnum block, rtems_bdbuf_buffer** bd);

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
rtems_bdbuf_release (rtems_bdbuf_buffer* bd);

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
rtems_bdbuf_release_modified (rtems_bdbuf_buffer* bd);

/**
 * Release the buffer as modified and wait until it has been synchronized with
 * the disk by writing it. This buffer will be the first to be transfer to disk
 * and other buffers may also be written if the maximum number of blocks in a
 * requests allows it.
 *
 * @note This code does not lock the sync mutex and stop additions to the
 *       modified queue.

 * @param bd Reference to the buffer descriptor.
 *
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_sync (rtems_bdbuf_buffer* bd);

/**
 * Synchronize all modified buffers for this device with the disk and wait
 * until the transfers have completed. The sync mutex for the pool is locked
 * stopping the addition of any further modifed buffers. It is only the
 * currently modified buffers that are written.
 *
 * @note Nesting calls to sync multiple devices attached to a single pool will
 * be handled sequentially. A nested call will be blocked until the first sync
 * request has complete. This is only true for device using the same pool.
 *
 * @param dev Block device number
 *
 * @return RTEMS status code (RTEMS_SUCCESSFUL if operation completed
 *         successfully or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_syncdev (dev_t dev);

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
rtems_bdbuf_find_pool (uint32_t block_size, rtems_bdpool_id *pool);

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
rtems_bdbuf_get_pool_info (rtems_bdpool_id pool, int *block_size, int *blocks);

#ifdef __cplusplus
}
#endif

#endif
