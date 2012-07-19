/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| I2C driver for MPC5200                                          |
+-----------------------------------------------------------------+
| This file has been adapted from an existing source code file,   |
| see the original file header below for reference                |
\*===============================================================*/

/* I2C driver for MCF5206eLITE board. I2C bus accessed through on-chip
 * MCF5206e MBUS controller.
 *
 * The purpose of this module is to perform I2C driver initialization
 * and serialize I2C transfers.
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <stdlib.h>
#include <string.h>

#include "../include/i2c.h"
#include "../include/i2cdrv.h"
#include "mpc5200mbus.h"

#ifndef I2C_NUMBER_OF_BUSES
#define I2C_NUMBER_OF_BUSES (2)
#endif

#ifndef I2C_SELECT_BUS
#define I2C_SELECT_BUS(bus)
#endif

/*
 * Few I2C transfers may be posted simultaneously, but MBUS driver is able
 * to process it one-by-one. To serialize transfers, function i2c_transfer
 * put transfer information to the queue and initiate new transfers if MBUS
 * driver is not busy. When driver is busy, next transfer is dequeued
 * when current active transfer is finished.
 */

/*
 * i2c_qel - I2C transfers queue element; contain information about
 * delayed transfer
 */
typedef struct i2c_qel {
    i2c_bus_number    bus;      /* I2C bus number */
    i2c_message      *msg;      /* pointer to the transfer' messages array */
    int               nmsg;     /* number of messages in transfer */
    i2c_transfer_done done;     /* transfer done callback function */
    void *    done_arg_ptr;     /* arbitrary arg pointer to done callback */
} i2c_qel;

/* Memory for I2C transfer queue. This queue represented like a ring buffer */
static i2c_qel *tqueue;

/* Maximum number of elements in transfer queue */
static int tqueue_size;

/* Position of next free element in a ring buffer */
static volatile int tqueue_head;

/* Position of the first element in transfer queue */
static volatile int tqueue_tail;

/* MBus I2C bus controller busy flag */
static volatile bool mbus_busy;

/* MBus I2C bus controller descriptor */
static mpc5200mbus mbus[I2C_NUMBER_OF_BUSES];

/* Clock rate selected for each of bus */
static int i2cdrv_bus_clock_div[I2C_NUMBER_OF_BUSES];

/* Currently selected I2C bus clock rate */
static int i2cdrv_bus_clock_div_current;

/* Forward function declaration */
static void i2cdrv_unload(void);

/* i2cdrv_done --
 *     Callback function which is called from MBus low-level driver when
 *     transfer is finished.
 */
static void
i2cdrv_done(void * arg_ptr)
{
    rtems_interrupt_level level;
    i2c_qel *qel = tqueue + tqueue_tail;
    qel->done(qel->done_arg_ptr);
    rtems_interrupt_disable(level);
    tqueue_tail = (tqueue_tail + 1) % tqueue_size;
    mbus_busy = false;
    rtems_interrupt_enable(level);
    i2cdrv_unload();
}

/* i2cdrv_unload --
 *     If MBUS controller is not busy and transfer waiting in a queue,
 *     initiate processing of the next transfer in queue.
 */
static void
i2cdrv_unload(void)
{
    rtems_interrupt_level level;
    i2c_qel *qel;
    rtems_status_code sc;
    rtems_interrupt_disable(level);
    if (!mbus_busy && (tqueue_head != tqueue_tail))
    {
        mbus_busy = true;
        rtems_interrupt_enable(level);
        qel = tqueue + tqueue_tail;

        I2C_SELECT_BUS(qel->bus);
        if (i2cdrv_bus_clock_div[qel->bus] != i2cdrv_bus_clock_div_current)
        {
            i2cdrv_bus_clock_div_current = i2cdrv_bus_clock_div[qel->bus];
            mpc5200mbus_select_clock_divider(&mbus[qel->bus], i2cdrv_bus_clock_div_current);
        }
        sc = mpc5200mbus_i2c_transfer(&mbus[qel->bus], qel->nmsg, qel->msg,
				      i2cdrv_done,qel);
        if (sc != RTEMS_SUCCESSFUL)
        {
            int i;
            for (i = 0; i < qel->nmsg; i++)
            {
                qel->msg[i].status = I2C_RESOURCE_NOT_AVAILABLE;
            }
            i2cdrv_done(qel);
        }
    }
    else
    {
        rtems_interrupt_enable(level);
    }
}

