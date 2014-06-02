/**
 * @file
 *
 * @ingroup rtems_bdbuf
 * @brief Block Device Buffer Management
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * Copyright (C) 2008,2009 Chris Johns <chrisj@rtems.org>
 *    Rewritten to remove score mutex access. Fixes many performance
 *    issues.
 *    Change to support demand driven variable buffer sizes.
 *
 * Copyright (c) 2009-2012 embedded brains GmbH.
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
 * @defgroup rtems_libblock Block Device Library
 *
 * Block device modules.
 */

/**
 * @defgroup rtems_bdbuf Block Device Buffer Management
 *
 * @ingroup rtems_libblock
 *
 * The Block Device Buffer Management implements a cache between the disk
 * devices and file systems.  The code provides read-ahead and write queuing to
 * the drivers and fast cache look-up using an AVL tree.
 *
 * The block size used by a file system can be set at runtime and must be a
 * multiple of the disk device block size.  The disk device's physical block
 * size is called the media block size.  The file system can set the block size
 * it uses to a larger multiple of the media block size.  The driver must be
 * able to handle buffers sizes larger than one media block.
 *
 * The user configures the amount of memory to be used as buffers in the cache,
 * and the minimum and maximum buffer size.  The cache will allocate additional
 * memory for the buffer descriptors and groups.  There are enough buffer
 * descriptors allocated so all the buffer memory can be used as minimum sized
 * buffers.
 *
 * The cache is a single pool of buffers.  The buffer memory is divided into
 * groups where the size of buffer memory allocated to a group is the maximum
 * buffer size.  A group's memory can be divided down into small buffer sizes
 * that are a multiple of 2 of the minimum buffer size.  A group is the minimum
 * allocation unit for buffers of a specific size.  If a buffer of maximum size
 * is request the group will have a single buffer.  If a buffer of minimum size
 * is requested the group is divided into minimum sized buffers and the
 * remaining buffers are held ready for use.  A group keeps track of which
 * buffers are with a file system or driver and groups who have buffer in use
 * cannot be realloced.  Groups with no buffers in use can be taken and
 * realloced to a new size.  This is how buffers of different sizes move around
 * the cache.

 * The buffers are held in various lists in the cache.  All buffers follow this
 * state machine:
 *
 * @dot
 * digraph state {
 *   size="16,8";
 *   f [label="FREE",style="filled",fillcolor="aquamarine"];
 *   e [label="EMPTY",style="filled",fillcolor="seagreen"];
 *   c [label="CACHED",style="filled",fillcolor="chartreuse"];
 *   ac [label="ACCESS CACHED",style="filled",fillcolor="royalblue"];
 *   am [label="ACCESS MODIFIED",style="filled",fillcolor="royalblue"];
 *   ae [label="ACCESS EMPTY",style="filled",fillcolor="royalblue"];
 *   ap [label="ACCESS PURGED",style="filled",fillcolor="royalblue"];
 *   t [label="TRANSFER",style="filled",fillcolor="red"];
 *   tp [label="TRANSFER PURGED",style="filled",fillcolor="red"];
 *   s [label="SYNC",style="filled",fillcolor="red"];
 *   m [label="MODIFIED",style="filled",fillcolor="gold"];
 *   i [label="INITIAL"];
 *
 *   legend_transfer [label="Transfer Wake-Up",fontcolor="red",shape="none"];
 *   legend_access [label="Access Wake-Up",fontcolor="royalblue",shape="none"];
 *
 *   i -> f [label="Init"];
 *   f -> e [label="Buffer Recycle"];
 *   e -> ae [label="Get"];
 *   e -> t [label="Read"];
 *   e -> f [label="Nobody Waits"];
 *   c -> ac [label="Get\nRead"];
 *   c -> e [label="Buffer Recycle\nPurge"];
 *   c -> f [label="Reallocate\nBlock Size Changed"];
 *   t -> c [label="Transfer Done",color="red",fontcolor="red"];
 *   t -> e [label="Transfer Error",color="red",fontcolor="red"];
 *   t -> tp [label="Purge"];
 *   tp -> e [label="Transfer Done\nTransfer Error",color="red",fontcolor="red"];
 *   m -> t [label="Swapout"];
 *   m -> s [label="Block Size Changed"];
 *   m -> am [label="Get\nRead"];
 *   m -> e [label="Purge"];
 *   ac -> m [label="Release Modified",color="royalblue",fontcolor="royalblue"];
 *   ac -> s [label="Sync",color="royalblue",fontcolor="royalblue"];
 *   ac -> c [label="Release",color="royalblue",fontcolor="royalblue"];
 *   ac -> ap [label="Purge"];
 *   am -> m [label="Release\nRelease Modified",color="royalblue",fontcolor="royalblue"];
 *   am -> s [label="Sync",color="royalblue",fontcolor="royalblue"];
 *   am -> ap [label="Purge"];
 *   ae -> m [label="Release Modified",color="royalblue",fontcolor="royalblue"];
 *   ae -> s [label="Sync",color="royalblue",fontcolor="royalblue"];
 *   ae -> e [label="Release",color="royalblue",fontcolor="royalblue"];
 *   ae -> ap [label="Purge"];
 *   ap -> e [label="Release\nRelease Modified\nSync",color="royalblue",fontcolor="royalblue"];
 *   s -> t [label="Swapout"];
 *   s -> e [label="Purge",color="red",fontcolor="red"];
 * }
 * @enddot
 *
 * Empty or cached buffers are added to the LRU list and removed from this
 * queue when a caller requests a buffer.  This is referred to as getting a
 * buffer in the code and the event get in the state diagram.  The buffer is
 * assigned to a block and inserted to the AVL based on the block/device key.
 * If the block is to be read by the user and not in the cache it is transfered
 * from the disk into memory.  If no buffers are on the LRU list the modified
 * list is checked.  If buffers are on the modified the swap out task will be
 * woken.  The request blocks until a buffer is available for recycle.
 *
 * A block being accessed is given to the file system layer and not accessible
 * to another requester until released back to the cache.  The same goes to a
 * buffer in the transfer state.  The transfer state means being read or
 * written.  If the file system has modified the block and releases it as
 * modified it placed on the cache's modified list and a hold timer
 * initialised.  The buffer is held for the hold time before being written to
 * disk.  Buffers are held for a configurable period of time on the modified
 * list as a write sets the state to transfer and this locks the buffer out
 * from the file system until the write completes.  Buffers are often accessed
 * and modified in a series of small updates so if sent to the disk when
 * released as modified the user would have to block waiting until it had been
 * written.  This would be a performance problem.
 *
 * The code performs multiple block reads and writes.  Multiple block reads or
 * read-ahead increases performance with hardware that supports it.  It also
 * helps with a large cache as the disk head movement is reduced.  It however
 * is a speculative operation so excessive use can remove valuable and needed
 * blocks from the cache.  The read-ahead is triggered after two misses of
 * ascending consecutive blocks or a read hit of a block read by the
 * most-resent read-ahead transfer.  The read-ahead works per disk, but all
 * transfers are issued by the read-ahead task.
 *
 * The cache has the following lists of buffers:
 *  - LRU: Accessed or transfered buffers released in least recently used
 *  order.  Empty buffers will be placed to the front.
 *  - Modified: Buffers waiting to be written to disk.
 *  - Sync: Buffers to be synchronized with the disk.
 *
 * A cache look-up will be performed to find a suitable buffer.  A suitable
 * buffer is one that matches the same allocation size as the device the buffer
 * is for.  The a buffer's group has no buffers in use with the file system or
 * driver the group is reallocated.  This means the buffers in the group are
 * invalidated, resized and placed on the LRU queue.  There is a performance
 * issue with this design.  The reallocation of a group may forced recently
 * accessed buffers out of the cache when they should not.  The design should be
 * change to have groups on a LRU list if they have no buffers in use.
 */
