/* 
 * diskdevs.c - Physical and logical block devices (disks) support
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) $Id$
 */


#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <string.h>

#include "rtems/diskdevs.h"
#include "rtems/bdbuf.h"

#define DISKTAB_INITIAL_SIZE 32

/* Table of disk devices having the same major number */
struct disk_device_table {
    disk_device **minor; /* minor-indexed disk device table */
    int size;            /* Number of entries in the table */
};

/* Pointer to [major].minor[minor] indexed array of disk devices */
static struct disk_device_table *disktab;

/* Number of allocated entries in disktab table */
static int disktab_size;

/* Mutual exclusion semaphore for disk devices table */
static rtems_id diskdevs_mutex;

/* Flag meaning that disk I/O, buffering etc. already has been initialized. */
static boolean disk_io_initialized = FALSE;

/* diskdevs data structures protection flag.
 * Normally, only table lookup operations performed. It is quite fast, so
 * it is possible to done lookups when interrupts are disabled, avoiding
 * obtaining the semaphore. This flags sets immediately after entering in
 * mutex-protected section and cleared before leaving this section in
 * "big" primitives like add/delete new device etc. Lookup function first
 * disable interrupts and check this flag. If it is set, lookup function
 * will be blocked on semaphore and lookup operation will be performed in
 * semaphore-protected code. If it is not set (very-very frequent case),
 * we can do lookup safely, enable interrupts and return result.
 */
static volatile rtems_boolean diskdevs_protected;

/* create_disk_entry --
 *     Return pointer to the disk_entry structure for the specified device, or
 *     create one if it is not exists.
 *
 * PARAMETERS:
 *     dev - device id (major, minor)
 *
 * RETURNS:
 *     pointer to the disk device descirptor entry, or NULL if no memory
 *     available for its creation.
 */
static disk_device *
create_disk_entry(dev_t dev)
{
    rtems_device_major_number major;
    rtems_device_minor_number minor;
    struct disk_device **d;

    rtems_filesystem_split_dev_t (dev, major, minor);
    
    if (major >= disktab_size)
    {
        struct disk_device_table *p;
        int newsize;
        int i;
        newsize = disktab_size * 2;
        if (major >= newsize)
            newsize = major + 1;
        p = realloc(disktab, sizeof(struct disk_device_table) * newsize);
        if (p == NULL)
            return NULL;
        p += disktab_size;
        for (i = disktab_size; i < newsize; i++, p++)
        {
            p->minor = NULL;
            p->size = 0;
        }
        disktab_size = newsize;
    }
    
    if ((disktab[major].minor == NULL) ||
        (minor >= disktab[major].size))
    {
        int newsize;
        disk_device **p;
        int i;
        int s = disktab[major].size;
        
        if (s == 0)
            newsize = DISKTAB_INITIAL_SIZE;
        else
            newsize = s * 2;
        if (minor >= newsize)
            newsize = minor + 1;
        
        p = realloc(disktab[major].minor, sizeof(disk_device *) * newsize);
        if (p == NULL)
            return NULL;
        disktab[major].minor = p;
        p += s;
        for (i = s; i < newsize; i++, p++)
            *p = NULL;
        disktab[major].size = newsize;
    }
    
    d = disktab[major].minor + minor;
    if (*d == NULL)
    {
        *d = calloc(1, sizeof(disk_device));
    }
    return *d;
}

/* get_disk_entry --
 *     Get disk device descriptor by device number.
 *
 * PARAMETERS:
 *     dev - block device number
 *
 * RETURNS:
 *     Pointer to the disk device descriptor corresponding to the specified
 *     device number, or NULL if disk device with such number not exists.
 */
static inline disk_device *
get_disk_entry(dev_t dev)
{
    rtems_device_major_number major;
    rtems_device_minor_number minor;
    struct disk_device_table *dtab;

    rtems_filesystem_split_dev_t (dev, major, minor);
    
    if ((major >= disktab_size) || (disktab == NULL))
        return NULL;
        
    dtab = disktab + major;
    
    if ((minor >= dtab->size) || (dtab->minor == NULL))
        return NULL;
    
    return dtab->minor[minor];
}

/* create_disk --
 *     Check that disk entry for specified device number is not defined
 *     and create it.
 *
 * PARAMETERS:
 *     dev        - device identifier (major, minor numbers)
 *     name       - character name of device (e.g. /dev/hda)
 *     disdev     - placeholder for pointer to created disk descriptor
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if disk entry successfully created, or
 *     error code if error occured (device already registered,
 *     no memory available).
 */
