/**
 * @file
 *
 * @ingroup rtems_disk
 *
 * Block device disk management.
 */
 
/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <unistd.h>	/* unlink */
#include <string.h>

#include "rtems/diskdevs.h"
#include "rtems/bdbuf.h"

#define DISKTAB_INITIAL_SIZE 32

/* Table of disk devices having the same major number */
typedef struct rtems_disk_device_table {
    rtems_disk_device **minor; /* minor-indexed disk device table */
    uint32_t            size;            /* Number of entries in the table */
} rtems_disk_device_table;

/* Pointer to [major].minor[minor] indexed array of disk devices */
static rtems_disk_device_table *disktab;

/* Number of allocated entries in disktab table */
static uint32_t disktab_size;

/* Mutual exclusion semaphore for disk devices table */
static rtems_id diskdevs_mutex;

/* Flag meaning that disk I/O, buffering etc. already has been initialized. */
static bool disk_io_initialized = false;

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
static volatile bool diskdevs_protected;

/* create_disk_entry --
 *     Return pointer to the disk_entry structure for the specified device, or
 *     create one if it is not exists.
 *
 * PARAMETERS:
 *     dev - device id (major, minor)
 *
 * RETURNS:
 *     pointer to the disk device descriptor entry, or NULL if no memory
 *     available for its creation.
 */