/**@{**/

#if defined(RTEMS_POSIX_API)
  /*
   * Use the PTHREAD mutexes and condition variables if available.  This helps
   * on SMP configurations to avoid the home grown condition variables via
   * disabled preemption.
   */
  #define RTEMS_BDBUF_USE_PTHREAD
#endif

/**
 * @brief State of a buffer of the cache.
 *
 * The state has several implications.  Depending on the state a buffer can be
 * in the AVL tree, in a list, in use by an entity and a group user or not.
 *
 * <table>
 *   <tr>
 *     <th>State</th><th>Valid Data</th><th>AVL Tree</th>
 *     <th>LRU List</th><th>Modified List</th><th>Synchronization List</th>
 *     <th>Group User</th><th>External User</th>
 *   </tr>
 *   <tr>
 *     <td>FREE</td><td></td><td></td>
 *     <td>X</td><td></td><td></td><td></td><td></td>
 *   </tr>
 *   <tr>
 *     <td>EMPTY</td><td></td><td>X</td>
 *     <td></td><td></td><td></td><td></td><td></td>
 *   </tr>
 *   <tr>
 *     <td>CACHED</td><td>X</td><td>X</td>
 *     <td>X</td><td></td><td></td><td></td><td></td>
 *   </tr>
 *   <tr>
 *     <td>ACCESS CACHED</td><td>X</td><td>X</td>
 *     <td></td><td></td><td></td><td>X</td><td>X</td>
 *   </tr>
 *   <tr>
 *     <td>ACCESS MODIFIED</td><td>X</td><td>X</td>
 *     <td></td><td></td><td></td><td>X</td><td>X</td>
 *   </tr>
 *   <tr>
 *     <td>ACCESS EMPTY</td><td></td><td>X</td>
 *     <td></td><td></td><td></td><td>X</td><td>X</td>
 *   </tr>
 *   <tr>
 *     <td>ACCESS PURGED</td><td></td><td>X</td>
 *     <td></td><td></td><td></td><td>X</td><td>X</td>
 *   </tr>
 *   <tr>
 *     <td>MODIFIED</td><td>X</td><td>X</td>
 *     <td></td><td>X</td><td></td><td>X</td><td></td>
 *   </tr>
 *   <tr>
 *     <td>SYNC</td><td>X</td><td>X</td>
 *     <td></td><td></td><td>X</td><td>X</td><td></td>
 *   </tr>
 *   <tr>
 *     <td>TRANSFER</td><td>X</td><td>X</td>
 *     <td></td><td></td><td></td><td>X</td><td>X</td>
 *   </tr>
 *   <tr>
 *     <td>TRANSFER PURGED</td><td></td><td>X</td>
 *     <td></td><td></td><td></td><td>X</td><td>X</td>
 *   </tr>
 * </table>
 */
