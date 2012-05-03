/*! @file
 * @brief Check that when there are some tasks waiting for an available
 * buffer only one of them get a buffer after some other task releases one.
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read(#N1) in thread #1.
 * -# Call rtems_bdbuf_get(#N2) in thread #2.
 *    This thread blocks because we have no buffers available.
 * -# Call rtems_bdbuf_get(#N3) in thread #3.
 *    This thread also blocks because we have no buffers available.
 * -# Call rtems_bdbuf_release_modified(#N1) in thread #1.
 * -# Check that only one thread (thread #2 or thread #3) got a buffer.
 *     Another thread shall still be blocked.
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

static rtems_task bdbuf_test3_2_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test3_2_thread2(rtems_task_argument arg);
static rtems_task bdbuf_test3_2_thread3(rtems_task_argument arg);

/* Block number used in the test */
#define TEST_BLK_NUM_N1 90
#define TEST_BLK_NUM_N2 92
#define TEST_BLK_NUM_N3 94

void
bdbuf_test3_2_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 3.2");

    /*
     * Create working threads.
     */
    SET_THREAD_PRIORITY(2, LOW);
    SET_THREAD_PRIORITY(3, HIGH);
    START_THREAD(1, bdbuf_test3_2_thread1);
    START_THREAD(2, bdbuf_test3_2_thread2);
    START_THREAD(3, bdbuf_test3_2_thread3);

    /*
     * Start thread #1: it will read buffer #N1.
     * Driver is asked to read this buffer.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    WAIT_THREAD_SYNC(1);

    /*
     * Block thread #2 and thread #3 on get buffers #N1 and #N2
     * correspondingly.
     */
    CONTINUE_THREAD(2);
    CONTINUE_THREAD(3);

    /* Make sure threads managed to block on the buffers get. */
    CHECK_THREAD_BLOCKED(2);
    CHECK_THREAD_BLOCKED(3);

    /*
     * Step 4:
     * Thread #1 release buffer.
     */
    CONTINUE_THREAD(1);

    /*
     * Check that thread #2 unblocked after this.
     */
    WAIT_THREAD_SYNC(2);
    TEST_CHECK_RESULT("5");

    CHECK_THREAD_BLOCKED(3);

    /* Release buffer in thread #2 */
    CONTINUE_THREAD(2);

    /* wait for driver message to flush it onto a disk */
    WAIT_DRV_MSG_WR(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /* Wait for thread #3 to be unblocked */
    WAIT_THREAD_SYNC(3);

    /* Release buffer in thread #3 */
    CONTINUE_THREAD(3);

    /* wait for driver message to flush it onto a disk */
    WAIT_DRV_MSG_WR(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    TEST_END();
}

static rtems_task
bdbuf_test3_2_thread1(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    /*
     * Step 1:
     * Call rtems_bdbuf_read(#N) to get a buffer;
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM_N1, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(1);

    /*
     * Step 4:
     * Call rtems_bdbuf_release(#N).
     */
    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    THREAD_END();
}

static rtems_task
bdbuf_test3_2_thread2(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 2:
     * In thread #2 call get(#N2)
     */
    rc = rtems_bdbuf_get(test_dd, TEST_BLK_NUM_N2, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    printk("Thread #2 DEBLOCK\n");
    CONTINUE_MAIN(2);

    rc = rtems_bdbuf_release_modified(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    THREAD_END();
}

static rtems_task
bdbuf_test3_2_thread3(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(3);

    /*
     * Step 3:
     * In thread #3 call get(#N3)
     */
    rc = rtems_bdbuf_get(test_dd, TEST_BLK_NUM_N3, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    printk("Thread #3 DEBLOCK\n");

    CONTINUE_MAIN(3);
    
    rc = rtems_bdbuf_release_modified(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    THREAD_END();
}

