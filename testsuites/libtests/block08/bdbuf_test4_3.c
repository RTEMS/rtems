/*! @file
 * @brief Check how rtems_bdbuf_sync() works.
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read(#N) in thread #1.
 * -# Call rtems_bdbuf_sync(#N) in thread #1.
 * -# After a while disk driver gets write request for block #N.
 *    Notify bdbuf about write complete event.
 * -# Check that rtems_bdbuf_sync(#N) in thread #1 unlocked after this.
 * -# Call rtems_bdbuf_read(#N) in thread #2.
 * -# Check that buffer is successfully obtained in thread #2.
 * -# Call rtems_bdbuf_release(#N) in thread #2.
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

static rtems_task bdbuf_test4_3_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test4_3_thread2(rtems_task_argument arg);

/* Block number used in the test */
#define TEST_BLK_NUM_N 116

void
bdbuf_test4_3_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 4.3");

    START_THREAD(1, bdbuf_test4_3_thread1);
    START_THREAD(2, bdbuf_test4_3_thread2);

    /*
     * Step 1:
     * Call rtems_bdbuf_read(#N) in thread #1.
     * Wait for read request in disk driver.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /*
     * Step 2:
     * Call rtems_bdbuf_sync(#N) in thread #1.
     * As the result buffer is asked to be flashed onto the disk.
     */
    WAIT_DRV_MSG_WR(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /*
     * Step 4:
     * Check that rtems_bdbuf_sync(#N) call is unlocked.
     */
    WAIT_THREAD_SYNC(1);
    TEST_CHECK_RESULT("4");

    /*
     * Step 5:
     * Call rtems_bdbuf_read(#N) in thread #2.
     */
    CONTINUE_THREAD(2);

    /*
     * Step 6:
     * Check that thread #2 successfully got the buffer.
     */
    WAIT_THREAD_SYNC(2);

    /*
     * Step 7:
     * Release buffer in thread #2
     */
    CONTINUE_THREAD(2);

    /*
     * Exit from thread #1.
     */
    CONTINUE_THREAD(1);

    TEST_END();
}

static rtems_task
bdbuf_test4_3_thread1(rtems_task_argument arg)
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
bdbuf_test4_3_thread2(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 5:
     * In thread #2 call rtems_bdbuf_read(#N).
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


