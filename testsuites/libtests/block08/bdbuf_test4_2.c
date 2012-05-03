/*! @file
 * @brief Check how rtems_bdbuf_sync() works.
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read(#N) in thread #1.
 * -# Call rtems_bdbuf_sync(#N) in thread #1.
 * -# After a while disk driver gets write request for block #N.
 * -# Call rtems_bdbuf_read(#N) from thread #2 (before reply from the driver).
 *    Thread #2 blocks on this call.
 * -# Notify bdbuf library about write complete event.
 * -# Check that rtems_bdbuf_sync(#N) in thread #1 unlocked after this.
 *    (in fact a thread to unlock might be any).
 * -# Check that thread #2 unblocks and get buffer #N.
 * -# Release buffer in thread #2.
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

static rtems_task bdbuf_test4_2_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test4_2_thread2(rtems_task_argument arg);

/* Block number used in the test */
#define TEST_BLK_NUM_N 114

void
bdbuf_test4_2_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 4.2");

    START_THREAD(1, bdbuf_test4_2_thread1);
    START_THREAD(2, bdbuf_test4_2_thread2);

    /*
     * Step 1:
     * Call rtems_bdbuf_read(#N) in thread #1.
     * Wait for read request in disk driver.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /*
     * Step 2, 3:
     * Call rtems_bdbuf_sync(#N) in thread #1.
     * As the result buffer is asked to be flashed onto the disk.
     */
    WAIT_DRV_MSG_WR(&msg);
    TEST_CHECK_RESULT("3");

    /*
     * Step 4:
     * Call rtems_bdbuf_read(#N) in thread #2
     */
    CONTINUE_THREAD(2);

    /* Make sure that thread #2 blocks */
    CHECK_THREAD_BLOCKED(2);

    /*
     * Step 5:
     * Notify bdbuf library about write complete event.
     */
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /* Check that rtems_bdbuf_sync() is unlocked in thread #1 */
    WAIT_THREAD_SYNC(1);

    /*
     * Step 7:
     * Check that rtems_bdbuf_read(#N) call is unlocked in thread #2.
     */
    WAIT_THREAD_SYNC(2);
    TEST_CHECK_RESULT("7");

    /*
     * Step 8:
     * Release buffer in thread #2
     */
    CONTINUE_THREAD(2);

    /* Exit from thread #1 */
    CONTINUE_THREAD(1);

    TEST_END();
}

static rtems_task
bdbuf_test4_2_thread1(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    /*
     * Step 1:
     * Call rtems_bdbuf_read(#N) in thread #1;
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM_N, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    /*
     * Step 2:
     * Call rtems_bdbuf_sync(#N)
     */
    rc = rtems_bdbuf_sync(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(1);

    THREAD_END();
}

static rtems_task
bdbuf_test4_2_thread2(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 4:
     * In thread #2 call read(#N).
     * We will block on this call.
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM_N, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(2);

    /*
     * Release buffer.
     */
    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    THREAD_END();
}


