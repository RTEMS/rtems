/*
 * flashdisk.h -- Flash disk block device implementation
 *
 * Copyright (C) 2007 Chris Johns
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#if !defined (_RTEMS_FLASHDISK_H_)
#define _RTEMS_FLASHDISK_H_

#include <stdint.h>
#include <sys/ioctl.h>

#include <rtems.h>

/**
 * The base name of the flash disks.
 */
#define RTEMS_FLASHDISK_DEVICE_BASE_NAME "/dev/flashdisk"

/**
 * Flash disk specific ioctl request types. To use open the
 * device and issue the ioctl call.
 *
 * @code
 *  int fd = open ("/dev/flashdisk0", O_WRONLY, 0);
 *  if (fd < 0)
 *  {
 *    printf ("driver open failed: %s\n", strerror (errno));
 *    exit (1);
 *  }
 *  if (ioctl (fd, RTEMS_FDISK_IOCTL_ERASE_DISK) < 0)
 *  {
 *    printf ("driver erase failed: %s\n", strerror (errno));
 *    exit (1);
 *  }
 *  close (fd);
 * @endcode
 */
#define RTEMS_FDISK_IOCTL_ERASE_DISK   _IO('B', 128)
#define RTEMS_FDISK_IOCTL_COMPACT      _IO('B', 129)
#define RTEMS_FDISK_IOCTL_ERASE_USED   _IO('B', 130)
#define RTEMS_FDISK_IOCTL_MONITORING   _IO('B', 131)
#define RTEMS_FDISK_IOCTL_INFO_LEVEL   _IO('B', 132)
#define RTEMS_FDISK_IOCTL_PRINT_STATUS _IO('B', 133)

/**
 * Flash Disk Monitoring Data allows a user to obtain 
 * the current status of the disk.
 */
typedef struct rtems_fdisk_monitor_data
{
  uint32_t block_size;
  uint32_t block_count;
  uint32_t device_count;
  uint32_t segment_count;
  uint32_t page_count;
  uint32_t blocks_used;
  uint32_t segs_available;
  uint32_t segs_used;
  uint32_t segs_failed;
  uint32_t seg_erases;
  uint32_t pages_desc;
  uint32_t pages_active;
  uint32_t pages_used;
  uint32_t pages_bad;
  uint32_t info_level;
} rtems_fdisk_monitor_data;

/**
 * Flash Segment Descriptor holds, number of continuous segments in the
 * device of this type, the base segment number in the device, the
 * address offset of the base segment in the device, and the size of
 * segment.
 *
 * Typically this structure is part of a table of segments in the
 * device which is referenced in the flash disk configuration table.
 * The reference is kept in the driver and used all the time to
 * manage the flash device, therefore it must always exist.
 */
typedef struct rtems_fdisk_segment_desc
{
  uint16_t count;    /**< Number of segments of this type in a row. */
  uint16_t segment;  /**< The base segment number. */
  uint32_t offset;   /**< Address offset of base segment in device. */
  uint32_t size;     /**< Size of the segment in bytes. */
} rtems_fdisk_segment_desc;

/**
 * Return the number of kilo-bytes.
 */
#define RTEMS_FDISK_KBYTES(_k) ((_k) * 1024)

/**
 * Forward declaration of the device descriptor.
 */
struct rtems_fdisk_device_desc;

/**
 * Flash Low Level driver handlers.

 * Typically this structure is part of a table of handlers in the
 * device which is referenced in the flash disk configuration table.
 * The reference is kept in the driver and used all the time to
 * manage the flash device, therefore it must always exist.
 */
