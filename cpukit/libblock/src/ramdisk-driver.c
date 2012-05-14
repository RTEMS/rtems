/**
 * @file
 *
 * @ingroup rtems_ramdisk
 *
 * @brief RAM disk block device implementation.
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* FIXME: How to set this define? */
#if !defined(RTEMS_RAMDISK_TRACE)
    #define RTEMS_RAMDISK_TRACE 0
#endif

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#if RTEMS_RAMDISK_TRACE
    #include <stdio.h>
#endif

#include <rtems.h>
#include <rtems/ramdisk.h>

#if RTEMS_RAMDISK_TRACE
    static void
    rtems_ramdisk_printf (const ramdisk *rd, const char *format, ...)
    {
        if (rd->trace)
        {
            va_list args;
            va_start (args, format);
            printf ("ramdisk:");
            vprintf (format, args);
            printf ("\n");
        }
    }
#endif

static int
ramdisk_read(struct ramdisk *rd, rtems_blkdev_request *req)
{
    uint8_t *from = rd->area;
    uint32_t   i;
    rtems_blkdev_sg_buffer *sg;

#if RTEMS_RAMDISK_TRACE
    rtems_ramdisk_printf (rd, "ramdisk read: start=%d, blocks=%d",
                          req->bufs[0].block, req->bufnum);
#endif

    for (i = 0, sg = req->bufs; i < req->bufnum; i++, sg++)
    {
#if RTEMS_RAMDISK_TRACE
        rtems_ramdisk_printf (rd, "ramdisk read: buf=%d block=%d length=%d off=%d addr=%p",
                              i, sg->block, sg->length, sg->block * rd->block_size,
                              from + (sg->block * rd->block_size));
#endif
        memcpy(sg->buffer, from + (sg->block * rd->block_size), sg->length);
    }
    req->status = RTEMS_SUCCESSFUL;
    req->req_done(req->done_arg, RTEMS_SUCCESSFUL);
    return 0;
}

static int
ramdisk_write(struct ramdisk *rd, rtems_blkdev_request *req)
{
    uint8_t *to = rd->area;
    uint32_t   i;
    rtems_blkdev_sg_buffer *sg;

#if RTEMS_RAMDISK_TRACE
    rtems_ramdisk_printf (rd, "ramdisk write: start=%d, blocks=%d",
                          req->bufs[0].block, req->bufnum);
#endif
    for (i = 0, sg = req->bufs; i < req->bufnum; i++, sg++)
    {
#if RTEMS_RAMDISK_TRACE
        rtems_ramdisk_printf (rd, "ramdisk write: buf=%d block=%d length=%d off=%d addr=%p",
                              i, sg->block, sg->length, sg->block * rd->block_size,
                              to + (sg->block * rd->block_size));
#endif
        memcpy(to + (sg->block * rd->block_size), sg->buffer, sg->length);
    }
    req->status = RTEMS_SUCCESSFUL;
    req->req_done(req->done_arg, RTEMS_SUCCESSFUL);
    return 0;
}

int
ramdisk_ioctl(rtems_disk_device *dd, uint32_t req, void *argp)
{
    struct ramdisk *rd = rtems_disk_get_driver_data(dd);

    switch (req)
    {
        case RTEMS_BLKIO_REQUEST:
        {
            rtems_blkdev_request *r = argp;

            switch (r->req)
            {
                case RTEMS_BLKDEV_REQ_READ:
                    return ramdisk_read(rd, r);

                case RTEMS_BLKDEV_REQ_WRITE:
                    return ramdisk_write(rd, r);

                default:
                    errno = EINVAL;
                    return -1;
            }
            break;
        }

        case RTEMS_BLKIO_DELETED:
            if (rd->free_at_delete_request) {
              ramdisk_free(rd);
            }
            break;

        default:
            return rtems_blkdev_ioctl (dd, req, argp);
            break;
    }

    errno = EINVAL;
    return -1;
}
