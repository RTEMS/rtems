/*! @file
 *  @brief Test disk block device implementation.
 *
 * Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
 * Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <rtems/blkdev.h>

#include "bdbuf_tests.h"

static Objects_Id drvq_id = OBJECTS_ID_NONE;
static Objects_Id testq_id = OBJECTS_ID_NONE;

static int
test_disk_ioctl(rtems_disk_device *dd, uint32_t req, void *argp)
{
    rtems_status_code     rc;
    bdbuf_test_msg        msg;
    size_t                msg_size;
    rtems_blkdev_request *r;

    switch (req)
    {
        case RTEMS_BLKIO_REQUEST:
        {
            rtems_blkdev_sg_buffer *sg;
            unsigned int            i;

            r = argp;

            printf("DISK_DRV: %s ",
                   r->req == RTEMS_BLKDEV_REQ_READ ? "R" :
                   r->req == RTEMS_BLKDEV_REQ_WRITE ? "W" : "?");
            for (i = 0, sg = r->bufs; i < r->bufnum; i++, sg++)
            {
                printf("[%" PRIu32 "] ", sg->block);
            }
            printf("\n");
            break;
        }

        default:
            return rtems_blkdev_ioctl (dd, req, argp);
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
        rtems_blkdev_request_done(r, RTEMS_IO_ERROR);
        return 0;
    }

    /* Wait for a reply from the test task */
    msg_size = sizeof(msg);
    rc = rtems_message_queue_receive(drvq_id, &msg, &msg_size,
                                     RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("Error while reading a message from Test task: %u\n", rc);
        rtems_blkdev_request_done(r, RTEMS_IO_ERROR);
        return 0;
    }
    if (msg.type != BDBUF_TEST_MSG_TYPE_DRIVER_REPLY)
    {
        printf("Unexpected message comes to test disk driver: %d\n",
               msg.type);
        rtems_blkdev_request_done(r, RTEMS_IO_ERROR);
        return 0;
    }

    if (msg.val.driver_reply.ret_val != 0)
    {
        rtems_blkdev_request_done(r, RTEMS_IO_ERROR);
    }
    else
    {
        rtems_blkdev_request_done(r, msg.val.driver_reply.res_status);
    }

    return 0;
}

rtems_status_code
test_disk_initialize(void)
{
    rtems_status_code rc;

    rc = rtems_blkdev_create(TEST_DISK_NAME,
                             TEST_DISK_BLOCK_SIZE, TEST_DISK_BLOCK_NUM,
                             test_disk_ioctl,
                             NULL);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("Failed to create %s disk\n", TEST_DISK_NAME);
        return rc;
    }

    rc = bdbuf_test_create_drv_rx_queue(&drvq_id);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("%s() Failed to create Msg Queue for RX: %u\n",
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

    printf("TEST DISK - OK\n");
    return RTEMS_SUCCESSFUL;
}