typedef enum
{
  /**
   * @brief Free.
   */
  RTEMS_BDBUF_STATE_FREE = 0,

  /**
   * @brief Empty.
   */
  RTEMS_BDBUF_STATE_EMPTY,

  /**
   * @brief Cached.
   */
  RTEMS_BDBUF_STATE_CACHED,

  /**
   * @brief Accessed by upper layer with cached data.
   */
  RTEMS_BDBUF_STATE_ACCESS_CACHED,

  /**
   * @brief Accessed by upper layer with modified data.
   */
  RTEMS_BDBUF_STATE_ACCESS_MODIFIED,

  /**
   * @brief Accessed by upper layer with invalid data.
   */
  RTEMS_BDBUF_STATE_ACCESS_EMPTY,

  /**
   * @brief Accessed by upper layer with purged data.
   */
  RTEMS_BDBUF_STATE_ACCESS_PURGED,

  /**
   * @brief Modified by upper layer.
   */
  RTEMS_BDBUF_STATE_MODIFIED,

  /**
   * @brief Scheduled for synchronization.
   */
  RTEMS_BDBUF_STATE_SYNC,

  /**
   * @brief In transfer by block device driver.
   */
  RTEMS_BDBUF_STATE_TRANSFER,

  /**
   * @brief In transfer by block device driver and purged.
   */
  RTEMS_BDBUF_STATE_TRANSFER_PURGED
} rtems_bdbuf_buf_state;

/**
 * Forward reference to the block.
 */
struct rtems_bdbuf_group;
typedef struct rtems_bdbuf_group rtems_bdbuf_group;

/**
 * To manage buffers we using buffer descriptors (BD). A BD holds a buffer plus
 * a range of other information related to managing the buffer in the cache. To
 * speed-up buffer lookup descriptors are organized in AVL-Tree. The fields
 * 'dd' and 'block' are search keys.
 */
