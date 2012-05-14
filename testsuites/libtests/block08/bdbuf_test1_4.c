/*! @file
 * @brief Check how rtems_bdbuf_read() handles two readers waiting
 * for a buffer with the same block number in cases when disk device 
 * driver reports success in read complete notification.
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read() function in thread #1 and block on
 *    waiting for read complete notification.
 * -# Call rtems_bdbuf_read() function in thread #2 for the same
 *    block number. As the result it blocks on this read as well
 *    (but it will block on transfer semaphore).
 * -# Disk device reports success in read complete notification.
 *    As the result rtems_bdbuf_read() function returns RTEMS_SUCCESSFUL
 *    in thread #1.
 * -# Thread #1 releases buffer with rtems_bdbuf_release() function.
 * -# rtems_bdbuf_read() function in thread #2 unlocks and 
 *    returns RTEMS_SUCCESSFUL.
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

static rtems_task bdbuf_test1_4_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test1_4_thread2(rtems_task_argument arg);

/* Block number used in the test */
#define TEST_BLK_NUM 40

void
bdbuf_test1_4_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 1.4");

    START_THREAD(1, bdbuf_test1_4_thread1);
    START_THREAD(2, bdbuf_test1_4_thread2);

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
     * on buffer state change (waiting on TRANSFER state).
     */
    CONTINUE_THREAD(2);

    /* Make sure threads #1 and #2 managed blocked on the buffer. */
    CHECK_THREAD_BLOCKED(1);
    CHECK_THREAD_BLOCKED(2);

    /*
     * Step 3:
     * Unblock thread #1 by reporting successful data transfer result.
     */
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /*
     * Wait for sync from thread #1.
     */
    WAIT_THREAD_SYNC(1);
    TEST_CHECK_RESULT("3");

    /* 
     * Check that thread #2 is still blocked.
     */
    CHECK_THREAD_BLOCKED(2);

    /*
     * Step 4:
     * Thread #1 releases buffer with bdbuf_release() call.
     */
    CONTINUE_THREAD(1);

    /*
     * Step 5:
     * On buffer release operation, we should have unblock
     * of thread #2 that is wating on read buffer operation.
     */
    WAIT_THREAD_SYNC(2);
    TEST_CHECK_RESULT("5");

    /*
     * Step 6:
     * Thread #2 release buffer.
     */
    CONTINUE_THREAD(2);

    TEST_END();
}

static rtems_task
bdbuf_test1_4_thread1(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    /*
     * Step 1 - 3:
     * Try to read blk #N on thread #1
     * We will block on this read and meanwhile
     * thread #2 will try to read the same block.
     * After blocking on read in thread #2, device
     * driver will notify successful completion of 
     * date transfer, and as the result this call 
     * will return valid buffer.
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    CONTINUE_MAIN(1);

    /*
     * Step 4:
     * Release buffer returned on the previous step.
     */
    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    THREAD_END();
}

static rtems_task
bdbuf_test1_4_thread2(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 2:
     * Try to read block #N. Right now thread #1 is waiting
     * on data transfer operation, so we will block here as well.
     *
     * Step 5:
     * On step 4 thread #1 releases buffer and as the result 
     * our read operation should finish with success. 
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    CONTINUE_MAIN(2);

    /*
     * Step 6:
     * Release buffer returned on the previous step.
     */
    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    THREAD_END();
}