typedef struct rtems_fdisk_driver_handlers
{
  /**
   * Read data from the device into the buffer. Return an errno
   * error number if the device cannot be read. A segment descriptor
   * can describe more than one segment in a device if the device has
   * repeating segments. The segment number is the device segment to
   * access and the segment descriptor must reference the segment
   * being requested. For example the segment number must resided in
   * the range [base, base + count).
   * 
   * @param sd The segment descriptor.
   * @param device The device to read data from.
   * @param segment The segment within the device to read.
   * @param offset The offset in the segment to read.
   * @param buffer The buffer to read the data into.
   * @param size The amount of data to read.
   * @retval 0 No error.
   * @retval EIO The read did not complete.
   */
  int (*read) (const rtems_fdisk_segment_desc* sd,
               uint32_t                        device,
               uint32_t                        segment,
               uint32_t                        offset,
               void*                           buffer,
               uint32_t                        size);

  /**
   * Write data from the buffer to the device. Return an errno
   * error number if the device cannot be written to. A segment
   * descriptor can describe more than segment in a device if the
   * device has repeating segments. The segment number is the device
   * segment to access and the segment descriptor must reference
   * the segment being requested. For example the segment number must
   * resided in the range [base, base + count).
   * 
   * @param sd The segment descriptor.
   * @param device The device to write data from.
   * @param segment The segment within the device to write to.
   * @param offset The offset in the segment to write.
   * @param buffer The buffer to write the data from.
   * @param size The amount of data to write.
   * @retval 0 No error.
   * @retval EIO The write did not complete or verify.
   */
  int (*write) (const rtems_fdisk_segment_desc* sd,
                uint32_t                        device,
                uint32_t                        segment,
                uint32_t                        offset,
                const void*                     buffer,
                uint32_t                        size);

  /**
   * Blank a segment in the device. Return an errno error number
   * if the device cannot be read or is not blank. A segment descriptor
   * can describe more than segment in a device if the device has
   * repeating segments. The segment number is the device segment to
   * access and the segment descriptor must reference the segment
   * being requested. For example the segment number must resided in
   * the range [base, base + count).
   * 
   * @param sd The segment descriptor.
   * @param device The device to read data from.
   * @param segment The segment within the device to read.
   * @param offset The offset in the segment to checl.
   * @param size The amount of data to check.
   * @retval 0 No error.
   * @retval EIO The segment is not blank.
   */
  int (*blank) (const rtems_fdisk_segment_desc* sd,
                uint32_t                        device,
                uint32_t                        segment,
                uint32_t                        offset,
                uint32_t                        size);

  /**
   * Verify data in the buffer to the data in the device. Return an
   * errno error number if the device cannot be read. A segment
   * descriptor can describe more than segment in a device if the
   * device has repeating segments. The segment number is the
   * segment to access and the segment descriptor must reference
   * the device segment being requested. For example the segment number
   * must resided in the range [base, base + count).
   * 
   * @param sd The segment descriptor.
   * @param device The device to verify data in.
   * @param segment The segment within the device to verify.
   * @param offset The offset in the segment to verify.
   * @param buffer The buffer to verify the data in the device with.
   * @param size The amount of data to verify.
   * @retval 0 No error.
   * @retval EIO The data did not verify.
   */
  int (*verify) (const rtems_fdisk_segment_desc* sd,
                 uint32_t                        device,
                 uint32_t                        segment,
                 uint32_t                        offset,
                 const void*                     buffer,
                 uint32_t                        size);

  /**
   * Erase the segment. Return an errno error number if the
   * segment cannot be erased. A segment descriptor can describe
   * more than segment in a device if the device has repeating
   * segments. The segment number is the device segment to access and
   * the segment descriptor must reference the segment being requested.
   * 
   * @param sd The segment descriptor.
   * @param device The device to erase the segment of.
   * @param segment The segment within the device to erase.
   * @retval 0 No error.
   * @retval EIO The segment was not erased.
   */
  int (*erase) (const rtems_fdisk_segment_desc* sd,
                uint32_t                        device,
                uint32_t                        segment);

  /**
   * Erase the device. Return an errno error number if the
   * segment cannot be erased. A segment descriptor can describe
   * more than segment in a device if the device has repeating
   * segments. The segment number is the segment to access and
   * the segment descriptor must reference the segment being requested.
   * 
   * @param sd The segment descriptor.
   * @param device The device to erase.
   * @retval 0 No error.
   * @retval EIO The device was not erased.
   */
  int (*erase_device) (const struct rtems_fdisk_device_desc* dd,
                       uint32_t                              device);

} rtems_fdisk_driver_handlers;

/**
 * Flash Device Descriptor holds the segments in a device. The
 * placing of the segments in a device decriptor allows the
 * low level driver to share the segment descriptors for a
 * number of devices.
 *
 * Typically this structure is part of a table of segments in the
 * device which is referenced in the flash disk configuration table.
 * The reference is kept in the driver and used all the time to
 * manage the flash device, therefore it must always exist.
 */
typedef struct rtems_fdisk_device_desc
{
  uint32_t                           segment_count; /**< Number of segments. */
  const rtems_fdisk_segment_desc*    segments;      /**< Array of segments. */
  const rtems_fdisk_driver_handlers* flash_ops;     /**< Device handlers. */
} rtems_fdisk_device_desc;

/**
 * RTEMS Flash Disk configuration table used to initialise the
 * driver.
 */
typedef struct rtems_flashdisk_config
{
  uint32_t                       block_size;   /**< The block size. */
  uint32_t                       device_count; /**< The number of devices. */
  const rtems_fdisk_device_desc* devices;      /**< The device descriptions. */
  uint32_t                       flags;        /**< Set of flags to control
                                                    driver. */
  uint32_t                       compact_segs; /**< Max number of segs to 
                                                    compact in one pass. */
  uint32_t                       info_level;   /**< Default info level. */
} rtems_flashdisk_config;

/*
 * Driver flags.
 */

/**
 * Leave the erasing of used segment to the background handler.
 */
#define RTEMS_FDISK_BACKGROUND_ERASE (1 << 0)

/**
 * Leave the compacting of of used segment to the background handler.
 */
#define RTEMS_FDISK_BACKGROUND_COMPACT (1 << 1)

/**
 * Check the pages during initialisation to see which pages are
 * valid and which are not. This could slow down initialising the
 * disk driver.
 */
#define RTEMS_FDISK_CHECK_PAGES (1 << 2)

/**
 * Blank check the flash device before writing to them. This is needed if
 * you think you have a driver or device problem.
 */
#define RTEMS_FDISK_BLANK_CHECK_BEFORE_WRITE (1 << 3)

/**
 * Flash disk device driver initialization. Place in a table as the
 * initialisation entry and remainder of the entries are the
 * RTEMS block device generic handlers.
 *
 * @param major Flash disk major device number.
 * @param minor Minor device number, not applicable.
 * @param arg Initialization argument, not applicable.
 * @return The rtems_device_driver is actually just
 *         rtems_status_code.
 */
rtems_device_driver
rtems_fdisk_initialize (rtems_device_major_number major,
                        rtems_device_minor_number minor,
                        void*                     arg);

/**
 * External reference to the configuration. Please supply.
 * Support is present in confdefs.h for providing this variable.
 */
extern const rtems_flashdisk_config rtems_flashdisk_configuration[];

/**
 * External reference to the number of configurations. Please supply.
 * Support is present in confdefs.h for providing this variable.
 */
extern uint32_t rtems_flashdisk_configuration_size;

#endif
