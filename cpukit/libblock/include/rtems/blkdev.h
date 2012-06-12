/**
 * @file
 *
 * @ingroup rtems_blkdev
 *
 * Block device management.
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 */

#ifndef _RTEMS_BLKDEV_H
#define _RTEMS_BLKDEV_H

#include <rtems.h>
#include <rtems/diskdevs.h>
#include <rtems/bspIo.h>
#include <sys/ioctl.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtems_blkdev Block Device Management
 *
 * @ingroup rtems_libblock
 *
 * Interface between device drivers and the
 * @ref rtems_bdbuf "block device buffer module".
 *
 * The heart of the block device driver is the @ref RTEMS_BLKIO_REQUEST IO
 * control. This call puts IO @ref rtems_blkdev_request "requests" to the block
 * device for asynchronous processing. When a driver executes a request, it
 * invokes the request done callback function to finish the request.
 *
 * @{
 */

/**
 * Block device request type.
 *
 * @warning The sync request is an IO one and only used from the cache. Use the
 *          Block IO when operating at the device level. We need a sync request
 *          to avoid requests looping for ever.
 */
typedef enum rtems_blkdev_request_op {
  RTEMS_BLKDEV_REQ_READ,       /**< Read the requested blocks of data. */
  RTEMS_BLKDEV_REQ_WRITE,      /**< Write the requested blocks of data. */
  RTEMS_BLKDEV_REQ_SYNC        /**< Sync any data with the media. */
} rtems_blkdev_request_op;

/**
 * @brief Block device request done callback function type.
 *
 * The first parameter @a arg must be the argument provided by the block device
 * request structure @ref rtems_blkdev_request.
 *
 * The second parameter @a status should contain the status of the operation:
 *  - @c RTEMS_SUCCESSFUL Operation was successful.
 *  - @c RTEMS_IO_ERROR Some sort of input or output error.
 *  - @c RTEMS_UNSATISFIED Media no more present.
 */
typedef void (*rtems_blkdev_request_cb)(void *arg, rtems_status_code status);

/**
 * Block device scatter or gather buffer structure.
 */
typedef struct rtems_blkdev_sg_buffer {
  /**
   * Block index.
   */
  rtems_blkdev_bnum block;

  /**
   * Buffer length.
   */
  uint32_t length;

  /**
   * Buffer pointer.
   */
  void *buffer;

  /**
   * User pointer.
   */
  void *user;
} rtems_blkdev_sg_buffer;

/**
 * The block device request structure is used to read or write a number of
 * blocks from or to the device.
 *
 * TODO: The use of these req blocks is not a great design. The req is a
 *       struct with a single 'bufs' declared in the req struct and the
 *       others are added in the outer level struct. This relies on the
 *       structs joining as a single array and that assumes the compiler
 *       packs the structs. Why not just place on a list ? The BD has a
 *       node that can be used.
 */
typedef struct rtems_blkdev_request {
  /**
   * Block device operation (read or write).
   */
  rtems_blkdev_request_op req;

  /**
   * Request done callback function.
   */
  rtems_blkdev_request_cb req_done;

  /**
   * Argument to be passed to callback function.
   */
  void *done_arg;

  /**
   * Last IO operation completion status.
   */
  rtems_status_code status;

  /**
   * Number of blocks for this request.
   */
  uint32_t bufnum;

  /**
   * The task requesting the IO operation.
   */
  rtems_id io_task;

  /**
   * List of scatter or gather buffers.
   */
  rtems_blkdev_sg_buffer bufs[0];
} rtems_blkdev_request;

/**
 * The start block in a request.
 *
 * Only valid if the driver has returned the @ref RTEMS_BLKIO_CAPABILITIES of
 * @ref RTEMS_BLKDEV_CAP_MULTISECTOR_CONT.
 */
#define RTEMS_BLKDEV_START_BLOCK(req) (req->bufs[0].block)

/**
 * @name IO Control Request Codes
 *
 * @{
 */

#define RTEMS_BLKIO_REQUEST         _IOWR('B', 1, rtems_blkdev_request)
#define RTEMS_BLKIO_GETMEDIABLKSIZE _IOR('B', 2, uint32_t)
#define RTEMS_BLKIO_GETBLKSIZE      _IOR('B', 3, uint32_t)
#define RTEMS_BLKIO_SETBLKSIZE      _IOW('B', 4, uint32_t)
#define RTEMS_BLKIO_GETSIZE         _IOR('B', 5, rtems_blkdev_bnum)
#define RTEMS_BLKIO_SYNCDEV         _IO('B', 6)
#define RTEMS_BLKIO_DELETED         _IO('B', 7)
#define RTEMS_BLKIO_CAPABILITIES    _IO('B', 8)
#define RTEMS_BLKIO_GETDISKDEV      _IOR('B', 9, rtems_disk_device *)
#define RTEMS_BLKIO_PURGEDEV        _IO('B', 10)
#define RTEMS_BLKIO_GETDEVSTATS     _IOR('B', 11, rtems_blkdev_stats *)
#define RTEMS_BLKIO_RESETDEVSTATS   _IO('B', 12)

/** @} */

static inline int rtems_disk_fd_get_media_block_size(
  int fd,
  uint32_t *media_block_size
)
{
  return ioctl(fd, RTEMS_BLKIO_GETMEDIABLKSIZE, media_block_size);
}

static inline int rtems_disk_fd_get_block_size(int fd, uint32_t *block_size)
{
  return ioctl(fd, RTEMS_BLKIO_GETBLKSIZE, block_size);
}

static inline int rtems_disk_fd_set_block_size(int fd, uint32_t block_size)
{
  return ioctl(fd, RTEMS_BLKIO_SETBLKSIZE, &block_size);
}

static inline int rtems_disk_fd_get_block_count(
  int fd,
  rtems_blkdev_bnum *block_count
)
{
  return ioctl(fd, RTEMS_BLKIO_GETSIZE, block_count);
}

static inline int rtems_disk_fd_get_disk_device(
  int fd,
  rtems_disk_device **dd_ptr
)
{
  return ioctl(fd, RTEMS_BLKIO_GETDISKDEV, dd_ptr);
}

static inline int rtems_disk_fd_sync(int fd)
{
  return ioctl(fd, RTEMS_BLKIO_SYNCDEV);
}

static inline int rtems_disk_fd_purge(int fd)
{
  return ioctl(fd, RTEMS_BLKIO_PURGEDEV);
}

static inline int rtems_disk_fd_get_device_stats(
  int fd,
  rtems_blkdev_stats *stats
)
{
  return ioctl(fd, RTEMS_BLKIO_GETDEVSTATS, stats);
}

static inline int rtems_disk_fd_reset_device_stats(int fd)
{
  return ioctl(fd, RTEMS_BLKIO_RESETDEVSTATS);
}

/**
 * Only consecutive multi-sector buffer requests are supported.
 *
 * This option means the cache will only supply multiple buffers that are
 * inorder so the ATA multi-sector command for example can be used. This is a
 * hack to work around the current ATA driver.
 */
#define RTEMS_BLKDEV_CAP_MULTISECTOR_CONT (1 << 0)

/**
 * The driver will accept a sync call. A sync call is made to a driver
 * after a bdbuf cache sync has finished.
 */
#define RTEMS_BLKDEV_CAP_SYNC (1 << 1)

/**
 * The device driver interface conventions suppose that a driver may contain an
 * initialize, open, close, read, write and IO control entry points. These
 * primitives (except initialize) can be implemented in a generic fashion based
 * upon the supplied block device driver IO control handler. Every block device
 * driver should provide an initialize entry point, which registers the
 * appropriate IO control handler.
 */
#define RTEMS_GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES \
  rtems_blkdev_generic_open, \
  rtems_blkdev_generic_close, \
  rtems_blkdev_generic_read, \
  rtems_blkdev_generic_write, \
  rtems_blkdev_generic_ioctl

/**
 * Generic block device read primitive.
 *
 * Implemented using block device buffer management primitives.
 */
rtems_device_driver
rtems_blkdev_generic_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/**
 * Generic block device write primitive.
 *
 * Implemented using block device buffer management primitives.
 */
rtems_device_driver
rtems_blkdev_generic_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/**
 * Generic block device open primitive.
 *
 * Implemented using block device buffer management primitives.
 */
rtems_device_driver
rtems_blkdev_generic_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/**
 * Generic block device close primitive.
 *
 * Implemented using block device buffer management primitives.
 */
rtems_device_driver
rtems_blkdev_generic_close(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/**
 * Generic block device IO control primitive.
 *
 * Implemented using block device buffer management primitives.
 */
rtems_device_driver
rtems_blkdev_generic_ioctl(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/**
 * Common IO control primitive.
 *
 * Use this in all block devices to handle the common set of ioctl requests.
 */
int
rtems_blkdev_ioctl(rtems_disk_device *dd, uint32_t req, void *argp);

/**
 * @brief Generic block operations driver address table.
 */
extern const rtems_driver_address_table rtems_blkdev_generic_ops;

/**
 * @brief Creates a block device.
 *
 * @param[in] device The path for the new block device.
 * @param[in] block_size The block size.  Must be positive.
 * @param[in] block_count The block count.  Must be positive.
 * @param[in] handler The block device IO control handler.  Must not be @c NULL.
 * @param[in] driver_data The block device driver data.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_NUMBER Block size or block count is not positive.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_UNSATISFIED Cannot create generic device node.
 */
rtems_status_code rtems_blkdev_create(
  const char *device,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_block_device_ioctl handler,
  void *driver_data
);

/**
 * @brief Creates a partition within a block device.
 *
 * A partition manages a subset of consecutive blocks contained in a block
 * device.  The blocks must be within the range of blocks managed by the
 * associated block device.  The media block size, block size, and IO control
 * handler are inherited by the block device.
 *
 * @param[in] partition The path for the new partition device.
 * @param[in] device The block device path.
 * @param[in] block_begin The block begin of the partition.
 * @param[in] block_count The block count of the partition.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Block device node does not exist.
 * @retval RTEMS_INVALID_NODE File system node is not a block device.
 * @retval RTEMS_NOT_IMPLEMENTED Block device implementation is incomplete.
 * @retval RTEMS_INVALID_NUMBER Block begin or block count is invalid.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_UNSATISFIED Cannot create generic device node.
 */
rtems_status_code rtems_blkdev_create_partition(
  const char *partition,
  const char *device,
  rtems_blkdev_bnum block_begin,
  rtems_blkdev_bnum block_count
);

/**
 * @brief Prints the block device statistics.
 */
void rtems_blkdev_print_stats(
  const rtems_blkdev_stats *stats,
  rtems_printk_plugin_t print,
  void *print_arg
);

/**
 * @brief Block device statistics command.
 */
void rtems_blkstats(
  FILE *output,
  const char *device,
  bool reset
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
