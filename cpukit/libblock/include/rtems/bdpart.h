/**
 * @file
 *
 * @ingroup rtems_bdpart
 *
 * Block device partition management.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef RTEMS_BDPART_H
#define RTEMS_BDPART_H

#include <uuid/uuid.h>

#include <rtems.h>
#include <rtems/blkdev.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup rtems_bdpart Block Device Partition Management
 *
 * @ingroup rtems_libblock
 *
 * This module provides functions to manage partitions of a disk device.
 *
 * A @ref rtems_disk "disk" is a set of blocks which are identified by a
 * consecutive set of non-negative integers starting at zero.  There are also
 * logical disks which contain a subset of consecutive disk blocks.  The
 * logical disks are used to represent the partitions of a disk. The disk
 * devices are accessed via the @ref rtems_disk "block device buffer module".
 *
 * The partition format on the physical disk will be converted to an internal
 * representation.  It is possible to convert the internal representation into
 * a specific output format and write it to the physical disk.  One of the
 * constrains for the internal representation was to support the GPT format
 * easily.
 *
 * Currently two physical partition formats are supported.  These are the MBR
 * and the GPT format.  Please note that the GPT support is not implemented.
 * With MBR format we mean the partition format of the wide spread IBM
 * PC-compatible systems.  The GPT format is defined in the Extensible Firmware
 * Interface (EFI).
 *
 * The most common task will be to read the partition information of a disk and
 * register logical disks for each partition.  This can be done with the
 * rtems_bdpart_register_from_disk() function.  Afterwards you can
 * @ref rtems_fsmount "mount" the file systems within the partitions.
 *
 * You can read the partition information from a disk with rtems_bdpart_read()
 * and write it to the disk with rtems_bdpart_write().
 *
 * To create a partition table from scratch for a disk use
 * rtems_bdpart_create().
 *
 * You can access some disk functions with the shell command @c fdisk.
 *
 * References used to create this module:
 *  - <a href="http://en.wikipedia.org/wiki/UUID">Universally Unique Identifier</a>
 *  - <a href="http://en.wikipedia.org/wiki/Globally_Unique_Identifier">Globally Unique Identifier</a>
 *  - <a href="http://en.wikipedia.org/wiki/Disk_partitioning">Disk Paritioning</a>
 *  - <a href="http://en.wikipedia.org/wiki/GUID_Partition_Table">GUID Partition Table</a>
 *  - <a href="http://en.wikipedia.org/wiki/Master_boot_record">Master Boot Record</a>
 *  - <a href="http://en.wikipedia.org/wiki/Extended_boot_record">Extended Boot Record</a>
 *  - <a href="http://en.wikipedia.org/wiki/Cylinder-head-sector">Cylinder Head Sector</a>
 *  - <a href="http://www.win.tue.nl/~aeb/partitions/partition_types-1.html">Partition Types</a>
 *
 * @{
 */

/**
 * @name MBR Partition Types and Flags
 *
 * @{
 */

#define RTEMS_BDPART_MBR_EMPTY 0x0U

#define RTEMS_BDPART_MBR_FAT_12 0x1U

#define RTEMS_BDPART_MBR_FAT_16 0x4U

#define RTEMS_BDPART_MBR_FAT_16_LBA 0xeU

#define RTEMS_BDPART_MBR_FAT_32 0xbU

#define RTEMS_BDPART_MBR_FAT_32_LBA 0xcU

#define RTEMS_BDPART_MBR_EXTENDED 0x5U

#define RTEMS_BDPART_MBR_DATA 0xdaU

#define RTEMS_BDPART_MBR_GPT 0xeeU

#define RTEMS_BDPART_MBR_FLAG_ACTIVE 0x80U

/** @} */

/**
 * Recommended maximum partition table size.
 */
#define RTEMS_BDPART_PARTITION_NUMBER_HINT 16

/**
 * Partition description.
 */
