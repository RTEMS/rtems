/*****************************************************************************
 *
 * ide_part_table.c
 *
 * The implementation of library supporting "MS-DOS-style" partition table
 *
 *
 * Copyright (C) 2002 OKTET Ltd., St.-Petersburg, Russia
 *
 * Author: Konstantin Abramenko <Konstantin.Abramenko@oktet.ru>
 *         Alexander Kukuta <Alexander.Kukuta@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 *
 *****************************************************************************/

#include <rtems/ide_part_table.h>


/*
 * get_sector --
 *      gets sector from the disk
 *
 * PARAMETERS:
 *      dev        - device number
 *      sector_num - number of sector to read
 *      sector     - returned pointer to pointer to allocated
 *                   sector_data_t structure
 *
 * RETURNS:
 *      RTEMS_SUCCESSFUL, if success;
 *      RTEMS_NO_MEMORY, if canot allocate memory for sector data;
 *      other error codes returned by rtems_bdbuf_read().
 *
 * NOTES:
 *      get_sector() operates with device via bdbuf library,
 *      and does not support devices with sector size other than 512 bytes
 */
static rtems_status_code
get_sector(dev_t dev, unsigned32 sector_num, sector_data_t **sector) 
{
    sector_data_t      *s;
    bdbuf_buffer       *buf;
    rtems_status_code   rc;
    
    if (sector == NULL)
    {
        return RTEMS_INTERNAL_ERROR;
    }

    s = (sector_data_t *) malloc(sizeof(sector_data_t) + RTEMS_IDE_SECTOR_SIZE);
    if (s == NULL)
    {
        return RTEMS_NO_MEMORY;
    }
    
    rc = rtems_bdbuf_read(dev, sector_num, &buf);
    if (rc != RTEMS_SUCCESSFUL)
    {
        free(s);
        return rc;
    }
    
    memcpy(s->data, buf->buffer, RTEMS_IDE_SECTOR_SIZE);
    s->sector_num = sector_num;

    *sector = s;
    
    rtems_bdbuf_release(buf);
    
    return RTEMS_SUCCESSFUL;
}


/*
 * msdos_signature_check --
 *      checks if the partition table sector has msdos signature
 *
 * PARAMETERS:
 *      sector - sector to check
 *
 * RETURNS:
 *      TRUE if sector has msdos signature, FALSE otherwise
 */
static rtems_boolean
msdos_signature_check (sector_data_t *sector) 
{
    unsigned8 *p = sector->data + RTEMS_IDE_PARTITION_MSDOS_SIGNATURE_OFFSET;

    return ((p[0] == RTEMS_IDE_PARTITION_MSDOS_SIGNATURE_DATA1) &&
            (p[1] == RTEMS_IDE_PARTITION_MSDOS_SIGNATURE_DATA2));
}


/*
 * is_extended --
 *      checks if the partition type is extended
 *
 * PARAMETERS:
 *      type - type of partition to check
 *
 * RETURNS:
 *      TRUE if partition type is extended, FALSE otherwise
 */
static rtems_boolean
is_extended(unsigned8 type) 
{
    return ((type == EXTENDED_PARTITION) || (type == LINUX_EXTENDED));
}


/*
 * data_to_part_desc --
 *      parses raw partition table sector data
 *      to partition description structure
 *
 * PARAMETERS:
 *      data          - raw partition table sector data
 *      new_part_desc - pointer to returned partition description structure
 *
 * RETURNS:
 *      RTEMS_SUCCESSFUL, if success;
 *      RTEMS_NO_MEMOTY, if cannot allocate memory for part_desc_t strucure;
 *      RTEMS_INTERNAL_ERROR, if other error occurs.
 */
static rtems_status_code
data_to_part_desc(unsigned8 *data, part_desc_t **new_part_desc)
{
    part_desc_t *part_desc;
    unsigned32   temp;

    if (new_part_desc == NULL)
    {
        return RTEMS_INTERNAL_ERROR;
    }
    
    *new_part_desc = NULL;

    if ((part_desc = calloc(1, sizeof(part_desc_t))) == NULL)
    {
        return RTEMS_NO_MEMORY;
    }

    part_desc->bootable = *(data + RTEMS_IDE_PARTITION_BOOTABLE_OFFSET);
    part_desc->sys_type = *(data + RTEMS_IDE_PARTITION_SYS_TYPE_OFFSET);

    /* read the offset start position and partition size in sectors */

    /* due to incorrect data alignment one have to align data first */
    memcpy(&temp, data + RTEMS_IDE_PARTITION_START_OFFSET, sizeof(unsigned32));
    part_desc->start = LE_TO_CPU_U32(temp);

    memcpy(&temp, data + RTEMS_IDE_PARTITION_SIZE_OFFSET, sizeof(unsigned32));
    part_desc->size = LE_TO_CPU_U32(temp);

    if ((part_desc->sys_type == EMPTY_PARTITION) ||
        ((part_desc->size == 0) && (!is_extended(part_desc->sys_type))))
    {
        /* empty partition */
        free(part_desc);
        return RTEMS_SUCCESSFUL;
    }
    
    *new_part_desc = part_desc;

    return RTEMS_SUCCESSFUL;
}


