/*
 * MCF5206e MBUS module (I2C bus) driver header file
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __MCFBSP_MCFMBUS_H__
#define __MCFBSP_MCFMBUS_H__

#include "mcf5206e.h"
#include "i2c.h"

/* States of I2C machine */
typedef enum mcfmbus_i2c_state {
    STATE_IDLE,
    STATE_ADDR_7,
    STATE_ADDR_1_W,
    STATE_ADDR_1_R,
    STATE_SENDING,
    STATE_RECEIVING
} mcfmbus_i2c_state;

typedef struct mcfmbus {
    uint32_t         base; /* ColdFire internal peripherial base
                                    address */
    enum mcfmbus_i2c_state state;/* State of I2C machine */
    i2c_message           *msg;  /* Pointer to the first message in transfer */
    int                    nmsg; /* Number of messages in transfer */
    i2c_message           *cmsg; /* Current message */
    int                    byte; /* Byte number in current message */
    rtems_isr_entry        oldisr; /* Old interrupt handler */
    rtems_id               sema; /* MBUS semaphore */
    i2c_transfer_done      done; /* Transfer done function */
    uint32_t       done_arg_ptr; /* Done function argument ptr */
} mcfmbus;

/* mcfmbus_initialize --
 *     Initialize ColdFire MBUS I2C bus controller.
 *
 * PARAMETERS:
 *     i2c_bus - pointer to the bus descriptor structure
 *     base    - ColdFire internal peripherial base address
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL, or RTEMS error code when initialization failed.
 */
rtems_status_code
mcfmbus_initialize(mcfmbus *i2c_bus, uint32_t   base);

/* mcfmbus_select_clock_divider --
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
mcfmbus_select_clock_divider(mcfmbus *i2c_bus, int divider);

/* mcfmbus_i2c_transfer --
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
mcfmbus_i2c_transfer(mcfmbus *bus, int nmsg, i2c_message *msg,
                     i2c_transfer_done done, void *done_arg_ptr);

/* mcfmbus_i2c_done --
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
mcfmbus_i2c_done(mcfmbus *i2c_bus);

/* mcfmbus_i2c_interrupt_handler --
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
void mcfmbus_i2c_interrupt_handler(mcfmbus *bus);

/* mcfmbus_poll --
 *     MBUS module poll routine; used to poll events when I2C driver
 *     operates in poll-driven mode.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void mcfmbus_poll(mcfmbus *bus);

#endif /* __MCFBSP_MCFMBUS_H__ */