typedef struct rtems_bdpart_partition {
  /**
   * Block index for partition begin.
   */
  rtems_blkdev_bnum begin;

  /**
   * Block index for partition end (this block is not a part of the partition).
   */
  rtems_blkdev_bnum end;

  /**
   * Partition type.
   */
  uuid_t type;

  /**
   * Partition ID.
   */
  uuid_t id;

  /**
   * Partition flags.
   */
  uint64_t flags;
} rtems_bdpart_partition;

/**
 * Disk format for the partition tables.
 */
typedef enum {
  /**
   * Type value for MBR format.
   */
  RTEMS_BDPART_FORMAT_MBR,

  /**
   * Type value for GPT format.
   */
  RTEMS_BDPART_FORMAT_GPT
} rtems_bdpart_format_type;

/**
 * Disk format description.
 */
typedef union {
  /**
   * Format type.
   */
  rtems_bdpart_format_type type;

  /**
   * MBR format fields.
   */
  struct {
    rtems_bdpart_format_type type;

    /**
     * Disk ID in MBR at offset 440.
     */
    uint32_t disk_id;

    /**
     * This option is used for partition table creation and validation checks
     * before a write to the disk.  It ensures that the first primary
     * partition and the logical partitions start at head one and sector one
     * under the virtual one head and 63 sectors geometry.  Each begin and
     * end of a partition will be aligned to the virtual cylinder boundary.
     */
    bool dos_compatibility;
  } mbr;

  /**
   * GPT format fields.
   */
  struct {
    rtems_bdpart_format_type type;

    /**
     * Disk ID in GPT header.
     */
    uuid_t disk_id;
  } gpt;
} rtems_bdpart_format;

/**
 * Reads the partition information from the physical disk device with name
 * @a disk_name.
 *
 * The partition information will be stored in the partition table
 * @a partitions with a maximum of @a count partitions.  The number of actual
 * partitions will be stored in @a count.  If there are more partitions than
 * space for storage an error status will be returned.  The partition table
 * format recognized on the disk will be stored in @a format.
 */
rtems_status_code rtems_bdpart_read(
  const char *disk_name,
  rtems_bdpart_format *format,
  rtems_bdpart_partition *partitions,
  size_t *count
);

/**
 * Sorts the partition table @a partitions with @a count partitions to have
 * ascending begin blocks
 */
void rtems_bdpart_sort( rtems_bdpart_partition *partitions, size_t count);

/**
 * Writes the partition table to the physical disk device with name
 * @a disk_name.
 *
 * The partition table @a partitions with @a count partitions will be written
 * to the disk.  The output format for the partition table on the disk is
 * specified by @a format.  There are some consistency checks applied to the
 * partition table.  The partition table must be sorted such that the begin
 * blocks are in ascending order.  This can be done with the
 * rtems_bdpart_sort() function.  The partitions must not overlap.  The
 * partitions must have a positive size.  The partitions must be within the
 * disk.  Depending on the output format there are additional constrains.
 */
rtems_status_code rtems_bdpart_write(
  const char *disk_name,
  const rtems_bdpart_format *format,
  const rtems_bdpart_partition *partitions,
  size_t count
);

/**
 * Creates a partition table in @a partitions with @a count partitions for the
 * physical disk device with name @a disk_name.
 *
 * The array of positive integer weights in @a distribution must have exactly
 * @a count elements.  The weights in the distribution array are summed up.
 * Each weight is then divided by the sum to obtain the disk fraction which
 * forms the corresponding partition.  The partition boundaries are generated
 * with respect to the output format in @a format.
 */
rtems_status_code rtems_bdpart_create(
  const char *disk_name,
  const rtems_bdpart_format *format,
  rtems_bdpart_partition *partitions,
  const unsigned *distribution,
  size_t count
);

/**
 * Registers the partitions as logical disks for the physical disk device with
 * name @a disk_name.
 *
 * For each partition of the partition table @a partitions with @a count
 * partitions a logical disk is registered.  The partition number equals the
 * partition table index plus one.  The name of the logical disk device is the
 * concatenation of the physical disk device name and the partition number.
 */
rtems_status_code rtems_bdpart_register(
  const char *disk_name,
  const rtems_bdpart_partition *partitions,
  size_t count
);

