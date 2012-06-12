/**
 * @file
 *
 * @ingroup rtems_disk
 *
 * @brief Block device disk management API.
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 */

#ifndef _RTEMS_DISKDEVS_H
#define _RTEMS_DISKDEVS_H

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/chain.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtems_disk_device rtems_disk_device;

/**
 * @defgroup rtems_disk Block Device Disk Management
 *
 * @ingroup rtems_libblock
 *
 * @brief This module provides functions to manage disk devices.
 *
 * A disk is a set of blocks which are identified by a consecutive set of
 * non-negative integers starting at zero.  There are also logical disks which
 * contain a subset of consecutive disk blocks.  The logical disks are used to
 * represent the partitions of a disk.  The disk devices are accessed via the
 * @ref rtems_bdbuf "block device buffer module".
 *
 * @{
 */

/**
 * @brief Block device block index type.
 */
typedef uint32_t rtems_blkdev_bnum;

/**
 * @brief Block device IO control handler type.
 */
typedef int (*rtems_block_device_ioctl)(
  rtems_disk_device *dd,
  uint32_t req,
  void *argp
);

/**
 * @brief Trigger value to disable further read-ahead requests.
 */
#define RTEMS_DISK_READ_AHEAD_NO_TRIGGER ((rtems_blkdev_bnum) -1)

/**
 * @brief Block device read-ahead control.
 */
typedef struct {
  /**
   * @brief Chain node for the read-ahead request queue of the read-ahead task.
   */
  rtems_chain_node node;

  /**
   * @brief Block value to trigger the read-ahead request.
   *
   * A value of @ref RTEMS_DISK_READ_AHEAD_NO_TRIGGER will disable further
   * read-ahead requests since no valid block can have this value.
   */
  rtems_blkdev_bnum trigger;

  /**
   * @brief Start block for the next read-ahead request.
   *
   * In case the trigger value is out of range of valid blocks, this value my
   * be arbitrary.
   */
  rtems_blkdev_bnum next;
} rtems_blkdev_read_ahead;

/**
 * @brief Block device statistics.
 *
 * Integer overflows in the statistic counters may happen.
 */
typedef struct {
  /**
   * @brief Read hit count.
   * 
   * A read hit occurs in the rtems_bdbuf_read() function in case the block is
   * in the cached or modified state.
   */
  uint32_t read_hits;

  /**
   * @brief Read miss count.
   * 
   * A read miss occurs in the rtems_bdbuf_read() function in case the block is
   * in the empty state and a read transfer must be initiated to read the data
   * from the device.
   */
  uint32_t read_misses;

  /**
   * @brief Read-ahead transfer count.
   *
   * Each read-ahead transfer may read multiple blocks.
   */
  uint32_t read_ahead_transfers;

  /**
   * @brief Count of blocks transfered from the device.
   */
  uint32_t read_blocks;

  /**
   * @brief Read error count.
   *
   * Error count of transfers issued by the read or read-ahead requests.
   */
  uint32_t read_errors;

  /**
   * @brief Write transfer count.
   *
   * Each write transfer may write multiple blocks.
   */
  uint32_t write_transfers;

  /**
   * @brief Count of blocks transfered to the device.
   */
  uint32_t write_blocks;

  /**
   * @brief Write error count.
   *
   * Error count of transfers issued by write requests.
   */
  uint32_t write_errors;
} rtems_blkdev_stats;

/**
 * @brief Description of a disk device (logical and physical disks).
 *
 * An array of pointer tables to rtems_disk_device structures is maintained.
 * The first table will be indexed by the major number and the second table
 * will be indexed by the minor number.  This allows quick lookup using a data
 * structure of moderated size.
 */
struct rtems_disk_device {
  /**
   * @brief Device identifier (concatenation of major and minor number).
   */
  dev_t dev;

  /**
   * @brief Physical device identifier (equals the @c dev entry if it specifies a
   * physical device).
   */
  rtems_disk_device *phys_dev;

  /**
   * @brief Driver capabilities.
   */
  uint32_t capabilities;

  /**
   * @brief Disk device name.
   */
  char *name;

  /**
   * @brief Usage counter.
   *
   * Devices cannot be deleted if they are in use.
   */
  unsigned uses;

  /**
   * @brief Start media block number.
   *
   * Equals zero for physical devices.  It is a media block offset to the
   * related physical device for logical device.
   */
  rtems_blkdev_bnum start;

