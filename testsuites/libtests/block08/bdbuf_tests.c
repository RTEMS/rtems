/*! @file
 * @brief Implementation of auxiliary functions of bdbuf tests.
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

#include <stdio.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/io.h>
#include <rtems/diskdevs.h>
#include <rtems/bdbuf.h>
#include "bdbuf_tests.h"


struct bdbuf_test_descr {
    void (* main)(void);
} bdbuf_tests[] = {
    { bdbuf_test1_1_main },
    { bdbuf_test1_2_main },
    { bdbuf_test1_3_main },
    { bdbuf_test1_4_main },
    { bdbuf_test1_5_main },
    
    { bdbuf_test2_1_main },
    { bdbuf_test2_2_main },
    
    { bdbuf_test3_1_main },
    { bdbuf_test3_2_main },
    { bdbuf_test3_3_main },
    
    { bdbuf_test4_1_main },
    { bdbuf_test4_2_main },
    { bdbuf_test4_3_main },
};

#define TEST_SEM_ATTRIBS RTEMS_DEFAULT_ATTRIBUTES

/** Device ID used for testing */
rtems_disk_device *test_dd = NULL;

/** Test result variable */
bool       good_test_result = true;

test_ctx g_test_ctx;

const char *test_name = "NO TEST";

bdbuf_test_msg test_drv_msg;

/** Count of messages in RX message queue used on disk driver side. */
#define TEST_DRV_RX_MQUEUE_COUNT 10
/** Name of disk driver RX message queue. */
#define TEST_DRV_RX_MQUEUE_NAME  (rtems_build_name( 'M', 'Q', 'D', ' ' ))

/** Count of messages in Test task RX message queue */
#define TEST_TASK_RX_MQUEUE_COUNT 10
/** Name of Test task RX message queue */
#define TEST_TASK_RX_MQUEUE_NAME  (rtems_build_name( 'M', 'Q', 'T', ' ' ))

rtems_status_code
bdbuf_test_start_aux_task(rtems_name name,
                          rtems_task_entry entry_point,
                          rtems_task_argument arg,
                          Objects_Id *id)
{
    rtems_status_code rc;
    Objects_Id        task_id;

    rc = rtems_task_create(name, BDBUF_TEST_THREAD_PRIO_NORMAL, 1024 * 2,
                           RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR,
                           RTEMS_LOCAL | RTEMS_NO_FLOATING_POINT,
                           &task_id);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("Failed to create task\n");
        return rc;
    }
    
    rc = rtems_task_start(task_id, entry_point, arg);
    if (rc != RTEMS_SUCCESSFUL)
    {
        printf("Failed to start task\n");
    }
    else
    {
        if (id != NULL)
            *id = task_id;
    }
    return rc;
}