/**
 * Reads the partition table from the disk device with name @a disk_name and
 * registers the partitions as logical disks.
 *
 * @see rtems_bdpart_register() and rtems_fsmount().
 */
rtems_status_code rtems_bdpart_register_from_disk( const char *disk_name);

/**
 * Deletes the logical disks associated with the partitions of the disk device
 * with name @a disk_name.
 *
 * The partition table @a partitions with @a count partitions will be used to
 * determine which disks need to be deleted.  It may be obtained from
 * rtems_bdpart_read().
 */
rtems_status_code rtems_bdpart_unregister(
  const char *disk_name,
  const rtems_bdpart_partition *partitions,
  size_t count
);

/**
 * Mounts all supported file systems inside the logical disks derived from the
 * partitions of the physical disk device with name @a disk_name.
 *
 * For each partition in the partition table @a partitions with @a count
 * partitions it will be checked if it contains a supported file system.  In
 * this case a mount point derived from the disk name will be created in the
 * mount base path @a mount_base.  The file system will be mounted there.  The
 * partition number equals the partition table index plus one.  The mount point
 * name for each partition will be the concatenation of the mount base path,
 * the disk device file name and the parition number.
 *
 * @see rtems_bdpart_read().
 */
rtems_status_code rtems_bdpart_mount(
  const char *disk_name,
  const rtems_bdpart_partition *partitions,
  size_t count,
  const char *mount_base
);

/**
 * Unmounts all file systems mounted with rtems_bdpart_mount().
 */
rtems_status_code rtems_bdpart_unmount(
  const char *disk_name,
  const rtems_bdpart_partition *partitions,
  size_t count,
  const char *mount_base
);

/**
 * Prints the partition table @a partitions with @a count partitions to
 * standard output.
 */
void rtems_bdpart_dump( const rtems_bdpart_partition *partitions, size_t count);

/**
 * Returns the partition type for the MBR partition type value @a mbr_type in
 * @a type.
 */
void rtems_bdpart_to_partition_type( uint8_t mbr_type, uuid_t type);

/**
 * Converts the partition type in @a type to the MBR partition type.
 *
 * The result will be stored in @a mbr_type.  Returns @c true in case of a
 * successful convertion and otherwise @c false.  Both arguments must not be
 * @c NULL.
 */
bool rtems_bdpart_to_mbr_partition_type(
  const uuid_t type,
  uint8_t *mbr_type
);

/** @} */

#define RTEMS_BDPART_MBR_CYLINDER_SIZE 63

#define RTEMS_BDPART_NUMBER_SIZE 4

#define RTEMS_BDPART_BLOCK_SIZE 512

#define RTEMS_BDPART_MBR_TABLE_ENTRY_SIZE 16

#define RTEMS_BDPART_MBR_OFFSET_TABLE_0 446

#define RTEMS_BDPART_MBR_OFFSET_TABLE_1 \
  (RTEMS_BDPART_MBR_OFFSET_TABLE_0 + RTEMS_BDPART_MBR_TABLE_ENTRY_SIZE)

#define RTEMS_BDPART_MBR_OFFSET_DISK_ID 440

#define RTEMS_BDPART_MBR_OFFSET_SIGNATURE_0 510

#define RTEMS_BDPART_MBR_OFFSET_SIGNATURE_1 511

#define RTEMS_BDPART_MBR_SIGNATURE_0 0x55U

#define RTEMS_BDPART_MBR_SIGNATURE_1 0xaaU

#define RTEMS_BDPART_MBR_OFFSET_BEGIN 8

#define RTEMS_BDPART_MBR_OFFSET_SIZE 12

#define RTEMS_BDPART_MBR_OFFSET_TYPE 4

#define RTEMS_BDPART_MBR_OFFSET_FLAGS 0

static inline uint8_t rtems_bdpart_mbr_partition_type(
  const uuid_t type
)
{
  return type [0];
}

rtems_status_code rtems_bdpart_get_disk_data(
  const char *disk_name,
  int *fd_ptr,
  rtems_disk_device **dd_ptr,
  rtems_blkdev_bnum *disk_end
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_BDPART_H */