  /**
   * @brief Size of the physical or logical disk in media blocks.
   */
  rtems_blkdev_bnum size;

  /**
   * @brief Media block size in bytes.
   *
   * This is the media transfer unit the hardware defaults to.
   */
  uint32_t media_block_size;

  /**
   * @brief Block size in bytes.
   *
   * This is the minimum transfer unit.  It may be a multiple of the media
   * block size. It must be positive.
   *
   * @see rtems_bdbuf_set_block_size().
   */
  uint32_t block_size;

  /**
   * @brief Block count.
   *
   * @see rtems_bdbuf_set_block_size().
   */
  rtems_blkdev_bnum block_count;

  /**
   * @brief Media blocks per device blocks.
   *
   * @see rtems_bdbuf_set_block_size().
   */
  uint32_t media_blocks_per_block;

  /**
   * @brief Block to media block shift.
   *
   * In case this value is non-negative the media block of a block can be
   * calculated as media block = block << block_to_media_block_shift, otherwise
   * a 64-bit operation will be used.
   *
   * @see rtems_bdbuf_set_block_size().
   */
  int block_to_media_block_shift;

  /**
   * @brief Buffer descriptors per group count.
   *
   * @see rtems_bdbuf_set_block_size().
   */
  size_t bds_per_group;

  /**
   * @brief IO control handler for this disk.
   */
  rtems_block_device_ioctl ioctl;

  /**
   * @brief Private data for the disk driver.
   */
  void *driver_data;

  /**
   * @brief Indicates that this disk should be deleted as soon as the last user
   * releases this disk.
   */
  bool deleted;

  /**
   * @brief Device statistics for this disk.
   */
  rtems_blkdev_stats stats;

  /**
   * @brief Read-ahead control for this disk.
   */
  rtems_blkdev_read_ahead read_ahead;
};

/**
 * @name Disk Device Data
 *
 * @{
 */

static inline dev_t rtems_disk_get_device_identifier(
  const rtems_disk_device *dd
)
{
  return dd->dev;
}

static inline rtems_device_major_number rtems_disk_get_major_number(
  const rtems_disk_device *dd
)
{
  return rtems_filesystem_dev_major_t(dd->dev);
}

static inline rtems_device_minor_number rtems_disk_get_minor_number(
  const rtems_disk_device *dd
)
{
  return rtems_filesystem_dev_minor_t(dd->dev);
}

static inline void *rtems_disk_get_driver_data(
  const rtems_disk_device *dd
)
{
  return dd->driver_data;
}

static inline uint32_t rtems_disk_get_media_block_size(
  const rtems_disk_device *dd
)
{
  return dd->media_block_size;
}

static inline uint32_t rtems_disk_get_block_size(
  const rtems_disk_device *dd
)
{
  return dd->block_size;
}

static inline rtems_blkdev_bnum rtems_disk_get_block_begin(
  const rtems_disk_device *dd
)
{
  return dd->start;
}

static inline rtems_blkdev_bnum rtems_disk_get_block_count(
  const rtems_disk_device *dd
)
{
  return dd->size;
}

/** @} */

/**
 * @name Disk Device Maintainance
 *
 * @{
 */

/**
 * @brief Creates a physical disk with device identifier @a dev.
 *
 * The block size @a block_size must be positive.  The disk will have
 * @a block_count blocks.  The block index starts with zero.  The associated disk
 * device driver will be invoked via the IO control handler @a handler.  A
 * device node will be registered in the file system with absolute path @a
 * name, if @a name is not @c NULL.  This function is usually invoked from a
 * block device driver during initialization when a physical device is detected
 * in the system.  The device driver provides an IO control handler to allow
 * block device operations.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NOT_CONFIGURED Cannot lock disk device operation mutex.
 * @retval RTEMS_INVALID_ADDRESS IO control handler is @c NULL.
 * @retval RTEMS_INVALID_NUMBER Block size is invalid.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_RESOURCE_IN_USE Disk device descriptor is already in use.
 * @retval RTEMS_UNSATISFIED Cannot create device node.
 */
rtems_status_code rtems_disk_create_phys(
  dev_t dev,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_block_device_ioctl handler,
  void *driver_data,
  const char *name
);