/*
 * read_extended_partition --
 *      recursively reads extended partition sector from the device
 *      and constructs the partition table tree
 *
 * PARAMETERS:
 *      start    - start sector of primary extended partition, used for 
 *                 calculation of absolute partition sector address
 *      ext_part - description of extended partition to process
 *
 * RETURNS:
 *      RTEMS_SUCCESSFUL if success,
 *      RTEMS_NO_MEMOTY if cannot allocate memory for part_desc_t strucure,
 *      RTEMS_INTERNAL_ERROR if other error occurs.
 */
static rtems_status_code
read_extended_partition(unsigned32 start, part_desc_t *ext_part) 
{
    int                 i;
    dev_t               dev;
    sector_data_t      *sector;
    unsigned32          here;
    unsigned8          *data;
    part_desc_t        *new_part_desc;
    rtems_status_code   rc;

    if ((ext_part == NULL) || (ext_part->disk_desc == NULL))
    {
        return RTEMS_INTERNAL_ERROR;
    }
    
    dev = ext_part->disk_desc->dev;

    /* get start sector of current extended partition */
    here = ext_part->start;

    /* get first extended partition sector */

    rc = get_sector(dev, here, &sector);
    if (rc != RTEMS_SUCCESSFUL)
    {
        return rc;
    }

    if (!msdos_signature_check(sector))
    {
        return RTEMS_INTERNAL_ERROR;
    }

    /* read and process up to 4 logical partition descriptors */

    data = sector->data + RTEMS_IDE_PARTITION_TABLE_OFFSET;

    for (i = 0; i < RTEMS_IDE_PARTITION_MAX_SUB_PARTITION_NUMBER; i++) 
    {
        /* if data_to_part_desc fails skip this partition
         * and parse the next one
         */
        rc = data_to_part_desc(data, &new_part_desc);
        if (rc != RTEMS_SUCCESSFUL)
        {
            free(sector);
            return rc;
        }
        
        if (new_part_desc == NULL)
        {
            data += RTEMS_IDE_PARTITION_DESCRIPTOR_SIZE;
            continue;
        }

        ext_part->sub_part[i] = new_part_desc;
        new_part_desc->ext_part = ext_part;
        new_part_desc->disk_desc = ext_part->disk_desc;

        if (is_extended(new_part_desc->sys_type)) 
        {
            new_part_desc->log_id = EMPTY_PARTITION;
            new_part_desc->start += start;
            read_extended_partition(start, new_part_desc); 
        }
        else 
        {
            disk_desc_t *disk_desc = new_part_desc->disk_desc;
            disk_desc->partitions[disk_desc->last_log_id] = new_part_desc;
            new_part_desc->log_id = ++disk_desc->last_log_id;
            new_part_desc->start += here;
            new_part_desc->end = new_part_desc->start + new_part_desc->size - 1;
        }
        data += RTEMS_IDE_PARTITION_DESCRIPTOR_SIZE;
    }

    free(sector);
    
    return RTEMS_SUCCESSFUL;
}


/*
 * read_mbr --
 *      reads Master Boot Record (sector 0) of physical device and
 *      constructs disk description structure
 *
 * PARAMETERS:
 *      disk_desc - returned disc description structure
 *
 * RETURNS:
 *      RTEMS_SUCCESSFUL if success,
 *      RTEMS_INTERNAL_ERROR otherwise
 */
static rtems_status_code
read_mbr(disk_desc_t *disk_desc) 
{
    int                 part_num;
    sector_data_t      *sector;
    part_desc_t        *part_desc;
    unsigned8          *data;
    rtems_status_code   rc;
    dev_t               dev = disk_desc->dev;

    /* get MBR sector */
    rc = get_sector(dev, 0, &sector);
    if (rc != RTEMS_SUCCESSFUL)
    {
        return rc;
    }

    /* check if the partition table structure is MS-DOS style */
    if (!msdos_signature_check(sector))
    {
        return RTEMS_INTERNAL_ERROR;
    }

    /* read and process 4 primary partition descriptors */

    data = sector->data + RTEMS_IDE_PARTITION_TABLE_OFFSET;

    for (part_num = 0;
         part_num < RTEMS_IDE_PARTITION_MAX_SUB_PARTITION_NUMBER;
         part_num++)
    {
        rc = data_to_part_desc(data, &part_desc);
        if (rc != RTEMS_SUCCESSFUL)
        {
            free(sector);
            return rc;
        }

        if (part_desc != NULL) 
        {
            part_desc->log_id = part_num + 1;
            part_desc->disk_desc = disk_desc;
            part_desc->end = part_desc->start + part_desc->size - 1;
            disk_desc->partitions[part_num] = part_desc;
        } 
        else
        {
            disk_desc->partitions[part_num] = NULL;
        }

        data += RTEMS_IDE_PARTITION_DESCRIPTOR_SIZE;
    }

    free(sector);
    
    disk_desc->last_log_id = RTEMS_IDE_PARTITION_MAX_SUB_PARTITION_NUMBER;

    /* There cannot be more than one extended partition,
       but we are to process each primary partition */
    for (part_num = 0;
         part_num < RTEMS_IDE_PARTITION_MAX_SUB_PARTITION_NUMBER;
         part_num++)
    {
        if (is_extended(disk_desc->partitions[part_num]->sys_type))
        {
            read_extended_partition(disk_desc->partitions[part_num]->start,
                                    disk_desc->partitions[part_num]);
        }
    }

    return RTEMS_SUCCESSFUL;
}