typedef struct rtems_bdbuf_buffer
{
  rtems_chain_node link;       /**< Link the BD onto a number of lists. */

  struct rtems_bdbuf_avl_node
  {
    struct rtems_bdbuf_buffer* left;   /**< Left Child */
    struct rtems_bdbuf_buffer* right;  /**< Right Child */
    signed char                cache;  /**< Cache */
    signed char                bal;    /**< The balance of the sub-tree */
  } avl;

  rtems_disk_device *dd;        /**< disk device */

  rtems_blkdev_bnum block;      /**< block number on the device */

  unsigned char*    buffer;     /**< Pointer to the buffer memory area */

  rtems_bdbuf_buf_state state;           /**< State of the buffer. */

  uint32_t waiters;              /**< The number of threads waiting on this
                                  * buffer. */
  rtems_bdbuf_group* group;      /**< Pointer to the group of BDs this BD is
                                  * part of. */
  uint32_t hold_timer;           /**< Timer to indicate how long a buffer
                                  * has been held in the cache modified. */

  int   references;              /**< Allow reference counting by owner. */
  void* user;                    /**< User data. */
} rtems_bdbuf_buffer;

/**
 * A group is a continuous block of buffer descriptors. A group covers the
 * maximum configured buffer size and is the allocation size for the buffers to
 * a specific buffer size. If you allocate a buffer to be a specific size, all
 * buffers in the group, if there are more than 1 will also be that size. The
 * number of buffers in a group is a multiple of 2, ie 1, 2, 4, 8, etc.
 */
struct rtems_bdbuf_group
{
  rtems_chain_node    link;          /**< Link the groups on a LRU list if they
                                      * have no buffers in use. */
  size_t              bds_per_group; /**< The number of BD allocated to this
                                      * group. This value must be a multiple of
                                      * 2. */
  uint32_t            users;         /**< How many users the block has. */
  rtems_bdbuf_buffer* bdbuf;         /**< First BD this block covers. */
};

/**
 * Buffering configuration definition. See confdefs.h for support on using this
 * structure.
 */
typedef struct rtems_bdbuf_config {
  uint32_t            max_read_ahead_blocks;   /**< Number of blocks to read
                                                * ahead. */
  uint32_t            max_write_blocks;        /**< Number of blocks to write
                                                * at once. */
  rtems_task_priority swapout_priority;        /**< Priority of the swap out
                                                * task. */
  uint32_t            swapout_period;          /**< Period swap-out checks buf
                                                * timers. */
  uint32_t            swap_block_hold;         /**< Period a buffer is held. */
  size_t              swapout_workers;         /**< The number of worker
                                                * threads for the swap-out
                                                * task. */
  rtems_task_priority swapout_worker_priority; /**< Priority of the swap out
                                                * task. */
  size_t              task_stack_size;         /**< Task stack size for swap-out
                                                * task and worker threads. */
  size_t              size;                    /**< Size of memory in the
                                                * cache */
  uint32_t            buffer_min;              /**< Minimum buffer size. */
  uint32_t            buffer_max;              /**< Maximum buffer size
                                                * supported. It is also the
                                                * allocation size. */
  rtems_task_priority read_ahead_priority;     /**< Priority of the read-ahead
                                                * task. */
} rtems_bdbuf_config;

/**
 * External reference to the configuration.
 *
 * The configuration is provided by the application.
 */
extern const rtems_bdbuf_config rtems_bdbuf_configuration;

/**
 * The default value for the maximum read-ahead blocks disables the read-ahead
 * feature.
 */
#define RTEMS_BDBUF_MAX_READ_AHEAD_BLOCKS_DEFAULT    0

/**
 * Default maximum number of blocks to write at once.
 */
#define RTEMS_BDBUF_MAX_WRITE_BLOCKS_DEFAULT         16

/**
 * Default swap-out task priority.
 */
#define RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT    15

/**
 * Default swap-out task swap period in milli seconds.
 */
#define RTEMS_BDBUF_SWAPOUT_TASK_SWAP_PERIOD_DEFAULT 250

/**
 * Default swap-out task block hold time in milli seconds.
 */
#define RTEMS_BDBUF_SWAPOUT_TASK_BLOCK_HOLD_DEFAULT  1000

