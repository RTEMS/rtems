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

#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/ramdisk.h>

rtems_device_driver
ramdisk_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor __attribute__((unused)),
    void *arg __attribute__((unused)))
{
    rtems_device_minor_number i;
    rtems_ramdisk_config *c = rtems_ramdisk_configuration;
    struct ramdisk *r;
    rtems_status_code rc;

    rc = rtems_disk_io_initialize();
    if (rc != RTEMS_SUCCESSFUL)
        return rc;

    /*
     * Coverity Id 27 notes that this calloc() is a resource leak.
     *
     * This is allocating memory for a RAM disk which will persist for
     * the life of the system. RTEMS has no "de-initialize" driver call
     * so there is no corresponding free(r).  Coverity is correct that
     * it is never freed but this is not a problem.
     */
    r = calloc(rtems_ramdisk_configuration_size, sizeof(struct ramdisk));
    r->trace = false;
    for (i = 0; i < rtems_ramdisk_configuration_size; i++, c++, r++)
    {
        dev_t dev = rtems_filesystem_make_dev_t(major, i);
        char name [] = RAMDISK_DEVICE_BASE_NAME "a";
        name [sizeof(RAMDISK_DEVICE_BASE_NAME)] += i;
        r->block_size = c->block_size;
        r->block_num = c->block_num;
        if (c->location == NULL)
        {
            r->malloced = true;
            r->area = malloc(r->block_size * r->block_num);
            if (r->area == NULL) /* No enough memory for this disk */
            {
                r->initialized = false;
                continue;
            }
            else
            {
                r->initialized = true;
            }
        }
        else
        {
            r->malloced = false;
            r->initialized = true;
            r->area = c->location;
        }
        rc = rtems_disk_create_phys(dev, c->block_size, c->block_num,
                                    ramdisk_ioctl, r, name);
        if (rc != RTEMS_SUCCESSFUL)
        {
            if (r->malloced)
            {
                free(r->area);
            }
            r->initialized = false;
        }
    }
    return RTEMS_SUCCESSFUL;
}
