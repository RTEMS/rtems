/* 
 * logdisk.h - Physical and logical block devices (disks) support
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) $Id$
 */

#ifndef __RTEMS_LIBBLOCK_LOGDISK_H__
#define __RTEMS_LIBBLOCK_LOGDISK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>

#include "rtems/blkdev.h"

/* Buffer pool identifier */
typedef int rtems_bdpool_id;

/* Block device ioctl handler */
typedef int (* block_device_ioctl) (dev_t dev, int req, void *argp);

/* disk_device: Entry of this type created for every disk device (both for
 * logical and physical disks).
 * Array of arrays of pointers to disk_device structures maintained. First
 * table indexed by major number and second table indexed by minor number.
 * Such data organization allow quick lookup using data structure of
 * moderated size.
 */
typedef struct disk_device {
    dev_t               dev;              /* Device ID (major + minor) */
    struct disk_device *phys_dev;         /* Physical device ID (the same
                                             as dev if this entry specifies
                                             the physical device) */
    char               *name;             /* Disk device name */
    int                 uses;             /* Use counter. Device couldn't be
                                             removed if it is in use. */
    int                 start;            /* Starting block number (0 for
                                             physical devices, block offset
                                             on the related physical device
                                             for logical device) */
    int                 size;             /* Size of physical or logical disk
                                             in disk blocks */
    int                 block_size;       /* Size of device block (minimum
                                             transfer unit) in bytes 
                                             (must be power of 2) */
    int                 block_size_log2;  /* log2 of block_size */
    rtems_bdpool_id     pool;             /* Buffer pool assigned to this
                                             device */
    block_device_ioctl  ioctl;            /* ioctl handler for this block
                                             device */
} disk_device;

/* rtems_disk_create_phys --
 *     Create physical disk entry. This function usually invoked from
 *     block device driver initialization code when physical device
 *     detected in the system. Device driver should provide ioctl handler
 *     to allow block device access operations. This primitive will register
 *     device in rtems (invoke rtems_io_register_name).
 *
 * PARAMETERS:
 *     dev        - device identifier (major, minor numbers)
 *     block_size - size of disk block (minimum data transfer unit); must be
 *                  power of 2
 *     disk_size  - number of blocks on device
 *     handler    - IOCTL handler (function providing basic block input/output
 *                  request handling BIOREQUEST and other device management
 *                  operations)
 *     name       - character name of device (e.g. /dev/hda)
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if information about new physical disk added, or
 *     error code if error occured (device already registered, wrong block
 *     size value, no memory available).
 */
rtems_status_code
rtems_disk_create_phys(dev_t dev, int block_size, int disk_size,
                       block_device_ioctl handler,
                       char *name);

/* rtems_disk_create_log --
 *     Create logical disk entry. Logical disk is contiguous area on physical
 *     disk. Disk may be splitted to several logical disks in several ways:
 *     manually or using information stored in blocks on physical disk
 *     (DOS-like partition table, BSD disk label, etc). This function usually
 *     invoked from application when application-specific splitting are in use,
 *     or from generic code which handle different logical disk organizations.
 *     This primitive will register device in rtems (invoke 
 *     rtems_io_register_name).
 *
 * PARAMETERS:
 *     dev   - logical device identifier (major, minor numbers)
 *     phys  - physical device (block device which holds this logical disk)
 *             identifier 
 *     start - starting block number on the physical device
 *     size  - logical disk size in blocks
 *     name  - logical disk name
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if logical device successfully added, or error code
 *     if error occured (device already registered, no physical device
 *     exists, logical disk is out of physical disk boundaries, no memory 
 *     available).
 */
rtems_status_code
rtems_disk_create_log(dev_t dev, dev_t phys, int start, int size, char *name);

/* rtems_disk_delete --
 *     Delete physical or logical disk device. Device may be deleted if its
 *     use counter (and use counters of all logical devices - if it is
 *     physical device) equal to 0. When physical device deleted,
 *     all logical devices deleted inherently. Appropriate devices removed
 *     from "/dev" filesystem.
 *
 * PARAMETERS:
 *     dev - device identifier (major, minor numbers)
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if block device successfully deleted, or error code
 *     if error occured (device is not defined, device is in use).
 */
rtems_status_code
rtems_disk_delete(dev_t dev);

/* rtems_disk_lookup --
 *     Find block device descriptor by its device identifier. This function
 *     increment usage counter to 1. User should release disk_device structure
 *     by invoking rtems_disk_release primitive.
 *
 * PARAMETERS:
 *     dev - device identifier (major, minor numbers)
 *
 * RETURNS:
 *     pointer to the block device descriptor, or NULL if no such device
 *     exists.
 */
disk_device *
rtems_disk_lookup(dev_t dev);

/* rtems_disk_release --
 *     Release disk_device structure (decrement usage counter to 1).
 *
 * PARAMETERS:
 *     dd - pointer to disk device structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 *
 * NOTE:
 *     It should be implemented as inline function.
 */
rtems_status_code
rtems_disk_release(disk_device *dd);
 
/* rtems_disk_next --
 *     Disk device enumerator. Looking for device having device number larger
 *     than dev and return disk device descriptor for it. If there are no
 *     such device, NULL value returned.
 *
 * PARAMETERS:
 *     dev - device number (use -1 to start search)
 *
 * RETURNS:
 *     Pointer to the disk descriptor for next disk device, or NULL if all
 *     devices enumerated. */
disk_device *
rtems_disk_next(dev_t dev);

/* rtems_diskio_initialize --
 *     Initialization of disk device library (initialize all data structures,
 *     etc.)
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if library initialized, or error code if error 
 *     occured.
 */
rtems_status_code
rtems_disk_io_initialize(void);

/* rtems_diskio_done --
 *     Release all resources allocated for disk device interface.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if all resources released, or error code if error 
 *     occured.
 */
rtems_status_code
rtems_disk_io_done(void);

#ifdef __cplusplus
}
#endif

#endif
