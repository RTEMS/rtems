/*! @file
 * @brief Common declarations of bdbuf tests.
 *
 * Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
 * Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef BDBUF_TESTS_H
#define BDBUF_TESTS_H

#include <stdio.h>
#include <errno.h>
#include <rtems/diskdevs.h>
#include <rtems/bdbuf.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void run_bdbuf_tests(void);

extern void bdbuf_test1_1_main(void);
extern void bdbuf_test1_2_main(void);
extern void bdbuf_test1_3_main(void);
extern void bdbuf_test1_4_main(void);
extern void bdbuf_test1_5_main(void);

extern void bdbuf_test2_1_main(void);
extern void bdbuf_test2_2_main(void);
extern void bdbuf_test2_3_main(void);
extern void bdbuf_test2_4_main(void);
extern void bdbuf_test2_5_main(void);

extern void bdbuf_test3_1_main(void);
extern void bdbuf_test3_2_main(void);
extern void bdbuf_test3_3_main(void);

extern void bdbuf_test4_1_main(void);
extern void bdbuf_test4_2_main(void);
extern void bdbuf_test4_3_main(void);

extern rtems_device_driver
test_disk_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg);

#define ARRAY_NUM(a_) (sizeof(a_) / sizeof(a_[0]))

/**
 * Configuration parameters of Test disk device.
 */
#define TEST_DISK_BLOCK_SIZE   512
#define TEST_DISK_BLOCK_NUM    1024
#define TEST_DISK_NAME         "/dev/testdisk"

/** Type of messages between test disk driver and main test task */
enum bdbuf_test_msg_type {
    /** Message sent by disk driver to main test task */
    BDBUF_TEST_MSG_TYPE_DRIVER_REQ,
    /**
     * Message sent main test task to disk driver
     * in reply to request.
     */
    BDBUF_TEST_MSG_TYPE_DRIVER_REPLY,
};

/**
 * Message used in communication between test disk driver
 * and main test task.
 * All R/W requests obtained by driver (from bdbuf library)
 * are directed to main test task. Then main test task 
 * sends a reply after which test disk driver notifies 
 * bdbuf library about operation complete event.
 */
typedef struct bdbuf_test_msg {
    /** Message type */
    enum bdbuf_test_msg_type type;

    union {
        struct driver_req {
            const rtems_disk_device *dd;
            uint32_t  req;
            void     *argp;
        } driver_req;
        struct driver_reply {
            int               ret_val;
            int               ret_errno;
            rtems_status_code res_status;
            int               res_errno;
        } driver_reply;
    } val;
} bdbuf_test_msg;


typedef enum bdbuf_rest_thread_prio {
    BDBUF_TEST_THREAD_PRIO_NORMAL = 3,
    BDBUF_TEST_THREAD_PRIO_LOW = 7,
    BDBUF_TEST_THREAD_PRIO_HIGH = 5,
} bdbuf_rest_thread_prio;


typedef struct test_ctx {
    /**
     * Message queue used by main task to get messages from
     * disk device driver.
     */
    Objects_Id test_qid;
   
    /**
     * Object ID for disk driver queue.
     * Test task will send messages to this queue in reply
     * to messages received on @a test_qid.
     */
    Objects_Id test_drv_qid;

    /** Test name */
    const char *test_name;

    /** 
     * Semaphore used for synchronization between test thread
     * and main test task.
     * Main test task blocks on one of these semaphores and an auxiliary thread
     * releases it in order to pass control to main task again.
     */
    rtems_id   test_sync_main[3];

    /**
     * This semaphore is used for synchornization between main test task
     * and the END of auxiliary threads.
     */
    rtems_id   test_end_main;

    /**
     * Semaphore used for synchronization between test thread
     * and main test task.
     * Thread #N blocks on this semaphore and the main task releases it
     * when it wants to pass control to the auxiliary thread #N.
     */
    rtems_id   test_sync[3];

    /** Task Id values of auxiliary threads */
    Objects_Id test_task[3];
} test_ctx;

extern test_ctx g_test_ctx;

/** Device ID used for testing */
extern rtems_disk_device *test_dd;

