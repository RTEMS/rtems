/* ramdisk.c -- RAM disk block device implementation
 *
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
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rtems/blkdev.h"
#include "rtems/diskdevs.h"
#include "rtems/ramdisk.h"

#define RAMDISK_DEVICE_BASE_NAME "/dev/ramdisk"

/* Internal RAM disk descriptor */
struct ramdisk {
    int           block_size; /* RAM disk block size */
    int           block_num;  /* Number of blocks on this RAM disk */
    void         *area;       /* RAM disk memory area */
    rtems_boolean initialized;/* RAM disk is initialized */
    rtems_boolean malloced;   /* != 0, if memory allocated by malloc for this 
                                 RAM disk */
};

static struct ramdisk *ramdisk;
static int nramdisks;

/* ramdisk_read --
 *     RAM disk READ request handler. This primitive copies data from RAM
 *     disk to supplied buffer and invoke the callout function to inform
 *     upper layer that reading is completed.
 * 
 * PARAMETERS:
 *     req - pointer to the READ block device request info
 *
 * RETURNS:
 *     ioctl return value
 */
static int
ramdisk_read(struct ramdisk *rd, blkdev_request *req)
{
    char *from;
    uint32_t   i;
    blkdev_sg_buffer *sg;
    uint32_t   remains;
    
    from = (char *)rd->area + (req->start * rd->block_size);
    remains = rd->block_size * req->count;
    sg = req->bufs;
    for (i = 0; (remains > 0) && (i < req->bufnum); i++, sg++)
    {
        int count = sg->length;
        if (count > remains)
            count = remains;
        memcpy(sg->buffer, from, count);
        remains -= count;
        from += count;
    }
    req->req_done(req->done_arg, RTEMS_SUCCESSFUL, 0);
    return 0;
}

/* ramdisk_write --
 *     RAM disk WRITE request handler. This primitive copies data from
 *     supplied buffer to RAM disk and invoke the callout function to inform
 *     upper layer that writing is completed.
 * 
 * PARAMETERS:
 *     req - pointer to the WRITE block device request info
 *
 * RETURNS:
 *     ioctl return value
 */
static int
ramdisk_write(struct ramdisk *rd, blkdev_request *req)
{
    char *to;
    uint32_t   i;
    blkdev_sg_buffer *sg;
    uint32_t   remains;
    
    to = (char *)rd->area + (req->start * rd->block_size);
    remains = rd->block_size * req->count;
    sg = req->bufs;
    for (i = 0; (remains > 0) && (i < req->bufnum); i++, sg++)
    {
        int count = sg->length;
        if (count > remains)
            count = remains;
        memcpy(to, sg->buffer, count);
        remains -= count;
        to += count;
    }
    req->req_done(req->done_arg, RTEMS_SUCCESSFUL, 0);
    return 0;
}

/* ramdisk_ioctl --
 *     IOCTL handler for RAM disk device.
 *
 * PARAMETERS:
 *      dev  - device number (major, minor number)
 *      req  - IOCTL request code
 *      argp - IOCTL argument
 *
 * RETURNS:
 *     IOCTL return value
 */
static int
ramdisk_ioctl(dev_t dev, int req, void *argp)
{
    switch (req)
    {
        case BLKIO_REQUEST:
        {
            rtems_device_minor_number minor;
            blkdev_request *r = argp;
            struct ramdisk *rd;

            minor = rtems_filesystem_dev_minor_t(dev);
            if ((minor >= nramdisks) || !ramdisk[minor].initialized)
            {
                errno = ENODEV;
                return -1;
            }
            
            rd = ramdisk + minor;
            
            switch (r->req)
            {
                case BLKDEV_REQ_READ:
                    return ramdisk_read(rd, r);

                case BLKDEV_REQ_WRITE:
                    return ramdisk_write(rd, r);
                    
                default:
                    errno = EBADRQC;
                    return -1;
            }
            break;
        }
        
        default:
            errno = EBADRQC;
            return -1;
    }
}

/* ramdisk_initialize --
 *     RAM disk device driver initialization. Run through RAM disk 
 *     configuration information and configure appropriate RAM disks.
 *
 * PARAMETERS:
 *     major - RAM disk major device number
 *     minor - minor device number, not applicable
 *     arg   - initialization argument, not applicable
 *
 * RETURNS:
 *     none
 */
rtems_device_driver
ramdisk_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg)
{
    rtems_device_minor_number i;
    rtems_ramdisk_config *c = rtems_ramdisk_configuration;
    struct ramdisk *r;
    rtems_status_code rc;

    rc = rtems_disk_io_initialize();
    if (rc != RTEMS_SUCCESSFUL)
        return rc;
        
    r = ramdisk = calloc(rtems_ramdisk_configuration_size, 
                         sizeof(struct ramdisk));
    
    for (i = 0; i < rtems_ramdisk_configuration_size; i++, c++, r++)
    {
        dev_t dev = rtems_filesystem_make_dev_t(major, i);
        char name[sizeof(RAMDISK_DEVICE_BASE_NAME "0123456789")];
        snprintf(name, sizeof(name), RAMDISK_DEVICE_BASE_NAME "%d", i);
        r->block_size = c->block_size;
        r->block_num = c->block_num;
        if (c->location == NULL)
        {
            r->malloced = TRUE;
            r->area = malloc(r->block_size * r->block_num);
            if (r->area == NULL) /* No enough memory for this disk */
            {
                r->initialized = FALSE;
                continue;
            }
            else
            {
                r->initialized = TRUE;
            }
        }
        else
        {
            r->malloced = FALSE;
            r->initialized = TRUE;
            r->area = c->location;
        }
        rc = rtems_disk_create_phys(dev, c->block_size, c->block_num,
                                    ramdisk_ioctl, name);
        if (rc != RTEMS_SUCCESSFUL)
        {
            if (r->malloced)
            {
                free(r->area);
            }
            r->initialized = FALSE;
        }
    }
    nramdisks = rtems_ramdisk_configuration_size;
    return RTEMS_SUCCESSFUL;
}
