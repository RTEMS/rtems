/**
 * @file
 *
 * RAM disk block device.
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) $Id$
 */

#ifndef _RTEMS_RAMDISK_H
#define _RTEMS_RAMDISK_H


#include <rtems.h>
#include <rtems/blkdev.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtems_ramdisk RAM Disk Device
 *
 * @ingroup rtems_blkdev
 *
 * @{
 */

/**
 * RAM disk configuration table entry.
 */
typedef struct rtems_ramdisk_config {
  /**
   * RAM disk block size (must be a power of two).
   */
  uint32_t block_size;

  /**
   * Number of blocks on this RAM disk.
   */
  rtems_blkdev_bnum block_num;

  /**
   * RAM disk location or @c NULL if RAM disk memory should be allocated
   * dynamically.
   */
  void *location;
} rtems_ramdisk_config;

/**
 * External reference to the RAM disk configuration table describing each RAM
 * disk in the system.
 *
 * The configuration table is provided by the application.
 */
extern rtems_ramdisk_config rtems_ramdisk_configuration [];

/**
 * External reference the size of the RAM disk configuration table
 * @ref rtems_ramdisk_configuration.
 *
 * The configuration table size is provided by the application.
 */
extern size_t rtems_ramdisk_configuration_size;

/**
 * RAM disk driver initialization entry point.
 */
rtems_device_driver ramdisk_initialize(
 rtems_device_major_number major,
 rtems_device_minor_number minor,
 void *arg
);

/**
 * RAM disk driver table entry.
 */
#define RAMDISK_DRIVER_TABLE_ENTRY \
  { \
    .initialization_entry = ramdisk_initialize, \
    RTEMS_GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES \
  }

/** @} */

#ifdef __cplusplus
}
#endif

#endif