/**
 * Default swap-out worker tasks. Currently disabled.
 */
#define RTEMS_BDBUF_SWAPOUT_WORKER_TASKS_DEFAULT     0

/**
 * Default swap-out worker task priority. The same as the swap-out task.
 */
#define RTEMS_BDBUF_SWAPOUT_WORKER_TASK_PRIORITY_DEFAULT \
                             RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT

/**
 * Default read-ahead task priority.  The same as the swap-out task.
 */
#define RTEMS_BDBUF_READ_AHEAD_TASK_PRIORITY_DEFAULT \
  RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT

/**
 * Default task stack size for swap-out and worker tasks.
 */
#define RTEMS_BDBUF_TASK_STACK_SIZE_DEFAULT RTEMS_MINIMUM_STACK_SIZE

/**
 * Default size of memory allocated to the cache.
 */
#define RTEMS_BDBUF_CACHE_MEMORY_SIZE_DEFAULT (64 * 512)

/**
 * Default minimum size of buffers.
 */
#define RTEMS_BDBUF_BUFFER_MIN_SIZE_DEFAULT (512)

/**
 * Default maximum size of buffers.
 */
#define RTEMS_BDBUF_BUFFER_MAX_SIZE_DEFAULT (4096)

/**
 * Prepare buffering layer to work - initialize buffer descritors and (if it is
 * neccessary) buffers. After initialization all blocks is placed into the
 * ready state.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation. 
 * @retval RTEMS_CALLED_FROM_ISR Called from an interrupt context.
 * @retval RTEMS_INVALID_NUMBER The buffer maximum is not an integral multiple
 * of the buffer minimum.  The maximum read-ahead blocks count is too large.
 * @retval RTEMS_RESOURCE_IN_USE Already initialized.
 * @retval RTEMS_UNSATISFIED Not enough resources.
 */
rtems_status_code
rtems_bdbuf_init (void);

/**
 * Get block buffer for data to be written into. The buffers is set to the
 * access or modified access state. If the buffer is in the cache and modified
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
 * Before you can use this function, the rtems_bdbuf_init() routine must be
 * called at least once to initialize the cache, otherwise a fatal error will
 * occur.
 *
 * @param dd [in] The disk device.
 * @param block [in] Linear media block number.
 * @param bd [out] Reference to the buffer descriptor pointer.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation. 
 * @retval RTEMS_INVALID_ID Invalid block number.
 */
rtems_status_code
rtems_bdbuf_get (
  rtems_disk_device *dd,
  rtems_blkdev_bnum block,
  rtems_bdbuf_buffer** bd
);

/**
 * Get the block buffer and if not already in the cache read from the disk. If
 * specified block already cached return. The buffer is set to the access or
 * modified access state. If the buffer is in the cache and modified the state
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
 * Before you can use this function, the rtems_bdbuf_init() routine must be
 * called at least once to initialize the cache, otherwise a fatal error will
 * occur.
 *
 * @param dd [in] The disk device.
 * @param block [in] Linear media block number.
 * @param bd [out] Reference to the buffer descriptor pointer.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation. 
 * @retval RTEMS_INVALID_ID Invalid block number.
 * @retval RTEMS_IO_ERROR IO error.
 */
rtems_status_code
rtems_bdbuf_read (
  rtems_disk_device *dd,
  rtems_blkdev_bnum block,
  rtems_bdbuf_buffer** bd
);

/**
 * Release the buffer obtained by a read call back to the cache. If the buffer
 * was obtained by a get call and was not already in the cache the release
 * modified call should be used. A buffer released with this call obtained by a
 * get call may not be in sync with the contents on disk. If the buffer was in
 * the cache and modified before this call it will be returned to the modified
 * queue. The buffers is returned to the end of the LRU list.
 *
 * Before you can use this function, the rtems_bdbuf_init() routine must be
 * called at least once to initialize the cache, otherwise a fatal error will
 * occur.
 *
 * @param bd [in] Reference to the buffer descriptor.  The buffer descriptor
 * reference must not be @c NULL and must be obtained via rtems_bdbuf_get() or
 * rtems_bdbuf_read().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation. 
 * @retval RTEMS_INVALID_ADDRESS The reference is NULL.
 */
