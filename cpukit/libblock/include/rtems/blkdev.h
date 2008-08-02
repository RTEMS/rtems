/**
 * @file rtems/blkdev.h
 * block device driver interface definitions
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
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Interface with device drivers Block device looks, initialized and behaves
 * like traditional RTEMS device driver. Heart of the block device driver is in
 * BIOREQUEST ioctl. This call puts I/O request to the block device queue, in
 * priority order, for asynchronous processing. When driver executes request,
 * req_done function invoked, so callee knows about it. Look for details below.
 */

/*
 * Block device block number datatype
 */
typedef uint32_t rtems_blkdev_bnum;

/* Block device request type */
typedef enum rtems_blkdev_request_op {
    RTEMS_BLKDEV_REQ_READ,     /* Read operation */
    RTEMS_BLKDEV_REQ_WRITE,    /* Write operation */
    RTEMS_BLKDEV_CAPABILITIES  /* Capabilities request */
} rtems_blkdev_request_op;

/** 
 * ATA multi-sector buffer requests only supported. This option
 * means the cache will only supply multiple buffers that are
 * inorder so the ATA multi-sector command can be used. This is a
 * hack to work around the current ATA driver.
 */
#define RTEMS_BLKDEV_CAP_MULTISECTOR_CONT (1 << 0)

/*
 * @typedef rtems_blkdev_request_cb
 *
 * Type for block device request done callback function.
 *
 * @param arg Argument supplied in blkdev_request
 * @param status RTEMS status code for this operation
 * @param errno errno value to be passed to the user when
 *              status != RTEMS_SUCCESSFUL
 */
typedef void (* rtems_blkdev_request_cb)(void *arg,
                                         rtems_status_code status,
                                         int error);

/**
 * @struct rtems_blkdev_sg_buffer
 * Block device scatter/gather buffer structure
 */
typedef struct rtems_blkdev_sg_buffer {
    uint32_t   block;   /* The block number */
    uint32_t   length;  /* Buffer length */
    void      *buffer;  /* Buffer pointer */
    void      *user;    /* User pointer */
} rtems_blkdev_sg_buffer;

/* blkdev_request (Block Device Request) structure is
 * used to read/write a number of blocks from/to device.
 */
typedef struct rtems_blkdev_request {
    /* Block device operation (read or write) */
    rtems_blkdev_request_op req;
    /* Callback function */
    rtems_blkdev_request_cb req_done;
    /* Argument to be passed to callback function*/
    void *done_arg;
    /* Last I/O operation completion status */
    rtems_status_code status; 
    /* If status != RTEMS_SUCCESSFUL, this field contains error code */
    int error;
    /* Number of blocks for this request. */
    uint32_t bufnum;

    /* The task requesting the IO operation. */
    rtems_id io_task;

    /* List of scatter/gather buffers */
    rtems_blkdev_sg_buffer bufs[0];
} rtems_blkdev_request;

/* The start block in a request. Only valid if the driver has returned the
 * RTEMS_BLKDEV_CAPABILITIES of RTEMS_BLKDEV_CAP_MULTISECTOR_CONT */
#define RTEMS_BLKDEV_START_BLOCK(_r) (_r->bufs[0].block)

/* Block device IOCTL request codes */
#define RTEMS_BLKIO_REQUEST      _IOWR('B', 1, rtems_blkdev_request)
#define RTEMS_BLKIO_GETBLKSIZE   _IO('B', 2)
#define RTEMS_BLKIO_GETSIZE      _IO('B', 3)
#define RTEMS_BLKIO_SYNCDEV      _IO('B', 4)

/* Device driver interface conventions suppose that driver may
 * contain initialize/open/close/read/write/ioctl entry points. These
 * primitives (except initialize) can be implemented in generic fashion,
 * based upon supplied block device driver ioctl handler. Every block
 * device driver should provide initialize entry point, which is register
 * all block devices and appropriate ioctl handlers.
 */

#define RTEMS_GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES \
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