static rtems_disk_device *
create_disk_entry(dev_t dev)
{
    rtems_device_major_number major;
    rtems_device_minor_number minor;
    rtems_disk_device **d;

    rtems_filesystem_split_dev_t (dev, major, minor);

    if (major >= disktab_size)
    {
        rtems_disk_device_table *p;
        uint32_t newsize;
        uint32_t i;
        newsize = disktab_size * 2;
        if (major >= newsize)
            newsize = major + 1;
        p = realloc(disktab, sizeof(rtems_disk_device_table) * newsize);
        if (p == NULL)
            return NULL;
        disktab = p;
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
        uint32_t            newsize;
        rtems_disk_device **p;
        uint32_t            i;
        uint32_t            s = disktab[major].size;

        if (s == 0)
            newsize = DISKTAB_INITIAL_SIZE;
        else
            newsize = s * 2;
        if (minor >= newsize)
            newsize = minor + 1;

        p = realloc(disktab[major].minor,
                    sizeof(rtems_disk_device *) * newsize);
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
        *d = calloc(1, sizeof(rtems_disk_device));
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
static rtems_disk_device *
get_disk_entry(dev_t dev)
{
    rtems_device_major_number major;
    rtems_device_minor_number minor;
    rtems_disk_device_table *dtab;

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
create_disk(dev_t dev, const char *name, rtems_disk_device **diskdev)
{
    rtems_disk_device *dd;
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

rtems_status_code rtems_disk_create_phys(
  dev_t dev,
  uint32_t block_size,
  rtems_blkdev_bnum disk_size,
  rtems_block_device_ioctl handler,
  const char *name
)
{
    rtems_disk_device *dd;
    rtems_status_code rc;
    rtems_device_major_number major;
    rtems_device_minor_number minor;

    rtems_filesystem_split_dev_t (dev, major, minor);

    rc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (rc != RTEMS_SUCCESSFUL)
        return rc;
    diskdevs_protected = true;

    rc = create_disk(dev, name, &dd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        diskdevs_protected = false;
        rtems_semaphore_release(diskdevs_mutex);
        return rc;
    }

    dd->phys_dev = dd;
    dd->uses = 0;
    dd->start = 0;
    dd->size = disk_size;
    dd->block_size = dd->media_block_size = block_size;
    dd->ioctl = handler;

    rc = rtems_io_register_name(name, major, minor);

    if (handler (dd->phys_dev->dev,
                 RTEMS_BLKDEV_CAPABILITIES,
                 &dd->capabilities) < 0)
      dd->capabilities = 0;
    
    diskdevs_protected = false;
    rtems_semaphore_release(diskdevs_mutex);

    return rc;
}

rtems_status_code rtems_disk_create_log(
  dev_t dev,
  dev_t phys,
  rtems_blkdev_bnum start,
  rtems_blkdev_bnum size,
  const char *name
)
{
  rtems_disk_device *dd = NULL;
  rtems_disk_device *pdd = NULL;
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;
  rtems_blkdev_bnum end = start + size;

  rtems_filesystem_split_dev_t (dev, major, minor);

  rc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (rc != RTEMS_SUCCESSFUL) {
    return rc;
  }

  diskdevs_protected = true;

  pdd = get_disk_entry(phys);
  if (
    pdd == NULL
      || pdd != pdd->phys_dev
      || start >= pdd->size
      || end <= start
      || end > pdd->size
  ) {
    diskdevs_protected = false;
    rtems_semaphore_release(diskdevs_mutex);
    return RTEMS_INVALID_NUMBER;
  }

  rc = create_disk(dev, name, &dd);
  if (rc != RTEMS_SUCCESSFUL) {
    diskdevs_protected = false;
    rtems_semaphore_release(diskdevs_mutex);
    return rc;
  }

  dd->phys_dev = pdd;
  dd->uses = 0;
  dd->start = start;
  dd->size = size;
  dd->block_size = dd->media_block_size = pdd->block_size;
  dd->ioctl = pdd->ioctl;

  rc = rtems_io_register_name(name, major, minor);

  diskdevs_protected = false;
  rc = rtems_semaphore_release(diskdevs_mutex);

  return rc;
}

rtems_status_code
rtems_disk_delete(dev_t dev)
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  rtems_device_major_number maj = 0;
  rtems_device_minor_number min = 0;
  rtems_disk_device *dd = NULL;
  bool physical_disk = true;

  rc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (rc != RTEMS_SUCCESSFUL) {
    return rc;
  }
  diskdevs_protected = true;

  /* Check if we have a physical or logical disk */
  dd = get_disk_entry(dev);
  if (dd == NULL) {
    return RTEMS_INVALID_NUMBER;
  }
  physical_disk = dd->phys_dev == dd;

  if (physical_disk) {
    unsigned used = 0;

    /* Check if this device is in use -- calculate usage counter */
    for (maj = 0; maj < disktab_size; maj++) {
      rtems_disk_device_table *dtab = disktab + maj;
      if (dtab != NULL) {
        for (min = 0; min < dtab->size; min++) {
          dd = dtab->minor[min];
          if ((dd != NULL) && (dd->phys_dev->dev == dev)) {
            used += dd->uses;
          }
        }
      }
    }

    if (used != 0) {
      diskdevs_protected = false;
      rtems_semaphore_release(diskdevs_mutex);
      return RTEMS_RESOURCE_IN_USE;
    }

    /* Delete this device and all of its logical devices */
    for (maj = 0; maj < disktab_size; maj++) {
      rtems_disk_device_table *dtab = disktab + maj;
      if (dtab != NULL) {
        for (min = 0; min < dtab->size; min++) {
          dd = dtab->minor[min];
          if ((dd != NULL) && (dd->phys_dev->dev == dev)) {
            unlink(dd->name);
            free(dd->name);
            free(dd);
            dtab->minor[min] = NULL;
          }
        }
      }
    }
  } else {
    rtems_filesystem_split_dev_t(dev, maj, min);
    disktab[maj].minor[min] = NULL;
    unlink(dd->name);
    free(dd->name);
    free(dd);
  }

  diskdevs_protected = false;
  rc = rtems_semaphore_release(diskdevs_mutex);
  return rc;
}

rtems_disk_device *
rtems_disk_obtain(dev_t dev)
{
    rtems_interrupt_level level;
    rtems_disk_device *dd;
    rtems_status_code rc;

    rtems_interrupt_disable(level);
    if (diskdevs_protected)
    {
        rtems_interrupt_enable(level);
        rc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT,
                                    RTEMS_NO_TIMEOUT);
        if (rc != RTEMS_SUCCESSFUL)
            return NULL;
        diskdevs_protected = true;
        dd = get_disk_entry(dev);
        dd->uses++;
        diskdevs_protected = false;
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

rtems_status_code
rtems_disk_release(rtems_disk_device *dd)
{
    rtems_interrupt_level level;
    rtems_interrupt_disable(level);
    dd->uses--;
    rtems_interrupt_enable(level);
    return RTEMS_SUCCESSFUL;
}

rtems_disk_device *
rtems_disk_next(dev_t dev)
{
    rtems_device_major_number major;
    rtems_device_minor_number minor;
    rtems_disk_device_table *dtab;

    dev++;
    rtems_filesystem_split_dev_t (dev, major, minor);

    if (major >= disktab_size)
        return NULL;

    dtab = disktab + major;
    while (true)
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

rtems_status_code
rtems_disk_io_initialize(void)
{
    rtems_status_code rc;

    if (disk_io_initialized)
        return RTEMS_SUCCESSFUL;

    disktab_size = DISKTAB_INITIAL_SIZE;
    disktab = calloc(disktab_size, sizeof(rtems_disk_device_table));
    if (disktab == NULL)
        return RTEMS_NO_MEMORY;

    diskdevs_protected = false;
    rc = rtems_semaphore_create(
        rtems_build_name('D', 'D', 'E', 'V'), 1,
        RTEMS_FIFO | RTEMS_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY |
        RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL, 0, &diskdevs_mutex);

    if (rc != RTEMS_SUCCESSFUL)
    {
        free(disktab);
        return rc;
    }

    rc = rtems_bdbuf_init();

    if (rc != RTEMS_SUCCESSFUL)
    {
        rtems_semaphore_delete(diskdevs_mutex);
        free(disktab);
        return rc;
    }

    disk_io_initialized = 1;
    return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_disk_io_done(void)
{
    rtems_device_major_number maj;
    rtems_device_minor_number min;
    rtems_status_code rc;

    /* Free data structures */
    for (maj = 0; maj < disktab_size; maj++)
    {
        rtems_disk_device_table *dtab = disktab + maj;
        if (dtab != NULL)
        {
            for (min = 0; min < dtab->size; min++)
            {
                rtems_disk_device *dd = dtab->minor[min];
                unlink(dd->name);
                free(dd->name);
                free(dd);
            }
            free(dtab);
        }
    }
    free(disktab);

    rc = rtems_semaphore_delete(diskdevs_mutex);

    disk_io_initialized = 0;
    return rc;
}