void
run_bdbuf_tests()
{
    rtems_disk_device  *disk;
    rtems_status_code   sc;
    dev_t               dev = -1;
    dev_t               test_dev;
    unsigned int        i;

    rtems_device_major_number  major;
    rtems_driver_address_table testdisk = {
        test_disk_initialize,
        RTEMS_GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES
    };

    /* Create a message queue to get events from disk driver. */
    sc = rtems_message_queue_create(TEST_TASK_RX_MQUEUE_NAME,
                                    TEST_TASK_RX_MQUEUE_COUNT,
                                    sizeof(bdbuf_test_msg),
                                    RTEMS_DEFAULT_ATTRIBUTES,
                                    &g_test_ctx.test_qid);

    if (sc != RTEMS_SUCCESSFUL)
    {
        printk("Failed to create message queue for test task: %u\n", sc);
        return;
    }

    /* Register a disk device that is used in tests */
    sc = rtems_io_register_driver(0, &testdisk, &major);
    if (sc != RTEMS_SUCCESSFUL)
    {
        printk("Failed to register TEST DEVICE: %d\n", sc);
        return;
    }

    test_dev = -1;
    while ((disk = rtems_disk_next(dev)) != NULL)
    {
        printk("DEV: %s [%lu]\n", disk->name, disk->size);
        dev = disk->dev;
        if (strcmp(disk->name, TEST_DISK_NAME) == 0)
            test_dev = dev;
        rtems_disk_release(disk);
    }

    if (test_dev == (dev_t)-1)
    {
        printf("Failed to find %s disk\n", TEST_DISK_NAME);
        return;
    }

    test_dd = rtems_disk_obtain(test_dev);
    if (test_dd == NULL)
    {
        printf("Failed to obtain %s disk\n", TEST_DISK_NAME);
        return;
    }

    /*
     * On initialization test disk device driver registers
     * its RX message queue, so we just need to locate it.
     */
    sc = rtems_message_queue_ident(TEST_DRV_RX_MQUEUE_NAME,
                                   RTEMS_SEARCH_ALL_NODES,
                                   &g_test_ctx.test_drv_qid);
    if (sc != RTEMS_SUCCESSFUL)
    {
        printf("Failed to find Test Driver Queue: %u\n", sc);
        return;
    }

    for (i = 0; i < ARRAY_NUM(g_test_ctx.test_sync_main); i++)
    {
        sc = rtems_semaphore_create(rtems_build_name('T', 'S', 'M', '0' + i),
                                    0, TEST_SEM_ATTRIBS, 0,
                                    &g_test_ctx.test_sync_main[i]);
        if (sc != RTEMS_SUCCESSFUL)
        {
            printk("Failed to create sync sem for test task: %u\n", sc);
            return;
        }
    }

    for (i = 0; i < ARRAY_NUM(g_test_ctx.test_sync); i++)
    {
        sc = rtems_semaphore_create(rtems_build_name('T', 'S', 'T', '0' + i),
                                    0, TEST_SEM_ATTRIBS, 0,
                                    &g_test_ctx.test_sync[i]);
        if (sc != RTEMS_SUCCESSFUL)
        {
            printk("Failed to create sync sem for test task #%d: %u\n", i + 1, sc);
            return;
        }
    }
    
    sc = rtems_semaphore_create(rtems_build_name('T', 'S', 'M', 'E'),
                                0, TEST_SEM_ATTRIBS, 0,
                                &g_test_ctx.test_end_main);
    if (sc != RTEMS_SUCCESSFUL)
    {
        printk("Failed to create end sync sem for test task: %u\n", sc);
        return;
    }

    for (i = 0; i < ARRAY_NUM(g_test_ctx.test_task); i++)
        g_test_ctx.test_task[i] = OBJECTS_ID_NONE;

    for (i = 0; i < sizeof(bdbuf_tests) / sizeof(bdbuf_tests[0]); i++)
    {
        bdbuf_tests[i].main();
    }
}


rtems_status_code
bdbuf_test_create_drv_rx_queue(Objects_Id *id)
{
    return rtems_message_queue_create(TEST_DRV_RX_MQUEUE_NAME,
                                      TEST_DRV_RX_MQUEUE_COUNT,
                                      sizeof(bdbuf_test_msg),
                                      RTEMS_DEFAULT_ATTRIBUTES,
                                      id);
}

rtems_status_code
bdbuf_test_create_drv_tx_queue(Objects_Id *id)
{
    return  rtems_message_queue_ident(TEST_TASK_RX_MQUEUE_NAME,
                                      RTEMS_SEARCH_ALL_NODES,
                                      id);
}

rtems_status_code
bdbuf_test_start_thread(unsigned int idx, rtems_task_entry func)
{
    rtems_status_code sc;

    if (g_test_ctx.test_task[idx] != OBJECTS_ID_NONE)
    {
        sc = rtems_task_delete(g_test_ctx.test_task[idx]);
        if (sc != RTEMS_SUCCESSFUL)
        {
            printk("Failed to delete test thread %u in test %s\n",
                   idx + 1, g_test_ctx.test_name);
            return sc;
        }
    }
    sc = bdbuf_test_start_aux_task(
            rtems_build_name('T', 'S', '.', '0' + idx),
            func, (rtems_task_argument)(NULL),
            &g_test_ctx.test_task[idx]);
    return sc;
}

rtems_status_code
bdbuf_test_end()
{
    rtems_status_code sc;
    unsigned int      i;

    for (i = 0; i < ARRAY_NUM(g_test_ctx.test_task); i++)
    {
        if (g_test_ctx.test_task[i] != OBJECTS_ID_NONE)
        {
            sc = rtems_semaphore_obtain(g_test_ctx.test_end_main,
                                        RTEMS_WAIT, RTEMS_NO_TIMEOUT);
            if (sc != RTEMS_SUCCESSFUL)
            {
                printk("Failed to get a thread stopped\n");
            }
            g_test_ctx.test_task[i] = OBJECTS_ID_NONE;
        }
    }
    return RTEMS_SUCCESSFUL;
}

