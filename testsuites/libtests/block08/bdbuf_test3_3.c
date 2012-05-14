/*! @file
 * @brief Check how read/release work in case of only one buffer in ready list.
 *
 * The same as Test 3.2, but instead of calling rtems_bdbuf_get() in 
 * threads #1 and #2, it calls rtems_bdbuf_read().
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read(#N1) in thread #1.
 * -# Call rtems_bdbuf_read(#N2) in thread #2.
 *    This thread blocks because we have no buffers available.
 * -# Call rtems_bdbuf_read(#N3) in thread #3.
 *    This thread also blocks because we have no buffers available.
 * -# Call rtems_bdbuf_release(#N1) in thread #1.
 * -# Check that only one thread (thread #2 or thread #3) got a buffer.
 *    Another thread shall still be blocked.
 * -# Call rtems_bdbuf_release(#N2) in thread #2 and check that 
 *    thread #3 got a buffer as the result.
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

static rtems_task bdbuf_test3_3_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test3_3_thread2(rtems_task_argument arg);
static rtems_task bdbuf_test3_3_thread3(rtems_task_argument arg);

/* Block number used in the test */
#define TEST_BLK_NUM_N1 96
#define TEST_BLK_NUM_N2 98
#define TEST_BLK_NUM_N3 100

void
bdbuf_test3_3_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 3.3");

    /*
     * Create working threads.
     */
    SET_THREAD_PRIORITY(2, LOW);
    SET_THREAD_PRIORITY(3, HIGH);
    START_THREAD(1, bdbuf_test3_3_thread1);
    START_THREAD(2, bdbuf_test3_3_thread2);
    START_THREAD(3, bdbuf_test3_3_thread3);

    /*
     * Start thread #1: it will read buffer #N1.
     * Driver is asked to read this buffer.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    WAIT_THREAD_SYNC(1);

    /*
     * Block thread #2 and thread #3 on get buffers #N2 and #N3
     * correspondingly (bdbuf librbary has no buffers available).
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

    /* Wait for read request in the driver (for thread #2) */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /* Check that there is no read request for another block */
    CHECK_NO_DRV_MSG();

    /*
     * Check that thread #2 unblocked after this.
     */
    WAIT_THREAD_SYNC(2);
    TEST_CHECK_RESULT("5");

    /* Thread #3 is still in blocked state */
    CHECK_THREAD_BLOCKED(3);

    /* Release buffer in thread #2 */
    CONTINUE_THREAD(2);

    /* Wait for read request in the driver (for thread #3) */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /* Wait for thread #3 to be unblocked */
    WAIT_THREAD_SYNC(3);

    /* Release buffer in thread #3 */
    CONTINUE_THREAD(3);

    CHECK_NO_DRV_MSG();

    TEST_END();
}

static rtems_task
bdbuf_test3_3_thread1(rtems_task_argument arg)
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
bdbuf_test3_3_thread2(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 2:
     * In thread #2 call read(#N2)
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM_N2, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    printk("Thread #2 DEBLOCK\n");
    CONTINUE_MAIN(2);

    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    THREAD_END();
}

static rtems_task
bdbuf_test3_3_thread3(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(3);

    /*
     * Step 3:
     * In thread #3 call read(#N3)
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM_N3, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    printk("Thread #3 DEBLOCK\n");

    CONTINUE_MAIN(3);
    
    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    THREAD_END();
}