/**
 * Create a message queue for test driver that is used for
 * receiving messages from test task.
 *
 * @param[out] id  RTEMS message queue identifier
 *
 * @return Status of the operation
 */
extern rtems_status_code bdbuf_test_create_drv_rx_queue(Objects_Id *id);

/**
 * Obtain a message queue for test driver that is used for
 * sending messages to test task.
 *
 * @param[out] id  RTEMS message queue identifier
 *
 * @return Status of the operation
 */
extern rtems_status_code bdbuf_test_create_drv_tx_queue(Objects_Id *id);

/**
 * Start a separate test task represented by function @p func
 * and associated with task index @p idx.
 *
 * @param[in] idx   Task index
 * @param[in] func  Test task function
 */
extern rtems_status_code bdbuf_test_start_thread(unsigned int idx,
                                                 rtems_task_entry func);

/**
 * Finalize test run.
 */
extern rtems_status_code bdbuf_test_end(void);

/** Test result variable */
extern bool       good_test_result;

#define BDBUF_TEST_BLOCK_TIMEOUT (2 * rtems_clock_get_ticks_per_second())

#define SET_THREAD_PRIORITY(t_num_, prio_) \
    do {                                                        \
        rtems_task_priority old_prio;                           \
        rtems_status_code   rc_;                                \
                                                                \
        rc_ = rtems_task_set_priority(                          \
                g_test_ctx.test_task[(t_num_) - 1],             \
                BDBUF_TEST_THREAD_PRIO_ ## prio_, &old_prio);   \
        if (rc_ != RTEMS_SUCCESSFUL)                            \
        {                                                       \
            printk("Failed to change priority of test thread #" \
                   #t_num_ " in test %s\n", g_test_ctx.test_name);         \
            return;                                             \
        }                                                       \
    } while (0)

/**
 * Start thread number @p t_num_ with @p func_ function 
 * as an entry point of the thread.
 *
 * @param t_num_  thread number.
 * @param func_   thread entry point.
 */
#define START_THREAD(t_num_, func_) \
    do {                                                        \
        if (bdbuf_test_start_thread((t_num_) - 1, func_) !=     \
                RTEMS_SUCCESSFUL)                               \
        {                                                       \
            return;                                             \
        }                                                       \
        printk("Thread #" #t_num_ " started\n");                \
    } while (0)

/**
 * Wait for a message from device driver.
 *
 * @param[out] msg_  Pointer to a message data structure
 */
#define WAIT_DRV_MSG(msg_) \
    do {                                                        \
        rtems_status_code rc_;                                  \
        size_t            msg_size_ = sizeof(*(msg_));          \
                                                                \
        rc_ = rtems_message_queue_receive(g_test_ctx.test_qid,  \
                                          (msg_), &msg_size_,   \
                                          RTEMS_WAIT,           \
                                          RTEMS_NO_TIMEOUT);    \
        if (rc_ != RTEMS_SUCCESSFUL ||                          \
            msg_size_ != sizeof(*(msg_)))                       \
        {                                                       \
            printk("Error at %s:%d\n", __FILE__, __LINE__);     \
            return;                                             \
        }                                                       \
        if ((msg_)->type != BDBUF_TEST_MSG_TYPE_DRIVER_REQ)     \
        {                                                       \
            printk("Unexpected message received: %d\n",         \
                   (msg_)->type);                               \
            return;                                             \
        }                                                       \
    } while (0)

#define WAIT_DRV_MSG_WR(msg_) \
    do {                                                                \
        WAIT_DRV_MSG(msg_);                                             \
        if ((msg_)->val.driver_req.req != RTEMS_BLKIO_REQUEST ||        \
            (msg_)->val.driver_req.dd != test_dd ||                   \
            ((rtems_blkdev_request *)                                   \
                 ((msg_)->val.driver_req.argp))->req !=                 \
                 RTEMS_BLKDEV_REQ_WRITE)                                \
        {                                                               \
            printk("Unexpected message received by disk driver: "       \
                   "req - 0x%x (0x%x), dev - %d (%d)\n",                \
                   (msg_)->val.driver_req.req, RTEMS_BLKIO_REQUEST,     \
                   (msg_)->val.driver_req.dd, test_dd);               \
            return;                                                     \
        }                                                               \
    } while (0)

#define CHECK_NO_DRV_MSG() \
    do {                                                        \
        rtems_status_code rc_;                                  \
        bdbuf_test_msg    msg_;                                 \
        size_t            msg_size_ = sizeof(msg_);             \
                                                                \
        rc_ = rtems_message_queue_receive(g_test_ctx.test_qid,  \
                  (&msg_), &msg_size_,                          \
                  RTEMS_WAIT,                                   \
                  BDBUF_TEST_BLOCK_TIMEOUT);                    \
        if (rc_ != RTEMS_TIMEOUT)                               \
        {                                                       \
            printk("Error at %s:%d\n", __FILE__, __LINE__);     \
            return;                                             \
        }                                                       \
    } while (0)

extern bdbuf_test_msg test_drv_msg;

/**
 * Send a message to device driver (in order to complete
 * blocked I/O operation).
 *
 * @param ret_val_     Return value to use when returning from
 *                     ioctl() function.
 * @param ret_errno_   errno value to set to errno variable.
 * @param res_status_  In case return value from the ioctl()
 *                     function is equal to 0, this value 
 *                     is used as status code in asynchronous
 *                     notification.
 * @param res_errno_   In case return value from the ioctl()
 *                     function is equal to 0, this value
 *                     is used as error value in asynchronous
 *                     notification.
 *
 */
#define SEND_DRV_MSG(ret_val_, ret_errno_, res_status_, res_errno_) \
    do {                                                                \
        rtems_status_code rc_;                                          \
                                                                        \
        memset(&test_drv_msg, 0, sizeof(test_drv_msg));                 \
        test_drv_msg.type = BDBUF_TEST_MSG_TYPE_DRIVER_REPLY;           \
        test_drv_msg.val.driver_reply.ret_val = (ret_val_);             \
        test_drv_msg.val.driver_reply.ret_errno = (ret_errno_);         \
        test_drv_msg.val.driver_reply.res_status = (res_status_);       \
        test_drv_msg.val.driver_reply.res_errno = (res_errno_);         \
                                                                        \
        rc_ = rtems_message_queue_send(g_test_ctx.test_drv_qid,         \
                                       &test_drv_msg,                   \
                                       sizeof(test_drv_msg));           \
        if (rc_ != RTEMS_SUCCESSFUL)                                    \
        {                                                               \
            printk("Error while sending a message to "                  \
                   "disk driver: %u\n", rc_);                           \
            return;                                                     \
        }                                                               \
    } while (0)

/**
 * Block main test task until a thread passes back control
 * with CONTINUE_MAIN().
 *
 * @param t_num_  thread number from which the main thread 
 *                is waiting for a sync.
 */
#define WAIT_THREAD_SYNC(t_num_) \
    do {                                                                \
        rtems_status_code rc_;                                          \
                                                                        \
        rc_ = rtems_semaphore_obtain(                                   \
                g_test_ctx.test_sync_main[(t_num_) - 1],                \
                RTEMS_WAIT, RTEMS_NO_TIMEOUT);                          \
        if (rc_ != RTEMS_SUCCESSFUL)                                    \
        {                                                               \
            printk("Failed to get sync with a thread: %d\n", rc_);      \
            return;                                                     \
        }                                                               \
    } while (0)

/**
 * Check that a particular thread is blocked.
 *
 * @param t_num_  thread number that we want to check for blocking
 */
#define CHECK_THREAD_BLOCKED(t_num_) \
    do {                                                                \
        rtems_status_code rc_;                                          \
                                                                        \
        rc_ = rtems_semaphore_obtain(                                   \
                g_test_ctx.test_sync_main[(t_num_) - 1],                \
                RTEMS_WAIT, BDBUF_TEST_BLOCK_TIMEOUT);                  \
        if (rc_ != RTEMS_TIMEOUT)                                       \
        {                                                               \
            printk("Thread %d is not blocked at%s:%d\n",                \
                   t_num_, __FILE__, __LINE__);                         \
            return;                                                     \
        }                                                               \
    } while (0)

/**
 * Main test task gives control to the particular thread.
 *
 * @param t_num_  thread number to which main test task wants
 *                to give control.
 */
#define CONTINUE_THREAD(t_num_) \
    do {                                                        \
        rtems_status_code rc_;                                  \
                                                                \
        rc_ = rtems_semaphore_release(                          \
                g_test_ctx.test_sync[(t_num_) - 1]);            \
        if (rc_ != RTEMS_SUCCESSFUL)                            \
        {                                                       \
            printk("Failed to give control to thread #"         \
                   #t_num_ ": %d\n", rc_);                      \
            return;                                             \
        }                                                       \
    } while (0)

/**
 * Passes control back to the main test task from a thread.
 *
 * @param t_num_ Current thread number
 */
#define CONTINUE_MAIN(t_num_) \
    do {                                                        \
        rtems_status_code rc_;                                  \
                                                                \
        rc_ = rtems_semaphore_release(                          \
                g_test_ctx.test_sync_main[(t_num_) - 1]);       \
        if (rc_ != RTEMS_SUCCESSFUL)                            \
        {                                                       \
            printk("Failed to give control to "                 \
                   "main task: %d", rc_);                       \
            return;                                             \
        }                                                       \
        rc_ = rtems_semaphore_obtain(                           \
                g_test_ctx.test_sync[(t_num_) - 1],             \
                RTEMS_WAIT, RTEMS_NO_TIMEOUT);                  \
        if (rc_ != RTEMS_SUCCESSFUL)                            \
        {                                                       \
            printk("Failed to block on thread #"                \
                   #t_num_ ": %d\n", rc_);                      \
            return;                                             \
        }                                                       \
    } while (0)

#define WAIT_MAIN_SYNC(t_num_) \
    do {                                                        \
        rtems_status_code rc_;                                  \
                                                                \
        rc_ = rtems_semaphore_obtain(                           \
                g_test_ctx.test_sync[(t_num_) - 1],             \
                RTEMS_WAIT, RTEMS_NO_TIMEOUT);                  \
        if (rc_ != RTEMS_SUCCESSFUL)                            \
        {                                                       \
            printk("Failed to block on thread #"                \
                   #t_num_ ": %d\n", rc_);                      \
            return;                                             \
        }                                                       \
    } while (0)


#define TEST_START(test_name_) \
    do {                                \
        good_test_result = true;        \
        g_test_ctx.test_name = test_name_; \
        printk("%s - STARTED\n",        \
               g_test_ctx.test_name);              \
    } while (0)

#define TEST_CHECK_RESULT(step_) \
    do {                                        \
        if (!good_test_result)                  \
        {                                       \
            printk("TEST FAILED (Step %s)\n",   \
                   step_);                      \
            rtems_task_delete(RTEMS_SELF);      \
        }                                       \
        else                                    \
        {                                       \
            printk("%s: Step %s - OK\n",        \
                   g_test_ctx.test_name, step_);           \
        }                                       \
    } while (0)

#define TEST_END() \
    do {                                 \
        bdbuf_test_end();                \
                                         \
        if (good_test_result)            \
            printk("TEST PASSED\n");     \
        else                             \
            printk("TEST FAILED (END)"); \
    } while (0)

#define THREAD_END() \
    do {                                                        \
        rtems_status_code rc_;                                  \
                                                                \
        rc_ = rtems_semaphore_release(g_test_ctx.test_end_main);\
        if (rc_ != RTEMS_SUCCESSFUL)                            \
        {                                                       \
            printk("Failed to give control to "                 \
                   "main task: %d", rc_);                       \
            return;                                             \
        }                                                       \
        rtems_task_delete(RTEMS_SELF);                          \
    } while (0)

#define TEST_FAILED() \
    do {                                \
        good_test_result = false;       \
    } while (0)

#define SLEEP() \
    rtems_task_wake_after(BDBUF_TEST_BLOCK_TIMEOUT)


extern rtems_status_code
bdbuf_test_start_aux_task(rtems_name name,
                          rtems_task_entry entry_point,
                          rtems_task_argument arg,
                          Objects_Id *id);
#ifdef __cplusplus
}
#endif

#endif /* BDBUF_TESTS_H */

