/* SPDX-License-Identifier: BSD-2-Clause */

/*! @file
 * @brief Check how rtems_bdbuf_read() handles read request
 * for a buffer that is owned by an application.
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read() function in thread #1 and
 *    provide successful read complete notification for this operation.
 *    As the result rtems_bdbuf_read() returns RTEMS_SUCCESSFUL
 *    in thread #1.
 * -# In thread #2 call rtems_bdbuf_read() function for the same
 *    block number. A buffer for this block is owned by an application
 *    and as the result thread #2 block on this function.
 * -# Call rtems_bdbuf_release() function in thread #1 in order to give
 *    it back under control of bdbuf library.
 * -# Buffer now is ready to be returned for another application and
 *    as the result rtems_bdbuf_read() unblocks and returns
 *    RTEMS_SUCCESSFUL in thread #2.
 * -# Call rtems_bdbuf_release() function in thread #2.
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

static rtems_task bdbuf_test1_5_thread1(rtems_task_argument arg);
static rtems_task bdbuf_test1_5_thread2(rtems_task_argument arg);

#define TEST_BLK_NUM 50

void
bdbuf_test1_5_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 1.5");

    START_THREAD(1, bdbuf_test1_5_thread1);
    START_THREAD(2, bdbuf_test1_5_thread2);

    /*
     * Step 1:
     * Thread #1 calls rtems_bdbuf_read() and successfully
     * get requested buffer.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    WAIT_THREAD_SYNC(1);
    TEST_CHECK_RESULT("1");

    /*
     * Step 2:
     * Thread #2 calls rtems_bdbuf_read() and blocks
     * on this call because thread #1 owns a buffer.
     */
    CONTINUE_THREAD(2);

    /* Make sure thread #2 managed to block on a read request. */
    CHECK_THREAD_BLOCKED(2);

    /*
     * Step 3:
     * Now thread #1 releases a buffer.
     */
    CONTINUE_THREAD(1);

    /*
     * Step 4:
     * Thread #2 should unblock now and get the buffer.
     */
    WAIT_THREAD_SYNC(2);
    TEST_CHECK_RESULT("4");

    /*
     * Step 5:
     * Thread #2 release buffer.
     */
    CONTINUE_THREAD(2);

    TEST_STOP();
}

static rtems_task
bdbuf_test1_5_thread1(rtems_task_argument arg)
{
    (void) arg;

    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    /*
     * Step 1:
     * read blk #N on thread #1
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    CONTINUE_MAIN(1);

    /*
     * Step 3:
     * Release buffer returned on step 1.
     */
    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    THREAD_END();
}

static rtems_task
bdbuf_test1_5_thread2(rtems_task_argument arg)
{
    (void) arg;

    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd = NULL;

    WAIT_MAIN_SYNC(2);

    /*
     * Step 2:
     * Try to read block #N. Right now thread #1 owns
     * this buffer, so we will block waiting for buffer.
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM, &bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    CONTINUE_MAIN(2);

    rc = rtems_bdbuf_release(bd);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    THREAD_END();
}
