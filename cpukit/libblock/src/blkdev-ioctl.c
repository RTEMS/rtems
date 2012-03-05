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

#include <rtems/blkdev.h>
#include <rtems/bdbuf.h>

int
rtems_blkdev_ioctl(rtems_disk_device *dd, uint32_t req, void *argp)
{
    size_t            *arg_size = argp;
    int                rc = 0;

    switch (req)
    {
        case RTEMS_BLKIO_GETMEDIABLKSIZE:
            *arg_size = dd->media_block_size;
            break;

        case RTEMS_BLKIO_GETBLKSIZE:
            *arg_size = dd->block_size;
            break;

        case RTEMS_BLKIO_SETBLKSIZE:
            dd->block_size = *arg_size;
            break;

        case RTEMS_BLKIO_GETSIZE:
            *arg_size = dd->size;
            break;

        case RTEMS_BLKIO_SYNCDEV:
        {
            rtems_status_code sc = rtems_bdbuf_syncdev(dd);
            if (sc != RTEMS_SUCCESSFUL) {
                errno = EIO;
                rc = -1;
            }
            break;
        }

        case RTEMS_BLKIO_GETDISKDEV:
        {
            rtems_disk_device **dd_ptr = argp;
            *dd_ptr = dd;
            break;
        }

        default:
            errno = EINVAL;
            rc = -1;
            break;
    }

    return rc;
}
