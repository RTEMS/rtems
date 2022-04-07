/* SPDX-License-Identifier: BSD-2-Clause */

/*! @file
 * @brief Check how rtems_bdbuf_sync() works.
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read(#N) in thread #1.
 * -# Call rtems_bdbuf_read(#N) from thread #2.
 * -# Thread #2 blocks on this call.
 * -# Call rtems_bdbuf_sync(#N) in thread #1.
 * -# After a while disk driver gets write request for block #N.
 *    Notify bdbuf library about write complete event.
 * -# Check that thread #1 unlocked after this.
 * -# Check that thread #2 unblocks and get buffer #N.
 * -# Release buffer in thread #2.
 * .
 *
 * Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
 * Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "bdbuf_tests.h"

static rtems_task bdbuf_test4_1_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test4_1_thread2(rtems_task_argument arg);

/* Block number used in the test */
#define TEST_BLK_NUM_N 112

void
bdbuf_test4_1_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 4.1");

    START_THREAD(1, bdbuf_test4_1_thread1);
    START_THREAD(2, bdbuf_test4_1_thread2);

    /*
     * Step 1:
     * Call rtems_bdbuf_read(#N) in thread #1.
     * Wait for read request in disk driver.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /* Wait for return from rtems_bdbuf_read(#N) function. */
    WAIT_THREAD_SYNC(1);

    /*
     * Step 2:
     * Call rtems_bdbuf_read(#N) in thread #2.
     */
    CONTINUE_THREAD(2);

    /* Make sure that thread #2 blocks */
    CHECK_THREAD_BLOCKED(2);
    TEST_CHECK_RESULT("2");

    /*
     * Step 3:
     * Call rtems_bdbuf_sync (#N) in thread #1.
     */
    CONTINUE_THREAD(1);

    /*
     * Setp 4:
     * Wait for Write request to device driver.
     */
    WAIT_DRV_MSG_WR(&msg);

    /* Check that both threads are blocked right now */
    CHECK_THREAD_BLOCKED(1);
    CHECK_THREAD_BLOCKED(2);

    /* Send Write complete notification */
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    /*
     * Step 5:
     * Check that rtems_bdbuf_sync(#N) call is unlocked.
     */
    WAIT_THREAD_SYNC(1);
    TEST_CHECK_RESULT("5");

    /*
     * Step 6:
     * Check that rtems_bdbuf_read(#N) call in thread #2 is unlocked.
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

    TEST_STOP();
}

static rtems_task
bdbuf_test4_1_thread1(rtems_task_argument arg)
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

    CONTINUE_MAIN(1);

    /*
     * Step 3:
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
bdbuf_test4_1_thread2(rtems_task_argument arg)
{
    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 2:
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
