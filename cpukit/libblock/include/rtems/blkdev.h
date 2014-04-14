/**
 * @file
 *
 * @ingroup rtems_blkdev
 *
 * @brief Block Device Management
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
 */
/**@{**/

/**
 * @brief Block device request type.
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

struct rtems_blkdev_request;

/**
 * @brief Block device request done callback function type.
 */
typedef void (*rtems_blkdev_request_cb)(
  struct rtems_blkdev_request *req,
  rtems_status_code status
);

/**
 * @brief Block device scatter or gather buffer structure.
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
 * @brief The block device transfer request is used to read or write a number
 * of blocks from or to the device.
 *
 * Transfer requests are issued to the disk device driver with the
 * @ref RTEMS_BLKIO_REQUEST IO control.  The transfer request completion status
 * must be signalled with rtems_blkdev_request_done().  This function must be
 * called exactly once per request.  The return value of the IO control will be
 * ignored for transfer requests.
 *
 * @see rtems_blkdev_create().
 */
typedef struct rtems_blkdev_request {
  /**
   * Block device operation (read or write).
   */
  rtems_blkdev_request_op req;

  /**
   * Request done callback function.
   */
  rtems_blkdev_request_cb done;

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

  /*
   * TODO: The use of these req blocks is not a great design. The req is a
   *       struct with a single 'bufs' declared in the req struct and the
   *       others are added in the outer level struct. This relies on the
   *       structs joining as a single array and that assumes the compiler
   *       packs the structs. Why not just place on a list ? The BD has a
   *       node that can be used.
   */

  /**
   * List of scatter or gather buffers.
   */
  rtems_blkdev_sg_buffer bufs[RTEMS_ZERO_LENGTH_ARRAY];
} rtems_blkdev_request;

/**
 * @brief Signals transfer request completion status.
 *
 * This function must be called exactly once per request.
 *
 * @param[in,out] req The transfer request.
 * @param[in] status The status of the operation should be
 *  - @c RTEMS_SUCCESSFUL, if the operation was successful,
 *  - @c RTEMS_IO_ERROR, if some sort of input or output error occurred, or
 *  - @c RTEMS_UNSATISFIED, if media is no more present.
 */
static inline void rtems_blkdev_request_done(
  rtems_blkdev_request *req,
  rtems_status_code status
)
{
  (*req->done)(req, status);
}

/**
 * @brief The start block in a request.
 *
 * Only valid if the driver has returned the
 * @ref RTEMS_BLKDEV_CAP_MULTISECTOR_CONT capability.
 */
#define RTEMS_BLKDEV_START_BLOCK(req) (req->bufs[0].block)

/**
 * @name IO Control Request Codes
 */
/**@{**/

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
 * @name Block Device Driver Capabilities
 */
/**@{**/

/**
 * @brief Only consecutive multi-sector buffer requests are supported.
 *
 * This option means the cache will only supply multiple buffers that are
 * inorder so the ATA multi-sector command for example can be used. This is a
 * hack to work around the current ATA driver.
 */
#define RTEMS_BLKDEV_CAP_MULTISECTOR_CONT (1 << 0)

/**
 * @brief The driver will accept a sync call.
 *
 * A sync call is made to a driver after a bdbuf cache sync has finished.
 */
#define RTEMS_BLKDEV_CAP_SYNC (1 << 1)

/** @} */

/**
 * @brief Common IO control primitive.
 *
 * Use this in all block devices to handle the common set of IO control
 * requests.
 */
int
rtems_blkdev_ioctl(rtems_disk_device *dd, uint32_t req, void *argp);

/**
 * @brief Creates a block device.
 *
 * The block size is set to the media block size.
 *
 * @param[in] device The path for the new block device.
 * @param[in] media_block_size The media block size in bytes.  Must be positive.
 * @param[in] media_block_count The media block count.  Must be positive.
 * @param[in] handler The block device IO control handler.  Must not be @c NULL.
 * @param[in] driver_data The block device driver data.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_NUMBER Media block size or count is not positive.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_UNSATISFIED Cannot create generic device node.
 *
 * @see rtems_blkdev_create_partition(), rtems_bdbuf_set_block_size(), and
 * rtems_blkdev_request.
 */
rtems_status_code rtems_blkdev_create(
  const char *device,
  uint32_t media_block_size,
  rtems_blkdev_bnum media_block_count,
  rtems_block_device_ioctl handler,
  void *driver_data
);

/**
 * @brief Creates a partition within a parent block device.
 *
 * A partition manages a subset of consecutive blocks contained in a parent block
 * device.  The blocks must be within the range of blocks managed by the
 * associated parent block device.  The media block size and IO control
 * handler are inherited by the parent block device.  The block size is set to
 * the media block size.
 *
 * @param[in] partition The path for the new partition device.
 * @param[in] parent_block_device The parent block device path.
 * @param[in] media_block_begin The media block begin of the partition within
 * the parent block device.
 * @param[in] media_block_count The media block count of the partition.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Block device node does not exist.
 * @retval RTEMS_INVALID_NODE File system node is not a block device.
 * @retval RTEMS_NOT_IMPLEMENTED Block device implementation is incomplete.
 * @retval RTEMS_INVALID_NUMBER Block begin or block count is invalid.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_UNSATISFIED Cannot create generic device node.
 *
 * @see rtems_blkdev_create() and rtems_bdbuf_set_block_size().
 */
rtems_status_code rtems_blkdev_create_partition(
  const char *partition,
  const char *parent_block_device,
  rtems_blkdev_bnum media_block_begin,
  rtems_blkdev_bnum media_block_count
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

/**
 * @defgroup rtems_blkdev_generic Generic Disk Device
 *
 * @ingroup rtems_blkdev
 *
 * Generic disk device operations for standard RTEMS IO drivers.
 */
/**@{**/

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
 * @brief Generic block operations driver address table.
 */
extern const rtems_driver_address_table rtems_blkdev_generic_ops;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
