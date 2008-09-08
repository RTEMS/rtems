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
| MBUS module (I2C bus) definitions                               |
+-----------------------------------------------------------------+
| This file has been adapted from an existing source code file,   |
| see the original file header below for reference                |
\*===============================================================*/
/*
 * MCF5206e MBUS module (I2C bus) driver header file
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef __MPC5200BSP_MPC5200MBUS_H__
#define __MPC5200BSP_MPC5200MBUS_H__

#include "../include/mpc5200.h"
#include "../include/i2c.h"

/* States of I2C machine */
typedef enum mpc5200mbus_i2c_state {
    STATE_UNINITIALIZED,
    STATE_IDLE,
    STATE_ADDR_7,
    STATE_ADDR_1_W,
    STATE_ADDR_1_R,
    STATE_SENDING,
    STATE_RECEIVING
} mpc5200mbus_i2c_state;

typedef struct mpc5200_mbus {
    enum mpc5200mbus_i2c_state state;/* State of I2C machine */
    i2c_message           *msg;  /* Pointer to the first message in transfer */
    int                    nmsg; /* Number of messages in transfer */
    i2c_message           *cmsg; /* Current message */
    int                    byte; /* Byte number in current message */
    rtems_id               sema; /* MBUS semaphore */
    i2c_transfer_done      done; /* Transfer done function */
    void *         done_arg_ptr; /* Done function argument pointer */
  int              bus_idx;  /* bus index: 0 or 1 */
} mpc5200mbus;

/* mpc5200mbus_initialize --
 *     Initialize ColdFire MBUS I2C bus controller.
 *
 * PARAMETERS:
 *     i2c_bus - pointer to the bus descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL, or RTEMS error code when initialization failed.
 */
rtems_status_code
mpc5200mbus_initialize(mpc5200mbus *i2c_bus);

/* mpc5200mbus_select_clock_divider --
 *     Select divider for system clock which is used for I2C bus clock
 *     generation. Not each divider can be selected for I2C bus; this
 *     function select nearest larger or equal divider, or maximum
 *     possible divider, if passed value greater.
 *
 * PARAMETERS:
 *     i2c_bus - pointer to the bus descriptor structure
 *     divider - system frequency divider for I2C serial clock.
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL, if operation performed successfully, or
 *     RTEMS error code when failed.
 */
rtems_status_code
mpc5200mbus_select_clock_divider(mpc5200mbus *i2c_bus, int divider);

/* mpc5200mbus_i2c_transfer --
 *     Initiate multiple-messages transfer over I2C bus via ColdFire MBUS
 *     controller.
 *
 * PARAMETERS:
 *     bus - pointer to MBUS controller descriptor
 *     nmsg - number of messages
 *     msg - pointer to messages array
 *     done - function which is called when transfer is finished
 *     done_arg_ptr - arbitrary argument ptr passed to done funciton
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if transfer initiated successfully, or error
 *     code when failed.
 */
rtems_status_code
mpc5200mbus_i2c_transfer(mpc5200mbus *bus, int nmsg, i2c_message *msg,
                     i2c_transfer_done done, void * done_arg_ptr);

/* mpc5200mbus_i2c_done --
 *     Close ColdFire MBUS I2C bus controller and release all resources.
 *
 * PARAMETERS:
 *     bus - pointer to MBUS controller descriptor
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL, if transfer initiated successfully, or error
 *     code when failed.
 */
rtems_status_code
mpc5200mbus_i2c_done(mpc5200mbus *i2c_bus);

/* mpc5200mbus_i2c_interrupt_handler --
 *     ColdFire MBUS I2C bus controller interrupt handler. This function
 *     called from real interrupt handler, and pointer to MBUS descriptor
 *     structure passed to this function.
 *
 * PARAMETERS:
 *     bus - pointert to the bus descriptor structure
 *
 * RETURNS:
 *     none
 */
void mpc5200mbus_i2c_interrupt_handler(mpc5200mbus *bus);

/* mpc5200mbus_poll --
 *     MBUS module poll routine; used to poll events when I2C driver
 *     operates in poll-driven mode.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void mpc5200mbus_poll(mpc5200mbus *bus);

#endif /* __MPC5200BSP_MPC5200MBUS_H__ */
