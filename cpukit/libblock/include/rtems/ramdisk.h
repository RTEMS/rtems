/**
 * @file
 *
 * @ingroup rtems_ramdisk
 *
 * @brief RAM disk block device API.
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
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
 * @name Static Configuration
 *
 * @{
 */

/**
 * @brief RAM disk configuration table entry.
 */
typedef struct rtems_ramdisk_config {
  /**
   * @brief RAM disk block size.
   */
  uint32_t block_size;

  /**
   * @brief Number of blocks on this RAM disk.
   */
  rtems_blkdev_bnum block_num;

  /**
   * @brief RAM disk location or @c NULL if RAM disk memory should be allocated
   * dynamically.
   */
  void *location;
} rtems_ramdisk_config;

/**
 * @brief External reference to the RAM disk configuration table describing
 * each RAM disk in the system.
 *
 * The configuration table is provided by the application.
 */
extern rtems_ramdisk_config rtems_ramdisk_configuration [];

/**
 * @brief External reference the size of the RAM disk configuration table @ref
 * rtems_ramdisk_configuration.
 *
 * The configuration table size is provided by the application.
 */
extern size_t rtems_ramdisk_configuration_size;

/**
 * @brief RAM disk driver initialization entry point.
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

#define RAMDISK_DEVICE_BASE_NAME "/dev/rd"

/** @} */

/**
 * @name Runtime Configuration
 *
 * @{
 */

/**
 * @brief RAM disk descriptor.
 */
typedef struct ramdisk {
  /**
   * @brief RAM disk block size, the media size.
   */
  uint32_t block_size;

  /**
   * @brief Number of blocks on this RAM disk.
   */
  rtems_blkdev_bnum block_num;

  /**
   * @brief RAM disk memory area.
   */
  void *area;

  /**
   * @brief RAM disk is initialized.
   */
  bool initialized;

  /**
   * @brief Indicates if memory is allocated by malloc() for this RAM disk.
   */
  bool malloced;

  /**
   * @brief Trace enable.
   */
  bool trace;

  /**
   * @brief Free the RAM disk at the block device delete request.
   */
  bool free_at_delete_request;
} ramdisk;

extern const rtems_driver_address_table ramdisk_ops;

int ramdisk_ioctl(rtems_disk_device *dd, uint32_t req, void *argp);

/**
 * @brief Allocates and initializes a RAM disk descriptor.
 *
 * The block size will be @a block_size.  The block count will be @a
 * block_count.  The disk storage area begins at @a area_begin.  If @a
 * area_begin is @c NULL, the memory will be allocated and zeroed.  Sets the
 * trace enable to @a trace.
 *
 * @return Pointer to allocated and initialized ramdisk structure, or @c NULL
 * if no memory is available.
 *
 * @note
 * Runtime configuration example:
 * @code
 * #include <rtems.h>
 * #include <rtems/libio.h>
 * #include <rtems/ramdisk.h>
 *
 * rtems_status_code create_ramdisk(
 *   const char *disk_name_path,
 *   uint32_t block_size,
 *   rtems_blkdev_bnum block_count
 * )
 * {
 *   rtems_status_code sc = RTEMS_SUCCESSFUL;
 *   rtems_device_major_number major = 0;
 *   ramdisk *rd = NULL;
 *   dev_t dev = 0;
 *
 *   sc = rtems_io_register_driver(0, &ramdisk_ops, &major);
 *   if (sc != RTEMS_SUCCESSFUL) {
 *     return RTEMS_UNSATISFIED;
 *   }
 *
 *   rd = ramdisk_allocate(NULL, block_size, block_count, false);
 *   if (rd == NULL) {
 *     rtems_io_unregister_driver(major);
 *
 *     return RTEMS_UNSATISFIED;
 *   }
 *
 *   dev = rtems_filesystem_make_dev_t(major, 0);
 *
 *   sc = rtems_disk_create_phys(
 *     dev,
 *     block_size,
 *     block_count,
 *     ramdisk_ioctl,
 *     rd,
 *     disk_name_path
 *   );
 *   if (sc != RTEMS_SUCCESSFUL) {
 *     ramdisk_free(rd);
 *     rtems_io_unregister_driver(major);
 *
 *     return RTEMS_UNSATISFIED;
 *   }
 *
 *   return RTEMS_SUCCESSFUL;
 * }
 * @endcode
 */
ramdisk *ramdisk_allocate(
  void *area_begin,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  bool trace
);

void ramdisk_free(ramdisk *rd);

static inline void ramdisk_enable_free_at_delete_request(ramdisk *rd)
{
  rd->free_at_delete_request = true;
}

/**
 * @brief Allocates, initializes and registers a RAM disk.
 *
 * The block size will be @a block_size.  The block count will be @a
 * block_count.  The disk storage will be allocated.  Sets the trace enable to
 * @a trace.  Registers a device node with disk name path @a disk.  The
 * registered device number will be returned in @a dev.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_UNSATISFIED Something is wrong.
 */
rtems_status_code ramdisk_register(
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  bool trace,
  const char *disk,
  dev_t *dev
);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
