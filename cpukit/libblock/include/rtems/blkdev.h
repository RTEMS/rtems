/* 
 * blkdev.h - block device driver interface definitions
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) $Id$
 */

#ifndef __RTEMS_LIBBLOCK_BLKDEV_H__
#define __RTEMS_LIBBLOCK_BLKDEV_H__

#include <rtems.h>
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Interface with device drivers
 * Block device looks, initialized and behaves like traditional RTEMS device 
 * driver. Heart of the block device driver is in BIOREQUEST ioctl. This call
 * puts I/O request to the block device queue, in priority order, for
 * asynchronous processing. When driver executes request, req_done
 * function invoked, so callee knows about it. Look for details below.
 */


/* Block device block number datatype */
typedef uint32_t   blkdev_bnum;

/* Block device request type */
typedef enum blkdev_request_op {
    BLKDEV_REQ_READ,  /* Read operation */
    BLKDEV_REQ_WRITE  /* Write operation */
} blkdev_request_op;

/* Type for block device request done callback function.
 *
 * PARAMETERS:
 *     arg    - argument supplied in blkdev_request
 *     status - rtems status code for this operation
 *     errno  - errno value to be passed to the user when
 *              status != RTEMS_SUCCESSFUL
 */
typedef void (* blkdev_request_cb)(void *arg, 
                                   rtems_status_code status,
                                   int error);

/* blkdev_sg_buffer
 * Block device scatter/gather buffer structure
 */
typedef struct blkdev_sg_buffer {
    uint32_t   length;  /* Buffer length */
    void            *buffer;  /* Buffer pointer */
} blkdev_sg_buffer;

/* blkdev_request (Block Device Request) structure is
 * used to read/write a number of blocks from/to device. 
 */
typedef struct blkdev_request {
    blkdev_request_op req;      /* Block device operation (read or write) */
    blkdev_request_cb req_done; /* Callback function */
    void             *done_arg; /* Argument to be passed to callback function*/
    rtems_status_code status;  /* Last I/O operation completion status */
    int               error;   /* If status != RTEMS_SUCCESSFUL, this field 
                                * contains error code
                                */
    blkdev_bnum       start;    /* Start block number */
    uint32_t    count;    /* Number of blocks to be exchanged */
    uint32_t    bufnum;   /* Number of buffers provided */    
    
    blkdev_sg_buffer  bufs[0];/* List of scatter/gather buffers */
} blkdev_request;

/* Block device IOCTL request codes */
#define BLKIO_REQUEST      _IOWR('B', 1, blkdev_request)
#define BLKIO_GETBLKSIZE   _IO('B', 2)
#define BLKIO_GETSIZE      _IO('B', 3)
#define BLKIO_SYNCDEV      _IO('B', 4)

/* Device driver interface conventions suppose that driver may
 * contain initialize/open/close/read/write/ioctl entry points. These 
 * primitives (except initialize) can be implemented in generic fashion, 
 * based upon supplied block device driver ioctl handler. Every block
 * device driver should provide initialize entry point, which is register
 * all block devices and appropriate ioctl handlers.
 */

#define GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES \
      rtems_blkdev_generic_open, rtems_blkdev_generic_close, \
      rtems_blkdev_generic_read, rtems_blkdev_generic_write, \
      rtems_blkdev_generic_ioctl

/* blkdev_generic_read --
 *     Generic block device read primitive. Implemented using block device
 *     buffer management primitives.
 */
rtems_device_driver
rtems_blkdev_generic_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/* blkdev_generic_write --
 *     Generic block device driver write primitive. Implemented using block 
 *     device buffer management primitives. 
 */
rtems_device_driver
rtems_blkdev_generic_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/* blkdev_generic_open --
 *     Generic block device open primitive.
 */
rtems_device_driver
rtems_blkdev_generic_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/* blkdev_generic_close --
 *     Generic block device close primitive.
 */
rtems_device_driver
rtems_blkdev_generic_close(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

/* blkdev_generic_ioctl --
 *     Generic block device ioctl primitive.
 */
rtems_device_driver
rtems_blkdev_generic_ioctl(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
);

#ifdef __cplusplus
}
#endif

#endif
