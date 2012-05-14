/*! @file
 * @brief Check how read/release work in case of only one buffer in ready list.
 *
 * Test sequence:
 * -# In thread #1 call rtems_bdbuf_get(#N) to get an empty block #N.
 * -# In thread #1 call rtems_bdbuf_release_modified() for previously 
 *    got buffer.
 * -# In thread #1 call rtems_bdbuf_read(#N) to get the same buffer
 *    (after this call a buffer is in AVL tree with ACCESS_MODIFIED state).
 * -# In thread #2 call rtems_bdbuf_read/get(#M).
 * -# In thread #1 call rtems_bdbuf_release(#N).
 * -# Check that in thread #2 a buffer is obtained.
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

static rtems_task bdbuf_test3_1_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test3_1_thread2(rtems_task_argument arg);

/* Block number used in the test */
#define TEST_BLK_NUM_N 80
#define TEST_BLK_NUM_M 85

void
bdbuf_test3_1_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 3.1");

    START_THREAD(1, bdbuf_test3_1_thread1);
    START_THREAD(2, bdbuf_test3_1_thread2);

    /*
     * Step 1-3:
     * Wait for sync from thread #1.
     * Thread #1 switch a buffer for #N into ACCESS_MODIFIED state.
     */
    WAIT_THREAD_SYNC(1);
    TEST_CHECK_RESULT("3");

    /*
     * Step 4:
     * Thread #2 calls get() function and blocks
     * (there no buffers available in bdbuf library).
     */
    CONTINUE_THREAD(2);

    /* Make sure thread #2 managed to block on the buffer. */
    CHECK_THREAD_BLOCKED(2);

    /*
     * Step 6:
     * Thread #1 release buffer.
     */
    CONTINUE_THREAD(1);

    /*
     * Buffer released in thread #1 was in MODIFIED state, so
     * there will be a request for disk write
     */
    WAIT_DRV_MSG_WR(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /*
     * Check that thread #2 unblocked after this.
     */
    WAIT_THREAD_SYNC(2);
    TEST_CHECK_RESULT("5");

    /* 
     * Release buffer in thread #2
     */
    CONTINUE_THREAD(2);

    /*
     * Buffer released in thread #2 switched to MODIFIED state, so
     * there will be a request for disk write
     */
    WAIT_DRV_MSG_WR(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    TEST_END();
}

static rtems_task
bdbuf_test3_1_thread1(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    /*
     * Step 1:
     * Call rtems_bdbuf_get(#N) to get an empty block db;
     * [this call will remove a buffer from ready list and insert
     * it in AVL tree with ACCESS state.
     * Step 2:
     * Call release_modified(bd);
     * [Now we have one entry in modified list and it is still 
     * in AVL tree with MODIFIED state]
     * Step 3: 
     * Call read(#N) to get the same buffer.
     * [An entry is found in AVL tree, removed from modified list and 
     * returned with state ACCESS_MODIFIED]
     */
    rc = rtems_bdbuf_get(test_dd, TEST_BLK_NUM_N, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    
    rc = rtems_bdbuf_release_modified(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM_N, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(1);

    /* Step 5:
     * Call rtems_bdbuf_release(#N).
     * As the result buffer will be used by bdbuf to get 
     * buffer #M for thread #2.
     */
    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    THREAD_END();
}

static rtems_task
bdbuf_test3_1_thread2(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 4:
     * In thread #2 call read/get(#M)
     * [We ask for block number #M - there is no such entry in AVL,
     * and all the lists are empty (ready, lru, modified), as a result
     * this thread blocks on 
     * rtems_bdbuf_wait(pool, &pool->waiting, &pool->wait_waiters)]
     */
    rc = rtems_bdbuf_get(test_dd, TEST_BLK_NUM_M, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(2);

    /*
     * Release buffer.
     */
    rc = rtems_bdbuf_release_modified(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    THREAD_END();
}


