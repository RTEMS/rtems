/* 
 * blkdev.h - block device driver generic support 
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) $Id$
 */

#include <string.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <sys/ioctl.h>

#include "rtems/diskdevs.h"
#include "rtems/bdbuf.h"

/* rtems_blkdev_generic_read --
 *     Generic block device read primitive. Implemented using block device
 *     buffer management primitives.
 */
rtems_device_driver
rtems_blkdev_generic_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
    rtems_libio_rw_args_t *args = arg;
    int block_size_log2;
    int block_size;
    char *buf;
    unsigned int count;
    unsigned int block;
    unsigned int blkofs;
    dev_t dev;
    disk_device *dd;
    
    dev = rtems_filesystem_make_dev_t(major, minor);
    dd = rtems_disk_lookup(dev);
    if (dd == NULL)
        return RTEMS_INVALID_NUMBER;
    
    block_size_log2 = dd->block_size_log2;
    block_size = dd->block_size;
    
    buf = args->buffer;
    count = args->count;
    args->bytes_moved = 0;
    
    block = args->offset >> block_size_log2;
    blkofs = args->offset & (block_size - 1);
    
    while (count > 0)
    {
        bdbuf_buffer *diskbuf;
        int copy;
        rtems_status_code rc;
        
        rc = rtems_bdbuf_read(dev, block, &diskbuf);
        if (rc != RTEMS_SUCCESSFUL)
            return rc;
        copy = block_size - blkofs;
        if (copy > count)
            copy = count;
        memcpy(buf, (char *)diskbuf->buffer + blkofs, copy);
        rc = rtems_bdbuf_release(diskbuf);
        args->bytes_moved += copy;
        if (rc != RTEMS_SUCCESSFUL)
            return rc;
        count -= copy;
        buf += copy;
        blkofs = 0;
        block++;
    }
    return RTEMS_SUCCESSFUL;
}

/* rtems_blkdev_generic_write --
 *     Generic block device write primitive. Implemented using block device
 *     buffer management primitives. 
 */
rtems_device_driver
rtems_blkdev_generic_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
    rtems_libio_rw_args_t *args = arg;
    int block_size_log2;
    int block_size;
    char *buf;
    unsigned int count;
    unsigned int block;
    unsigned int blkofs;
    dev_t dev;
    rtems_status_code rc;
    disk_device *dd;
    
    dev = rtems_filesystem_make_dev_t(major, minor);
    dd = rtems_disk_lookup(dev);
    if (dd == NULL)
        return RTEMS_INVALID_NUMBER;
    
    block_size_log2 = dd->block_size_log2;
    block_size = dd->block_size;
    
    buf = args->buffer;
    count = args->count;
    args->bytes_moved = 0;
    
    block = args->offset >> block_size_log2;
    blkofs = args->offset & (block_size - 1);
    
    while (count > 0)
    {
        bdbuf_buffer *diskbuf;
        int copy;
        
        if ((blkofs == 0) && (count >= block_size))
            rc = rtems_bdbuf_get(dev, block, &diskbuf);
        else
            rc = rtems_bdbuf_read(dev, block, &diskbuf);
        if (rc != RTEMS_SUCCESSFUL)
            return rc;
            
        copy = block_size - blkofs;
        if (copy > count)
            copy = count;
        memcpy((char *)diskbuf->buffer + blkofs, buf, copy);
        args->bytes_moved += copy;
        
        rc = rtems_bdbuf_release_modified(diskbuf);
        if (rc != RTEMS_SUCCESSFUL)
            return rc;
        
        count -= copy;
        buf += copy;
        blkofs = 0;
        block++;
    }
    return RTEMS_SUCCESSFUL;
}

/* blkdev_generic_open --
 *     Generic block device open primitive.
 */
rtems_device_driver
rtems_blkdev_generic_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
    dev_t dev;
    disk_device *dd;
    
    dev = rtems_filesystem_make_dev_t(major, minor);
    dd = rtems_disk_lookup(dev);
    if (dd == NULL)
        return RTEMS_INVALID_NUMBER;
        
    dd->uses++;
    
    rtems_disk_release(dd);
    
    return RTEMS_SUCCESSFUL;
}


/* blkdev_generic_close --
 *     Generic block device close primitive.
 */
rtems_device_driver
rtems_blkdev_generic_close(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
    dev_t dev;
    disk_device *dd;
    
    dev = rtems_filesystem_make_dev_t(major, minor);
    dd = rtems_disk_lookup(dev);
    if (dd == NULL)
        return RTEMS_INVALID_NUMBER;
        
    dd->uses--;
    
    rtems_disk_release(dd);
    
    return RTEMS_SUCCESSFUL;
}

/* blkdev_generic_ioctl --
 *     Generic block device ioctl primitive.
 */
rtems_device_driver
rtems_blkdev_generic_ioctl(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
    rtems_libio_ioctl_args_t *args = arg;
    dev_t dev;
    disk_device *dd;
    int rc;
    
    dev = rtems_filesystem_make_dev_t(major, minor);
    dd = rtems_disk_lookup(dev);
    if (dd == NULL)
        return RTEMS_INVALID_NUMBER;
    
    switch (args->command)
    {
        case BLKIO_GETBLKSIZE:
            args->ioctl_return = dd->block_size;
            break;
        
        case BLKIO_GETSIZE:
            args->ioctl_return = dd->size;
            break;

        case BLKIO_SYNCDEV:
            rc = rtems_bdbuf_syncdev(dd->dev);
            args->ioctl_return = (rc == RTEMS_SUCCESSFUL ? 0 : -1);
            break;
            
        case BLKIO_REQUEST:
        {
            blkdev_request *req = args->buffer;
            req->start += dd->start;
            args->ioctl_return = dd->ioctl(dd->phys_dev->dev, args->command, 
                                           req);
            break;
        }
        
        default:
            args->ioctl_return = dd->ioctl(dd->phys_dev->dev, args->command, 
                                           args->buffer);
            break;
    }
    rtems_disk_release(dd);
    
    return RTEMS_SUCCESSFUL;
}
