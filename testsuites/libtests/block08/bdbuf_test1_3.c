/*! @file
 * @brief Check how rtems_bdbuf_read() handles two readers waiting
 * for a buffer with the same block number in cases when disk device 
 * driver reports failure in read complete notification.
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read() function in thread #1 and block on
 *    waiting for read complete notification.
 * -# Call rtems_bdbuf_read() function in thread #2 for the same block
 *    number. As the result it blocks on this read as well (but it will
 *    block on transfer semaphore).
 * -# Disk device reports an error in read complete notification.
 *    As the result an error is returned from rtems_bdbuf_read()
 *    in thread #1.
 * -# rtems_bdbuf_read() called in thread #2 should try to re-read data
 *    again so disk device ioctl() function is called again for this
 *    block number.
 * -# This time disk device reports success in read complete notification.
 *    As the result rtems_bdbuf_read() in thread #2 shall return 
 *    RTEMS_SUCCESSFUL and provide buffer descriptor.
 * -# Call rtems_bdbuf_release() function in thread #2.
 * .
 *
 * Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
 * Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bdbuf_tests.h>

static rtems_task bdbuf_test1_3_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test1_3_thread2(rtems_task_argument arg);

#define TEST_BLK_NUM 30

void
bdbuf_test1_3_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 1.3");

    START_THREAD(1, bdbuf_test1_3_thread1);
    START_THREAD(2, bdbuf_test1_3_thread2);

    /*
     * Step 1:
     * Thread #1 calls rtems_bdbuf_read() and we block
     * this thread on data transfer operation.
     */
    WAIT_DRV_MSG(&msg);

    /*
     * Step 2:
     * Thread #2 calls rtems_bdbuf_read() for the same 
     * block number, as the result it shall block waiting
     * on buffer state change.
     */
    CONTINUE_THREAD(2);

    /* Make sure thread #2 managed to block on the buffer. */
    CHECK_THREAD_BLOCKED(2);

    /*
     * Step 3:
     * Unblock thread #1 by reporting erroneous data transfer result.
     */
    SEND_DRV_MSG(0, 0, RTEMS_IO_ERROR, EFAULT);

    /*
     * Wait for sync from thread #1.
     */
    WAIT_THREAD_SYNC(1);
    CONTINUE_THREAD(1);
    TEST_CHECK_RESULT("3");

    /* Check thread #2 is still blocked */
    CHECK_THREAD_BLOCKED(2);

    /*
     * Step 4:
     * For thread #2 bdbuf shall try to re-read data.
     * As the result we will get read call to device driver.
     */
    WAIT_DRV_MSG(&msg);

    /*
     * Step 5:
     * This time report success from the driver and
     * rtems_bdbuf_read() in thread #2 shall return
     * RTEMS_SUCCESSFUL.
     */
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /*
     * Wait for sync from thread #2.
     */
    WAIT_THREAD_SYNC(2);
    TEST_CHECK_RESULT("5");

    /*
     * Step 6:
     * Release buffer in thread #2
     */
    CONTINUE_THREAD(2);

    TEST_END();
}

static rtems_task
bdbuf_test1_3_thread1(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    /*
     * Step 1 - 3:
     * Try to read blk #N on thread #1
     * We will block on this read and meanwhile
     * thread #2 will try to read the same block.
     * After blocking on read in thread #2, device
     * driver will notify about an error, and as the
     * result this call will return an error.
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM, &bd);
    if (rc != RTEMS_IO_ERROR || bd != NULL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(1);

    THREAD_END();
}

static rtems_task
bdbuf_test1_3_thread2(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 2:
     * Try to read block #N. Right now thread #1 is waiting
     * on data transfer operation, so we will block here as well.
     *
     * Step 4-5:
     * Due to the fact that thread #1 failed to read required block
     * number, bdbuf library should ask for re-read data again.
     * Time time main task will tell driver to report success.
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM, &bd);
    if (rc != RTEMS_SUCCESSFUL || bd == NULL)
    {
        TEST_FAILED();
    }
    CONTINUE_MAIN(2);

    /*
     * Step 6:
     * Release buffer.
     */
    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    THREAD_END();
}

