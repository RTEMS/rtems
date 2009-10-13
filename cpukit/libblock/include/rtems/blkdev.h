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
 *
 * @(#) $Id$
 */

#ifndef _RTEMS_BLKDEV_H
#define _RTEMS_BLKDEV_H

#include <rtems.h>
#include <rtems/diskdevs.h>
#include <sys/ioctl.h>

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
 */
typedef enum rtems_blkdev_request_op {
  RTEMS_BLKDEV_REQ_READ,       /**< Read the requested blocks of data. */
  RTEMS_BLKDEV_REQ_WRITE,      /**< Write the requested blocks of data. */
  RTEMS_BLKDEV_CAPABILITIES    /**< Return the driver capabilities set. */
} rtems_blkdev_request_op;

/** 
 * Only consecutive multi-sector buffer requests are supported.
 *
 * This option means the cache will only supply multiple buffers that are
 * inorder so the ATA multi-sector command for example can be used. This is a
 * hack to work around the current ATA driver.
 */
#define RTEMS_BLKDEV_CAP_MULTISECTOR_CONT (1 << 0)

/**
 * Type for block device request done callback function.
 *
 * @param arg Argument supplied in @ref rtems_blkdev_request.
 * @param status Status code for this operation.
 * @param errno The @c errno value to be passed to the user when status is not
 * equal to @c RTEMS_SUCCESSFUL.
 */
typedef void (* rtems_blkdev_request_cb)(void *arg,
                                         rtems_status_code status,
                                         int error);

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
   * If @c status is not equal to @c RTEMS_SUCCESSFUL, this field contains the
   * error number.
   */
  int error;

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
 * Only valid if the driver has returned the @ref RTEMS_BLKDEV_CAPABILITIES of
 * @ref RTEMS_BLKDEV_CAP_MULTISECTOR_CONT.
 */
#define RTEMS_BLKDEV_START_BLOCK(req) (req->bufs[0].block)

/**
 * @name IO Control Request Codes
 *
 * @{
 */

#define RTEMS_BLKIO_REQUEST         _IOWR('B', 1, rtems_blkdev_request)
#define RTEMS_BLKIO_GETMEDIABLKSIZE _IO('B', 2)
#define RTEMS_BLKIO_GETBLKSIZE      _IO('B', 3)
#define RTEMS_BLKIO_SETBLKSIZE      _IO('B', 4)
#define RTEMS_BLKIO_GETSIZE         _IO('B', 5)
#define RTEMS_BLKIO_SYNCDEV         _IO('B', 6)

/** @} */

/**
 * The device driver interface conventions suppose that a driver may contain an
 * initialize, open, close, read, write and IO control entry points. These
 * primitives (except initialize) can be implemented in a generic fashion based
 * upon the supplied block device driver IO control handler. Every block device
 * driver should provide an initialize entry point, which registers the
 * appropriate IO control handler.
 */
#define RTEMS_GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES \
  .open_entry = rtems_blkdev_generic_open, \
  .close_entry = rtems_blkdev_generic_close, \
  .read_entry = rtems_blkdev_generic_read, \
  .write_entry = rtems_blkdev_generic_write, \
  .control_entry = rtems_blkdev_generic_ioctl

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

/** @} */

#ifdef __cplusplus
}
#endif

#endif
