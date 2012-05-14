/**
 * @file
 *
 * @ingroup rtems_blkdev
 *
 * Block device management.
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
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
    rtems_device_major_number major __attribute__((unused)),
    rtems_device_minor_number minor __attribute__((unused)),
    void                    * arg)
{
    rtems_status_code rc = RTEMS_SUCCESSFUL;
    rtems_libio_rw_args_t *args = arg;
    rtems_libio_t *iop = args->iop;
    rtems_disk_device *dd = iop->data1;
    uint32_t block_size = dd->block_size;
    char *buf = args->buffer;
    uint32_t count = args->count;
    rtems_blkdev_bnum block = (rtems_blkdev_bnum) (args->offset / block_size);
    uint32_t blkofs = (uint32_t) (args->offset % block_size);

    args->bytes_moved = 0;

    while (count > 0)
    {
        rtems_bdbuf_buffer *diskbuf;
        uint32_t            copy;

        rc = rtems_bdbuf_read(dd, block, &diskbuf);
        if (rc != RTEMS_SUCCESSFUL)
            break;
        copy = block_size - blkofs;
        if (copy > count)
            copy = count;
        memcpy(buf, (char *)diskbuf->buffer + blkofs, copy);
        rc = rtems_bdbuf_release(diskbuf);
        args->bytes_moved += copy;
        if (rc != RTEMS_SUCCESSFUL)
            break;
        count -= copy;
        buf += copy;
        blkofs = 0;
        block++;
    }

    return rc;
}

/* rtems_blkdev_generic_write --
 *     Generic block device write primitive. Implemented using block device
 *     buffer management primitives.
 */
rtems_device_driver
rtems_blkdev_generic_write(
    rtems_device_major_number major __attribute__((unused)),
    rtems_device_minor_number minor __attribute__((unused)),
    void                    * arg)
{
    rtems_status_code rc = RTEMS_SUCCESSFUL;
    rtems_libio_rw_args_t *args = arg;
    rtems_libio_t *iop = args->iop;
    rtems_disk_device *dd = iop->data1;
    uint32_t block_size = dd->block_size;
    char *buf = args->buffer;
    uint32_t count = args->count;
    rtems_blkdev_bnum block = (rtems_blkdev_bnum) (args->offset / block_size);
    uint32_t blkofs = (uint32_t) (args->offset % block_size);

    args->bytes_moved = 0;

    while (count > 0)
    {
        rtems_bdbuf_buffer *diskbuf;
        uint32_t            copy;

        if ((blkofs == 0) && (count >= block_size))
            rc = rtems_bdbuf_get(dd, block, &diskbuf);
        else
            rc = rtems_bdbuf_read(dd, block, &diskbuf);
        if (rc != RTEMS_SUCCESSFUL)
            break;

        copy = block_size - blkofs;
        if (copy > count)
            copy = count;
        memcpy((char *)diskbuf->buffer + blkofs, buf, copy);
        args->bytes_moved += copy;

        rc = rtems_bdbuf_release_modified(diskbuf);
        if (rc != RTEMS_SUCCESSFUL)
            break;

        count -= copy;
        buf += copy;
        blkofs = 0;
        block++;
    }

    return rc;
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
  rtems_libio_open_close_args_t *oc = arg;
  rtems_libio_t *iop = oc->iop;
  dev_t dev = rtems_filesystem_make_dev_t(major, minor);
  rtems_disk_device *dd = rtems_disk_obtain(dev);

  iop->data1 = dd;

  if (dd != NULL)
    return RTEMS_SUCCESSFUL;
  else
    return RTEMS_UNSATISFIED;
}


/* blkdev_generic_close --
 *     Generic block device close primitive.
 */
rtems_device_driver
rtems_blkdev_generic_close(
    rtems_device_major_number major __attribute__((unused)),
    rtems_device_minor_number minor __attribute__((unused)),
    void                    * arg)
{
  rtems_libio_open_close_args_t *oc = arg;
  rtems_libio_t *iop = oc->iop;
  rtems_disk_device *dd = iop->data1;

  rtems_disk_release(dd);

  return RTEMS_SUCCESSFUL;
}

/* blkdev_generic_ioctl --
 *     Generic block device ioctl primitive.
 */
rtems_device_driver
rtems_blkdev_generic_ioctl(
    rtems_device_major_number major __attribute__((unused)),
    rtems_device_minor_number minor __attribute__((unused)),
    void                    * arg)
{
    rtems_libio_ioctl_args_t *args = arg;
    rtems_libio_t *iop = args->iop;
    rtems_disk_device *dd = iop->data1;

    if (args->command != RTEMS_BLKIO_REQUEST)
    {
        args->ioctl_return = dd->ioctl(dd,
                                                  args->command,
                                                  args->buffer);
    }
    else
    {
        /*
         * It is not allowed to directly access the driver circumventing the
         * cache.
         */
        args->ioctl_return = -1;
    }

    return RTEMS_SUCCESSFUL;
}