/* i2c_transfer --
 *     Initiate multiple-messages transfer over specified I2C bus or
 *     put request into queue if bus or some other resource is busy. (This
 *     is non-blocking function).
 *
 * PARAMETERS:
 *     bus - I2C bus number
 *     nmsg - number of messages
 *     msg - pointer to messages array
 *     done - function which is called when transfer is finished
 *     done_arg_ptr - arbitrary argument pointer passed to done funciton
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if transfer initiated successfully, or error
 *     code if something failed.
 */
rtems_status_code
i2c_transfer(i2c_bus_number bus, int nmsg, i2c_message *msg,
             i2c_transfer_done done, void *     done_arg_ptr)
{
    i2c_qel qel;
    rtems_interrupt_level level;

    if (bus >= I2C_NUMBER_OF_BUSES)
    {
        return RTEMS_INVALID_NUMBER;
    }

    if (msg == NULL)
    {
        return RTEMS_INVALID_ADDRESS;
    }

    qel.bus = bus;
    qel.msg = msg;
    qel.nmsg = nmsg;
    qel.done = done;
    qel.done_arg_ptr = done_arg_ptr;
    rtems_interrupt_disable(level);
    if ((tqueue_head + 1) % tqueue_size == tqueue_tail)
    {
        rtems_interrupt_enable(level);
        return RTEMS_TOO_MANY;
    }
    memcpy(tqueue + tqueue_head, &qel, sizeof(qel));
    tqueue_head = (tqueue_head + 1) % tqueue_size;
    rtems_interrupt_enable(level);
    i2cdrv_unload();
    return RTEMS_SUCCESSFUL;
}

/* i2cdrv_initialize --
 *     I2C driver initialization (rtems I/O driver primitive)
 */
rtems_device_driver
i2cdrv_initialize(rtems_device_major_number major,
                  rtems_device_minor_number minor,
                  void *arg)
{
    int i;
    rtems_status_code sc;
    mbus_busy = false;
    tqueue_tail = tqueue_head = 0;
    tqueue_size = 32;
    tqueue = calloc(tqueue_size, sizeof(i2c_qel));

    for (i = 0; i < I2C_NUMBER_OF_BUSES; i++)
    {
      mbus[i].bus_idx = i;
      sc = mpc5200mbus_initialize(&mbus[i]);
      if (sc != RTEMS_SUCCESSFUL)
        return sc;
    }

    for (i = 0; i < I2C_NUMBER_OF_BUSES; i++)
    {
        sc = i2c_select_clock_rate(i, 100000);
        if (sc != RTEMS_SUCCESSFUL)
            return sc;
    }
    i2cdrv_bus_clock_div_current = -1;
    return RTEMS_SUCCESSFUL;
}

/* i2c_select_clock_rate --
 *     select I2C bus clock rate for specified bus. Some bus controller do not
 *     allow to select arbitrary clock rate; in this case nearest possible
 *     slower clock rate is selected.
 *
 * PARAMETERS:
 *     bus - I2C bus number
 *     bps - data transfer rate for this bytes in bits per second
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL, if operation performed successfully,
 *     RTEMS_INVALID_NUMBER, if wrong bus number is specified,
 *     RTEMS_UNSATISFIED, if bus do not support data transfer rate selection
 *     or specified data transfer rate could not be used.
 */
rtems_status_code
i2c_select_clock_rate(i2c_bus_number bus, int bps)
{
    int div;
    if (bus >= I2C_NUMBER_OF_BUSES)
        return RTEMS_INVALID_NUMBER;

    if (bps == 0)
        return RTEMS_UNSATISFIED;

    div = IPB_CLOCK / bps;
    i2cdrv_bus_clock_div[bus] = div;
    return RTEMS_SUCCESSFUL;
}

/* i2c_poll --
 *     Poll I2C bus controller for events and hanle it. This function is
 *     used when I2C driver operates in poll-driven mode.
 *
 * PARAMETERS:
 *     bus - bus number to be polled
 *
 * RETURNS:
 *     none
 */
void
i2c_poll(i2c_bus_number bus)
{
    mpc5200mbus_poll(&mbus[bus]);
}