static rtems_status_code
create_disk(dev_t dev, char *name, disk_device **diskdev)
{
    disk_device *dd;
    char *n;
    
    dd = get_disk_entry(dev);
    if (dd != NULL)
    {
        return RTEMS_RESOURCE_IN_USE;
    }
    
    if (name == NULL)
    {
        n = NULL;
    }
    else
    {
        int nlen = strlen(name) + 1;
        n = malloc(nlen);
        if (n == NULL)
            return RTEMS_NO_MEMORY;
        strncpy(n, name, nlen);
    }
    
    dd = create_disk_entry(dev);
    if (dd == NULL)
    {
        free(n);
        return RTEMS_NO_MEMORY;
    }
    
    dd->dev = dev;
    dd->name = n;
    
    *diskdev = dd;
    
    return RTEMS_SUCCESSFUL;
}

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
                       char *name)
{
    int bs_log2;
    int i;
    disk_device *dd;
    rtems_status_code rc;
    rtems_bdpool_id pool;
    rtems_device_major_number major;
    rtems_device_minor_number minor;

    rtems_filesystem_split_dev_t (dev, major, minor);

    
    for (bs_log2 = 0, i = block_size; (i & 1) == 0; i >>= 1, bs_log2++);
    if ((bs_log2 < 9) || (i != 1)) /* block size < 512 or not power of 2 */
        return RTEMS_INVALID_NUMBER;
    
    rc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (rc != RTEMS_SUCCESSFUL)
        return rc;
    diskdevs_protected = TRUE;

    rc = rtems_bdbuf_find_pool(block_size, &pool);
    if (rc != RTEMS_SUCCESSFUL)
    {
        diskdevs_protected = FALSE;
        rtems_semaphore_release(diskdevs_mutex);
        return rc;
    }
    
    rc = create_disk(dev, name, &dd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        diskdevs_protected = FALSE;
        rtems_semaphore_release(diskdevs_mutex);
        return rc;
    }
    
    dd->phys_dev = dd;
    dd->uses = 0;
    dd->start = 0;
    dd->size = disk_size;
    dd->block_size = block_size;
    dd->block_size_log2 = bs_log2;
    dd->ioctl = handler;
    dd->pool = pool;

    rc = rtems_io_register_name(name, major, minor);
    
    diskdevs_protected = FALSE;
    rtems_semaphore_release(diskdevs_mutex);

    return rc;
}

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
rtems_disk_create_log(dev_t dev, dev_t phys, int start, int size, char *name)
{
    disk_device *dd;
    disk_device *pdd;
    rtems_status_code rc;
    rtems_device_major_number major;
    rtems_device_minor_number minor;

    rtems_filesystem_split_dev_t (dev, major, minor);

    rc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (rc != RTEMS_SUCCESSFUL)
        return rc;
    diskdevs_protected = TRUE;

    pdd = get_disk_entry(phys);
    if (pdd == NULL)
    {
        diskdevs_protected = FALSE;
        rtems_semaphore_release(diskdevs_mutex);
        return RTEMS_INVALID_NUMBER;
    }
    
    rc = create_disk(dev, name, &dd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        diskdevs_protected = FALSE;
        rtems_semaphore_release(diskdevs_mutex);
        return rc;
    }
    
    dd->phys_dev = pdd;
    dd->uses = 0;
    dd->start = start;
    dd->size = size;
    dd->block_size = pdd->block_size;
    dd->block_size_log2 = pdd->block_size_log2;
    dd->ioctl = pdd->ioctl;

    rc = rtems_io_register_name(name, major, minor);

    diskdevs_protected = FALSE;
    rc = rtems_semaphore_release(diskdevs_mutex);
    
    return rc;
}

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
rtems_disk_delete(dev_t dev)
{
    rtems_status_code rc;
    int used;
    rtems_device_major_number maj;
    rtems_device_minor_number min;
    
    rc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (rc != RTEMS_SUCCESSFUL)
        return rc;
    diskdevs_protected = TRUE;

    /* Check if this device is in use -- calculate usage counter */
    used = 0;
    for (maj = 0; maj < disktab_size; maj++)
    {
        struct disk_device_table *dtab = disktab + maj;
        if (dtab != NULL)
        {
            for (min = 0; min < dtab->size; min++)
            {
                disk_device *dd = dtab->minor[min];
                if ((dd != NULL) && (dd->phys_dev->dev == dev))
                    used += dd->uses;
            }
        }
    }
    
    if (used != 0)
    {
        diskdevs_protected = FALSE;
        rtems_semaphore_release(diskdevs_mutex);
        return RTEMS_RESOURCE_IN_USE;
    }

    /* Delete this device and all of its logical devices */
    for (maj = 0; maj < disktab_size; maj++)
    {
        struct disk_device_table *dtab = disktab +maj;
        if (dtab != NULL)
        {
            for (min = 0; min < dtab->size; min++)
            {
                disk_device *dd = dtab->minor[min];
                if ((dd != NULL) && (dd->phys_dev->dev == dev))
                {
                    unlink(dd->name);
                    free(dd->name);
                    free(dd);
                    dtab->minor[min] = NULL;
                }
            }
        }
    }
    
    diskdevs_protected = FALSE;
    rc = rtems_semaphore_release(diskdevs_mutex);
    return rc;
}

