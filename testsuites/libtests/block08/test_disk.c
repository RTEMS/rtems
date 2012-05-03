/*! @file
 *  @brief Test disk block device implementation.
 *
 * Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
 * Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
#include <inttypes.h>

#include "rtems/blkdev.h"
#include "rtems/diskdevs.h"

#include "bdbuf_tests.h"

static Objects_Id drvq_id = OBJECTS_ID_NONE;
static Objects_Id testq_id = OBJECTS_ID_NONE;

static int
test_disk_ioctl(rtems_disk_device *dd, uint32_t req, void *argp)
{
    rtems_status_code rc;
    bdbuf_test_msg    msg;
    size_t            msg_size;

    switch (req)
    {
        case RTEMS_BLKIO_REQUEST:
        {
            rtems_blkdev_request   *r = argp;
            rtems_blkdev_sg_buffer *sg;
            unsigned int            i;

            printk("DISK_DRV: %s ",
                   r->req == RTEMS_BLKDEV_REQ_READ ? "R" :
                   r->req == RTEMS_BLKDEV_REQ_WRITE ? "W" : "?");
            for (i = 0, sg = r->bufs; i < r->bufnum; i++, sg++)
            {
                printk("[%d] ", sg->block);
            }
            printk("\n");
            break;
        }

        default:
            printk("%s() Unexpected request comes %u\n",
                   __FUNCTION__, req);
            return -1;
    }

    memset(&msg, 0, sizeof(msg));
    msg.type = BDBUF_TEST_MSG_TYPE_DRIVER_REQ;
    msg.val.driver_req.dd = dd;
    msg.val.driver_req.req = req;
    msg.val.driver_req.argp = argp;

    rc = rtems_message_queue_send(testq_id, &msg, sizeof(msg));
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("Error while sending a message to Test task: %u\n", rc);
        return -1;
    }

    /* Wait for a reply from the test task */
    msg_size = sizeof(msg);
    rc = rtems_message_queue_receive(drvq_id, &msg, &msg_size,
                                     RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("Error while reading a message from Test task: %u\n", rc);
        return rc;
    }
    if (msg.type != BDBUF_TEST_MSG_TYPE_DRIVER_REPLY)
    {
        printf("Unexpected message comes to test disk driver: %d\n",
               msg.type);
        return -1;
    }

    if (msg.val.driver_reply.ret_val != 0)
    {
        errno = msg.val.driver_reply.ret_errno;
    }
    else
    {
        rtems_blkdev_request *r = (rtems_blkdev_request *)argp;

        r->req_done(r->done_arg, msg.val.driver_reply.res_status);
    }

    return msg.val.driver_reply.ret_val;
}

rtems_device_driver
test_disk_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg)
{
    rtems_status_code rc;
    dev_t             dev;

    rc = rtems_disk_io_initialize();
    if (rc != RTEMS_SUCCESSFUL)
        return rc;

    dev = rtems_filesystem_make_dev_t(major, minor);
    rc = rtems_disk_create_phys(dev,
                                TEST_DISK_BLOCK_SIZE, TEST_DISK_BLOCK_NUM,
                                test_disk_ioctl,
                                NULL,
                                TEST_DISK_NAME);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("Failed to create %s disk\n", TEST_DISK_NAME);
        return rc;
    }

    rc = bdbuf_test_create_drv_rx_queue(&drvq_id);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printk("%s() Failed to create Msg Queue for RX: %u\n",
               __FUNCTION__, rc);
        return rc;
    }

    rc = bdbuf_test_create_drv_tx_queue(&testq_id);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("%s() Failed to get Msg Queue for TX: %u\n",
               __FUNCTION__, rc);
        return rc;
    }

    printk("TEST DISK - OK\n");
    return RTEMS_SUCCESSFUL;
}