/**
 * @brief Creates a logical disk with device identifier @a dev.
 *
 * A logical disk manages a subset of consecutive blocks contained in the
 * physical disk with identifier @a phys.  The start block index of the logical
 * disk device is @a block_begin.  The block count of the logcal disk will be
 * @a block_count.  The blocks must be within the range of blocks managed by
 * the associated physical disk device.  A device node will be registered in
 * the file system with absolute path @a name, if @a name is not @c NULL.  The
 * block size and IO control handler are inherited by the physical disk.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NOT_CONFIGURED Cannot lock disk device operation mutex.
 * @retval RTEMS_INVALID_ID Specified physical disk identifier does not
 * correspond to a physical disk.
 * @retval RTEMS_INVALID_NUMBER Begin block or block count are out of range.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_RESOURCE_IN_USE Disk device descriptor for logical disk
 * identifier is already in use.
 * @retval RTEMS_UNSATISFIED Cannot create device node.
 */
rtems_status_code rtems_disk_create_log(
  dev_t dev,
  dev_t phys,
  rtems_blkdev_bnum block_begin,
  rtems_blkdev_bnum block_count,
  const char *name
);

/**
 * @brief Deletes a physical or logical disk device with identifier @a dev.
 *
 * Marks the disk device as deleted.  When a physical disk device is deleted,
 * all corresponding logical disk devices will marked as deleted too.  Disks
 * that are marked as deleted and have a usage counter of zero will be deleted.
 * The corresponding device nodes will be removed from the file system.  In
 * case of a physical disk deletion the IO control handler will be invoked with
 * a RTEMS_BLKIO_DELETED request.  Disks that are still in use will be deleted
 * upon release.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NOT_CONFIGURED Cannot lock disk device operation mutex.
 * @retval RTEMS_INVALID_ID No disk for specified device identifier.
 */
rtems_status_code rtems_disk_delete(dev_t dev);

/**
 * @brief Returns the disk device descriptor for the device identifier @a dev.
 *
 * Increments usage counter by one.  You should release the disk device
 * descriptor with rtems_disk_release().
 *
 * @return Pointer to the disk device descriptor or @c NULL if no corresponding
 * disk exists.
 */
rtems_disk_device *rtems_disk_obtain(dev_t dev);

/**
 * @brief Releases the disk device descriptor @a dd.
 *
 * Decrements usage counter by one.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 */
rtems_status_code rtems_disk_release(rtems_disk_device *dd);

/** @} */

/**
 * @name Disk Management
 *
 * @{
 */

/**
 * @brief Initializes the disk device management.
 *
 * This functions returns successful if the disk device management is already
 * initialized.  There is no protection against concurrent access.
 *
 * @retval RTEMS_SUCCESSFUL Successful initialization.
 * @retval RTEMS_NO_MEMORY Not enough memory or no semaphore available.
 * @retval RTEMS_UNSATISFIED Block device buffer initialization failed.
 */
rtems_status_code rtems_disk_io_initialize(void);

/**
 * @brief Releases all resources allocated for disk device management.
 *
 * There is no protection against concurrent access.  If parts of the system
 * are still in use the behaviour is undefined.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 */
rtems_status_code rtems_disk_io_done(void);

/** @} */

/** @} */

/**
 * @brief Disk device iterator.
 *
 * Returns the next disk device descriptor with a device identifier larger than
 * @a dev.  If there is no such device, @c NULL will be returned.  Use minus
 * one to start the search.
 *
 * @code
 * rtems_status_code sc = RTEMS_SUCCESSFUL;
 * rtems_disk_device *dd = (dev_t) -1;
 *
 * while (sc == RTEMS_SUCCESSFUL && (dd = rtems_disk_next(dev)) != NULL) {
 *   dev = rtems_disk_get_device_identifier(dd);
 *   sc = rtems_disk_release(dd);
 * }
 * @endcode
 */
rtems_disk_device *rtems_disk_next(dev_t dev);

/* Internal function, do not use */
rtems_status_code rtems_disk_init_phys(
  rtems_disk_device *dd,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_block_device_ioctl handler,
  void *driver_data
);

/* Internal function, do not use */
rtems_status_code rtems_disk_init_log(
  rtems_disk_device *dd,
  rtems_disk_device *phys_dd,
  rtems_blkdev_bnum block_begin,
  rtems_blkdev_bnum block_count
);

#ifdef __cplusplus
}
#endif

#endif
