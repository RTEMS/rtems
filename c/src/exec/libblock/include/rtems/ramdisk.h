/* ramdisk.c -- RAM disk block device implementation
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) $Id$
 */

#ifndef __RTEMS_LIBBLOCK_RAMDISK_H__
#define __RTEMS_LIBBLOCK_RAMDISK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>

#include "rtems/blkdev.h"

/* RAM disk configuration table entry */
typedef struct rtems_ramdisk_config {
    int   block_size; /* RAM disk block size */
    int   block_num;  /* Number of blocks on this RAM disk */
    void *location;   /* RAM disk permanent location (out of RTEMS controlled
                         memory), or NULL if RAM disk memory should be
                         allocated dynamically */
} rtems_ramdisk_config;

/* If application want to use RAM disk, it should specify configuration of
 * available RAM disks.
 * The following is definitions for RAM disk configuration table
 */
extern rtems_ramdisk_config rtems_ramdisk_configuration[];
extern int rtems_ramdisk_configuration_size;

/* ramdisk_initialize --
 *     RAM disk driver initialization entry point.
 */
rtems_device_driver
ramdisk_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg);

#define RAMDISK_DRIVER_TABLE_ENTRY \
    { ramdisk_initialize, GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES }

#ifdef __cplusplus
}
#endif

#endif