/*
 * partition free --
 *      frees partition description structure
 *
 * PARAMETERS:
 *      part_desc - returned disc description structure
 *
 * RETURNS:
 *      N/A
 */
static void
partition_free(part_desc_t *part_desc)
{
    int part_num;

    if (part_desc == NULL)
        return;
    
    if (is_extended(part_desc->sys_type))
    {
        for (part_num = 0;
             part_num < RTEMS_IDE_PARTITION_MAX_SUB_PARTITION_NUMBER;
             part_num++)
        {
            partition_free(part_desc->sub_part[part_num]);
        }
    }
    
    free(part_desc);
}


/*
 * rtems_ide_part_table_free - frees disk descriptor structure
 *
 * PARAMETERS:
 *      disk_desc - disc descriptor structure to free
 *
 * RETURNS:
 *      N/A
 */
void
rtems_ide_part_table_free(disk_desc_t *disk_desc)
{
    int part_num;

    for (part_num = 0; part_num < RTEMS_IDE_PARTITION_MAX_SUB_PARTITION_NUMBER; part_num++)
    {
        partition_free(disk_desc->partitions[part_num]);
    }
    
    free(disk_desc);
}


/*
 * rtems_ide_part_table_get - reads partition table structure from the device
 *                            and creates disk description structure
 *
 * PARAMETERS:
 *      dev_name - path to physical device in /dev filesystem
 *      disk_desc       - returned disc description structure
 *
 * RETURNS:
 *      RTEMS_SUCCESSFUL if success,
 *      RTEMS_INTERNAL_ERROR otherwise
 */
rtems_status_code
rtems_ide_part_table_get(const char *dev_name, disk_desc_t *disk_desc)
{
    struct stat         dev_stat;
    rtems_status_code   rc;

    rc = stat(dev_name, &dev_stat);
    if (rc != RTEMS_SUCCESSFUL)
    {
        return RTEMS_INTERNAL_ERROR;
    }

    strncpy (disk_desc->dev_name, dev_name, 15);
    disk_desc->dev = dev_stat.st_dev;
    disk_desc->sector_size = (dev_stat.st_blksize) ? dev_stat.st_blksize :
                                              RTEMS_IDE_SECTOR_SIZE;

    rc = read_mbr(disk_desc);

    return rc;
}


/*
 * rtems_ide_part_table_initialize - initializes logical devices
 *                                   on the physical IDE drive
 *
 * PARAMETERS:
 *      dev_name - path to physical device in /dev filesystem
 *
 * RETURNS:
 *      RTEMS_SUCCESSFUL if success,
 *      RTEMS_NO_MEMOTY if cannot have not enough memory,
 *      RTEMS_INTERNAL_ERROR if other error occurs.
 */
rtems_status_code
rtems_ide_part_table_initialize(char *dev_name)
{
    int                         part_num;
    dev_t                       dev;
    disk_desc_t                *disk_desc;
    rtems_device_major_number   major;
    rtems_device_minor_number   minor;
    rtems_status_code           rc;
    part_desc_t                *part_desc;

    /* logical device name /dev/hdxyy */
    char                        name[RTEMS_IDE_PARTITION_DEV_NAME_LENGTH_MAX];

    disk_desc = (disk_desc_t *) calloc(1, sizeof(disk_desc_t));
    if (disk_desc == NULL)
    {
        return RTEMS_NO_MEMORY;
    }
    
    /* get partition table */
    rc = rtems_ide_part_table_get(dev_name, disk_desc);
    if (rc != RTEMS_SUCCESSFUL)
    {
        return rc;
    }

    /* To avoid device numbers conflicts we have to use for logic disk the same
     * device major number as ATA device has, and minor number that equals to
     * sum of logic disk partition number and the minor number of physical disk
     */

    rtems_filesystem_split_dev_t (disk_desc->dev, major, minor);

    /* create logical disks on the physical one */
    for (part_num = 0; part_num < disk_desc->last_log_id; part_num++)
    {
        sprintf(name, "%s%d", dev_name, part_num + 1);
        dev = rtems_filesystem_make_dev_t(major, ++minor);

        part_desc = disk_desc->partitions[part_num];
        if (part_desc == NULL)
        {
            continue;
        }

        rc = rtems_disk_create_log(dev, disk_desc->dev, part_desc->start,
                                   part_desc->size, name);
        if (rc != RTEMS_SUCCESSFUL)
        {
            printf("Cannot create device %s, error code %d\n", name, rc);
            continue;
        }
    }

    rtems_ide_part_table_free(disk_desc);

    return RTEMS_SUCCESSFUL;
}
