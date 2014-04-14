/**
 * @file
 * 
 * @brief "MS-DOS-style" Partition Tables Support
 */

/*
 * Copyright (C) 2002 OKTET Ltd., St.-Petersburg, Russia
 *
 * Author: Konstantin Abramenko <Konstantin.Abramenko@oktet.ru>
 *         Alexander Kukuta <Alexander.Kukuta@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *****************************************************************************/

#ifndef _RTEMS_IDE_PART_TABLE_H
#define _RTEMS_IDE_PART_TABLE_H

#include <rtems/chain.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <rtems.h>
#include <rtems/blkdev.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/bdbuf.h>
#include <rtems/seterr.h>

/* Minor base number for all logical devices */
#define RTEMS_IDE_SECTOR_BITS                             9
#define RTEMS_IDE_SECTOR_SIZE                             512
#define RTEMS_IDE_PARTITION_DESCRIPTOR_SIZE               16
#define RTEMS_IDE_PARTITION_MAX_PARTITION_NUMBER          63
#define RTEMS_IDE_PARTITION_MAX_SUB_PARTITION_NUMBER      4
#define RTEMS_IDE_PARTITION_DEV_NAME_LENGTH_MAX           16

#define RTEMS_IDE_PARTITION_MSDOS_SIGNATURE_DATA1         0x55
#define RTEMS_IDE_PARTITION_MSDOS_SIGNATURE_DATA2         0xaa
#define RTEMS_IDE_PARTITION_MSDOS_SIGNATURE_OFFSET        0x1fe
#define RTEMS_IDE_PARTITION_TABLE_OFFSET                  0x1be
#define RTEMS_IDE_PARTITION_TABLE_SIZE                    (4 * 16)
#define RTEMS_IDE_PARTITION_BOOTABLE_OFFSET               0
#define RTEMS_IDE_PARTITION_SYS_TYPE_OFFSET               4
#define RTEMS_IDE_PARTITION_START_OFFSET                  8
#define RTEMS_IDE_PARTITION_SIZE_OFFSET                   12

/*
 * Conversion from and to little-endian byte order. (no-op on i386/i486)
 */

#if (CPU_BIG_ENDIAN == TRUE)
#   define LE_TO_CPU_U16(v) CPU_swap_u16(v)
#   define LE_TO_CPU_U32(v) CPU_swap_u32(v)
#   define CPU_TO_LE_U16(v) CPU_swap_u16(v)
#   define CPU_TO_LE_U32(v) CPU_swap_u32(v)
#else
#   define LE_TO_CPU_U16(v) (v)
#   define LE_TO_CPU_U32(v) (v)
#   define CPU_TO_LE_U16(v) (v)
#   define CPU_TO_LE_U32(v) (v)
#endif


/*
 * sector_data_t --
 *      corresponds to the sector on the device
 */
typedef struct rtems_sector_data_s
{
    uint32_t   sector_num; /* sector number on the device */
    uint8_t    data[RTEMS_ZERO_LENGTH_ARRAY]; /* raw sector data */
} rtems_sector_data_t;


/*
 * Enum partition types
 * see list at http://ata-atapi.com/hiwtab.htm
 *
 * @todo Should these have RTEMS before them.
 */
enum {
    EMPTY_PARTITION     = 0x00,
    DOS_FAT12_PARTITION = 0x01,
    DOS_FAT16_PARTITION = 0x04,
    EXTENDED_PARTITION  = 0x05,
    DOS_P32MB_PARTITION = 0x06,
    FAT32_PARTITION     = 0x0B,
    FAT32_LBA_PARTITION = 0x0C,
    FAT16_LBA_PARTITION = 0x0E,
    DM6_PARTITION       = 0x54,
    EZD_PARTITION       = 0x55,
    DM6_AUX1PARTITION   = 0x51,
    DM6_AUX3PARTITION   = 0x53,
    LINUX_SWAP          = 0x82,
    LINUX_NATIVE        = 0x83,
    LINUX_EXTENDED      = 0x85
};


/* Forward declaration */
struct rtems_disk_desc_s;

/*
 * part_desc_t --
 *      contains all neccessary information about partition
 */
typedef struct rtems_part_desc_s {
    uint8_t             bootable; /* is the partition active */
    uint8_t             sys_type; /* type of partition */
    uint8_t             log_id; /* logical number of partition */
    uint32_t            start; /* first partition sector, in absolute
                                * numeration */
    uint32_t            size; /* size in sectors */
    uint32_t            end; /* last partition sector, end = start + size - 1 */
    struct rtems_disk_desc_s *disk_desc; /* descriptor of disk, partition
                                          * contains in */
    struct rtems_part_desc_s *ext_part; /* extended partition containing this
                                         * one */

    /* partitions, containing in this one */
    struct rtems_part_desc_s *sub_part[RTEMS_IDE_PARTITION_MAX_SUB_PARTITION_NUMBER];
} rtems_part_desc_t;



typedef struct rtems_disk_desc_s {
    dev_t        dev; /* device number */

    /* device name in /dev filesystem */
    char         dev_name[RTEMS_IDE_PARTITION_DEV_NAME_LENGTH_MAX];

    uint32_t     sector_size; /* size of sector */
    uint32_t     sector_bits; /* the base-2 logarithm of sector_size */
    uint32_t     lba_size; /* total amount of sectors in lba address mode */
    int          last_log_id; /* used for logical disks enumerating */

    /* primary partition descriptors */
    rtems_part_desc_t *partitions[RTEMS_IDE_PARTITION_MAX_PARTITION_NUMBER];
} rtems_disk_desc_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * rtems_ide_part_table_free --
 *      frees disk descriptor structure
 *
 * PARAMETERS:
 *      disk_desc - disc descriptor structure to free
 *
 * RETURNS:
 *      N/A
 */
/**
 * @deprecated Use the @ref rtems_bdpart "block device partition module" instead.
 */
void rtems_ide_part_table_free(
  rtems_disk_desc_t *disk_desc
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;


/*
 * rtems_ide_part_table_get --
 *      reads partition table structure from the device
 *      and creates disk description structure
 *
 * PARAMETERS:
 *      dev_name  - path to physical device in /dev filesystem
 *      disk_desc - returned disc description structure
 *
 * RETURNS:
 *      RTEMS_SUCCESSFUL if success, or -1 and corresponding errno else
 */
/**
 * @deprecated Use the @ref rtems_bdpart "block device partition module" instead.
 */
rtems_status_code rtems_ide_part_table_get(
  const char *dev_name,
  rtems_disk_desc_t *disk_desc
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;


/*
 * rtems_ide_part_table_initialize --
 *      initializes logical devices on the physical IDE drive
 *
 * PARAMETERS:
 *      dev_name - path to physical device in /dev filesystem
 *
 * RETURNS:
 *      RTEMS_SUCCESSFUL if success, or -1 and corresponding errno else
 */
/**
 * @deprecated Use the @ref rtems_bdpart "block device partition module" instead.
 */
rtems_status_code rtems_ide_part_table_initialize(
  const char *dev_name
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_IDE_PART_TABLE_H */