rtems_status_code
rtems_bdbuf_release (rtems_bdbuf_buffer* bd);

/**
 * Release the buffer allocated with a get or read call placing it on the
 * modified list.  If the buffer was not released modified before the hold
 * timer is set to the configuration value. If the buffer had been released
 * modified before but not written to disk the hold timer is not updated. The
 * buffer will be written to disk when the hold timer has expired, there are
 * not more buffers available in the cache and a get or read buffer needs one
 * or a sync call has been made. If the buffer is obtained with a get or read
 * before the hold timer has expired the buffer will be returned to the user.
 *
 * Before you can use this function, the rtems_bdbuf_init() routine must be
 * called at least once to initialize the cache, otherwise a fatal error will
 * occur.
 *
 * @param bd [in] Reference to the buffer descriptor.  The buffer descriptor
 * reference must not be @c NULL and must be obtained via rtems_bdbuf_get() or
 * rtems_bdbuf_read().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation. 
 * @retval RTEMS_INVALID_ADDRESS The reference is NULL.
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
 *
 * Before you can use this function, the rtems_bdbuf_init() routine must be
 * called at least once to initialize the cache, otherwise a fatal error will
 * occur.
 *
 * @param bd [in] Reference to the buffer descriptor.  The buffer descriptor
 * reference must not be @c NULL and must be obtained via rtems_bdbuf_get() or
 * rtems_bdbuf_read().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation. 
 * @retval RTEMS_INVALID_ADDRESS The reference is NULL.
 */
rtems_status_code
rtems_bdbuf_sync (rtems_bdbuf_buffer* bd);

/**
 * Synchronize all modified buffers for this device with the disk and wait
 * until the transfers have completed. The sync mutex for the cache is locked
 * stopping the addition of any further modified buffers. It is only the
 * currently modified buffers that are written.
 *
 * @note Nesting calls to sync multiple devices will be handled sequentially. A
 * nested call will be blocked until the first sync request has complete.
 *
 * Before you can use this function, the rtems_bdbuf_init() routine must be
 * called at least once to initialize the cache, otherwise a fatal error will
 * occur.
 *
 * @param dd [in] The disk device.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation. 
 */
rtems_status_code
rtems_bdbuf_syncdev (rtems_disk_device *dd);

/**
 * @brief Purges all buffers corresponding to the disk device @a dd.
 *
 * This may result in loss of data.  The read-ahead state of this device is reset.
 *
 * Before you can use this function, the rtems_bdbuf_init() routine must be
 * called at least once to initialize the cache, otherwise a fatal error will
 * occur.
 *
 * @param dd [in] The disk device.
 */
void
rtems_bdbuf_purge_dev (rtems_disk_device *dd);

/**
 * @brief Sets the block size of a disk device.
 *
 * This will set the block size derived fields of the disk device.  If
 * requested the disk device is synchronized before the block size change
 * occurs.  Since the cache is unlocked during the synchronization operation
 * some tasks may access the disk device in the meantime.  This may result in
 * loss of data.  After the synchronization the disk device is purged to ensure
 * a consistent cache state and the block size change occurs.  This also resets
 * the read-ahead state of this disk device.  Due to the purge operation this
 * may result in loss of data.
 *
 * Before you can use this function, the rtems_bdbuf_init() routine must be
 * called at least once to initialize the cache, otherwise a fatal error will
 * occur.
 *
 * @param dd [in, out] The disk device.
 * @param block_size [in] The new block size in bytes.
 * @param sync [in] If @c true, then synchronize the disk device before the
 * block size change.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation. 
 * @retval RTEMS_INVALID_NUMBER Invalid block size.
 */
rtems_status_code
rtems_bdbuf_set_block_size (rtems_disk_device *dd,
                            uint32_t           block_size,
                            bool               sync);

/**
 * @brief Returns the block device statistics.
 */
void
rtems_bdbuf_get_device_stats (const rtems_disk_device *dd,
                              rtems_blkdev_stats      *stats);

/**
 * @brief Resets the block device statistics.
 */
void
rtems_bdbuf_reset_device_stats (rtems_disk_device *dd);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
