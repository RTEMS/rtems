/* SPDX-License-Identifier: BSD-2-Clause */

/*! @file
 * @brief Check the behaviour of rtems_bdbuf_read() function
 * with different reports from disk device driver.
 *
 * Test sequence:
 * -# Call rtems_bdbuf_read() function and return 0 from disk device
 *    driver ioctl() function, and the result of asynchronous read
 *    complete notification is successful.
 * -# Check that rtems_bdbuf_read() returns RTEMS_SUCCESSFUL and
 *    provides buffer descriptor.
 * -# Call rtems_bdbuf_read() function and return -1 from disk device
 *    driver ioctl() function (there will be no asynchronous read
 *    complete notification).
 * -# Check that rtems_bdbuf_read() returns RTEMS_IO_ERROR.
 * -# Call rtems_bdbuf_read() function and return 0 from disk device
 *    driver ioctl() function, but the result of asynchronous read
 *    complete notification is faulty (with some erroneous status).
 * -# Check that rtems_bdbuf_read() returns that status and does not
 *    return buffer descriptor.
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

static rtems_task bdbuf_test1_1_thread1(rtems_task_argument arg);

#define TEST_BLK_NUM 10

void
bdbuf_test1_1_main()
{
    bdbuf_test_msg msg;

    TEST_START("Test 1.1");

    /*
     * Create working thread that will call rtems_bdbuf_read() function.
     */
    START_THREAD(1, bdbuf_test1_1_thread1);

    /*
     * Step 1:
     * Check that rtems_bdbuf_read() returns RTEMS_SUCCESSFUL
     * when device driver returns 0 from ioctl() call and
     * return RTEMS_SUCCESSFUL status in asynchronous
     * callback notification.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_SUCCESSFUL, 0);

    WAIT_THREAD_SYNC(1);
    TEST_CHECK_RESULT("2");

    CONTINUE_THREAD(1);

    /*
     * Step 3:
     * Check that rtems_bdbuf_read() returns RTEMS_IO_ERROR
     * return code, when device driver returns -1 from ioctl() call.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(-1, EFAULT, RTEMS_SUCCESSFUL, 0);

    WAIT_THREAD_SYNC(1);
    TEST_CHECK_RESULT("4");

    CONTINUE_THREAD(1);

    /*
     * Step 5:
     * Check that rtems_bdbuf_read() returns status obtained
     * from device driver via asynchonous notification.
     * On this step device driver returns 0 from ioctl() call,
     * but notification callback is called with RTEMS_IO_ERROR status.
     */
    WAIT_DRV_MSG(&msg);
    SEND_DRV_MSG(0, 0, RTEMS_IO_ERROR, EFAULT);

    WAIT_THREAD_SYNC(1);
    TEST_CHECK_RESULT("6");

    CONTINUE_THREAD(1);

    TEST_STOP();
}


static rtems_task
bdbuf_test1_1_thread1(rtems_task_argument arg)
{
    (void) arg;

    rtems_status_code   rc;
    rtems_bdbuf_buffer *bd1 = NULL;
    rtems_bdbuf_buffer *bd2 = NULL;

    /*
     * Step 1-2:
     * Successful read operation.
     */
    rc = rtems_bdbuf_read(test_dd, 0, &bd1);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }
    rc = rtems_bdbuf_release(bd1);
    if (rc != RTEMS_SUCCESSFUL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(1);

    /*
     * Step 3-4:
     * Read operation fails with RTEMS_IO_ERROR code.
     * The function shall not update user pointer.
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM, &bd2);
    if (rc != RTEMS_IO_ERROR || bd2 != NULL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(1);

    /*
     * Step 5-6:
     * Read operation fails with RTEMS_IO_ERROR code.
     * The function shall not update user pointer.
     */
    rc = rtems_bdbuf_read(test_dd, TEST_BLK_NUM, &bd2);
    if (rc != RTEMS_IO_ERROR || bd2 != NULL)
    {
        TEST_FAILED();
    }

    CONTINUE_MAIN(1);

    THREAD_END();
}