/* rtems_disk_lookup --
 *     Find block device descriptor by its device identifier.
 *
 * PARAMETERS:
 *     dev - device identifier (major, minor numbers)
 *
 * RETURNS:
 *     pointer to the block device descriptor, or NULL if no such device
 *     exists.
 */
disk_device *
rtems_disk_lookup(dev_t dev)
{
    rtems_interrupt_level level;
    disk_device *dd;
    rtems_status_code rc;
    
    rtems_interrupt_disable(level);
    if (diskdevs_protected)
    {
        rtems_interrupt_enable(level);
        rc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT, 
                                    RTEMS_NO_TIMEOUT);
        if (rc != RTEMS_SUCCESSFUL)
            return NULL;
        diskdevs_protected = TRUE;
        dd = get_disk_entry(dev);
        dd->uses++;
        diskdevs_protected = FALSE;
        rtems_semaphore_release(diskdevs_mutex);
        return dd;
    }
    else
    {
        /* Frequent and quickest case */
        dd = get_disk_entry(dev);
        dd->uses++;
        rtems_interrupt_enable(level);
        return dd;
    }
}

/* rtems_disk_release --
 *     Release disk_device structure (decrement usage counter to 1).
 *
 * PARAMETERS:
 *     dd - pointer to disk device structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
rtems_status_code
rtems_disk_release(disk_device *dd)
{
    rtems_interrupt_level level;
    rtems_interrupt_disable(level);
    dd->uses--;
    rtems_interrupt_enable(level);
    return RTEMS_SUCCESSFUL;
}

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
 *     devices enumerated.
 */
disk_device *
rtems_disk_next(dev_t dev)
{
    rtems_device_major_number major;
    rtems_device_minor_number minor;
    struct disk_device_table *dtab;

    dev++;
    rtems_filesystem_split_dev_t (dev, major, minor);

    if (major >= disktab_size)
        return NULL;
    
    dtab = disktab + major;
    while (TRUE) 
    {
        if ((dtab == NULL) || (minor > dtab->size))
        {
             major++; minor = 0;
             if (major >= disktab_size)
                 return NULL;
             dtab = disktab + major;
        }
        else if (dtab->minor[minor] == NULL)
        {
            minor++;
        }
        else
            return dtab->minor[minor];
    }
}

/* rtems_disk_initialize --
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
rtems_disk_io_initialize(void)
{
    rtems_status_code rc;

    if (disk_io_initialized)
        return RTEMS_SUCCESSFUL;
    
    disktab_size = DISKTAB_INITIAL_SIZE;
    disktab = calloc(disktab_size, sizeof(struct disk_device_table));
    if (disktab == NULL)
        return RTEMS_NO_MEMORY;

    diskdevs_protected = FALSE;
    rc = rtems_semaphore_create(
        rtems_build_name('D', 'D', 'E', 'V'), 1,
        RTEMS_FIFO | RTEMS_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | 
        RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL, 0, &diskdevs_mutex);
    
    if (rc != RTEMS_SUCCESSFUL)
    {
        free(disktab);
        return rc;
    }
    
    rc = rtems_bdbuf_init(rtems_bdbuf_configuration,
                          rtems_bdbuf_configuration_size);
    
    if (rc != RTEMS_SUCCESSFUL)
    {
        rtems_semaphore_delete(diskdevs_mutex);
        free(disktab);
        return rc;
    }
    
    disk_io_initialized = 1;
    return RTEMS_SUCCESSFUL;
}

/* rtems_disk_io_done --
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
rtems_disk_io_done(void)
{
    rtems_device_major_number maj;
    rtems_device_minor_number min;
    rtems_status_code rc;

    /* Free data structures */    
    for (maj = 0; maj < disktab_size; maj++)
    {
        struct disk_device_table *dtab = disktab + maj;
        if (dtab != NULL)
        {
            for (min = 0; min < dtab->size; min++)
            {
                disk_device *dd = dtab->minor[min];
                unlink(dd->name);
                free(dd->name);
                free(dd);
            }
            free(dtab);
        }
    }
    free(disktab);

    rc = rtems_semaphore_release(diskdevs_mutex);
    
    /* XXX bdbuf should be released too! */
    disk_io_initialized = 0;
    return rc;
}
