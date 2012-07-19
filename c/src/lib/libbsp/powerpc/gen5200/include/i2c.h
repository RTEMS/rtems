/*
 * Generic I2C bus interface for RTEMS
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef __RTEMS__I2C_H__
#define __RTEMS__I2C_H__

#include <rtems.h>
#include <bsp.h>
/* This header file define the generic interface to i2c buses available in
 * system. This interface may be used by user applications or i2c-device
 * drivers (like RTC, NVRAM, etc).
 *
 * Functions i2c_initialize and i2c_transfer declared in this header usually
 * implemented in particular board support package. Usually this
 * implementation is a simple wrapper or multiplexor to I2C controller
 * driver which is available in system.  It may be generic "software
 * controller" I2C driver which control SDA and SCL signals directly (if SDA
 * and SCL is general-purpose I/O pins), or driver for hardware I2C
 * controller (standalone or integrated with processors: MBus controller in
 * ColdFire processors, I2C controller in PowerQUICC and so on).
 *
 * i2c_transfer is a very generic low-level function. Higher-level function
 * i2c_write, i2c_read, i2c_wrrd, i2c_wbrd is defined here too.
 */

/* I2C Bus Number type */
typedef uint32_t         i2c_bus_number;

/* I2C device address */
typedef uint16_t         i2c_address;

/* I2C error codes generated during message transfer */
typedef enum i2c_message_status {
    I2C_SUCCESSFUL = 0,
    I2C_TIMEOUT,
    I2C_NO_DEVICE,
    I2C_ARBITRATION_LOST,
    I2C_NO_ACKNOWLEDGE,
    I2C_NO_DATA,
    I2C_RESOURCE_NOT_AVAILABLE
} i2c_message_status;

/* I2C Message */
typedef struct i2c_message {
    i2c_address        addr;   /* I2C slave device address */
    uint16_t           flags;  /* message flags (see below) */
    i2c_message_status status; /* message transfer status code */
    uint16_t           len;    /* Number of bytes to read or write */
    uint8_t           *buf;    /* pointer to data array */
} i2c_message;

/* I2C message flag */
#define I2C_MSG_ADDR_10 (0x01)  /* 10-bit address */
#define I2C_MSG_WR      (0x02)  /* transfer direction for this message
                                   from master to slave */
#define I2C_MSG_ERRSKIP (0x04)  /* Skip message if last transfered message
                                   is failed */
/* Type for function which is called when transfer over I2C bus is finished */
typedef void (*i2c_transfer_done) (void * arg);

/* i2c_initialize --
 *     I2C driver initialization. This function usually called on device
 *     driver initialization state, before initialization task. All I2C
 *     buses are initialized; reasonable slow data transfer rate is
 *     selected for each bus.
 *
 * PARAMETERS:
 *     major - I2C device major number
 *     minor - I2C device minor number
 *     arg - RTEMS driver initialization argument
 *
 * RETURNS:
 *     RTEMS status code
 */
rtems_device_driver
i2c_initialize(rtems_device_major_number major,
               rtems_device_minor_number minor,
               void *arg);

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
i2c_select_clock_rate(i2c_bus_number bus, int bps);

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
 *     done_arg_ptr - arbitrary argument ptr passed to done funciton
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if transfer initiated successfully, or error
 *     code if something failed.
 */
rtems_status_code
i2c_transfer(i2c_bus_number bus, int nmsg, i2c_message *msg,
             i2c_transfer_done done, void *done_arg);

/* i2c_transfer_wait --
 *     Initiate I2C bus transfer and block until this transfer will be
 *     finished. This function wait the semaphore if system in
 *     SYSTEM_STATE_UP state, or poll done flag in other states.
 *
 * PARAMETERS:
 *     bus - I2C bus number
 *     msg - pointer to transfer messages array
 *     nmsg - number of messages in transfer
 *
 * RETURNS:
 *     I2C_SUCCESSFUL, if tranfer finished successfully,
 *     I2C_RESOURCE_NOT_AVAILABLE, if semaphore operations has failed,
 *     value of status field of first error-finished message in transfer,
 *     if something wrong.
 */
i2c_message_status
i2c_transfer_wait(i2c_bus_number bus, i2c_message *msg, int nmsg);

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
i2c_poll(i2c_bus_number bus);

/* i2c_write --
 *     Send single message over specified I2C bus to addressed device and
 *     wait while transfer is finished.
 *
 * PARAMETERS:
 *     bus  - I2C bus number
 *     addr - address of I2C device
 *     buf  - data to be sent to device
 *     size - data buffer size
 *
 * RETURNS:
 *     transfer status
 */
i2c_message_status
i2c_write(i2c_bus_number bus, i2c_address addr, void *buf, int size);

/* i2c_wrbyte --
 *     Send single one-byte long message over specified I2C bus to
 *     addressed device and wait while transfer is finished.
 *
 * PARAMETERS:
 *     bus  - I2C bus number
 *     addr - address of I2C device
 *     cmd  - byte message to be sent to device
 *
 * RETURNS:
 *     transfer status
 */
i2c_message_status
i2c_wrbyte(i2c_bus_number bus, i2c_address addr, uint8_t         cmd);

/* i2c_read --
 *     receive single message over specified I2C bus from addressed device.
 *     This call will wait while transfer is finished.
 *
 * PARAMETERS:
 *     bus  - I2C bus number
 *     addr - address of I2C device
 *     buf  - buffer for received message
 *     size - receive buffer size
 *
 * RETURNS:
 *     transfer status
 */
i2c_message_status
i2c_read(i2c_bus_number bus, i2c_address addr, void *buf, int size);

/* i2c_wrrd --
 *     Send message over I2C bus to specified device and receive message
 *     from the same device during single transfer.
 *
 * PARAMETERS:
 *     bus   - I2C bus number
 *     addr  - address of I2C device
 *     bufw  - data to be sent to device
 *     sizew - send data buffer size
 *     bufr  - buffer for received message
 *     sizer - receive buffer size
 *
 * RETURNS:
 *     transfer status
 */
i2c_message_status
i2c_wrrd(i2c_bus_number bus, i2c_address addr, void *bufw, int sizew,
         void *bufr, int sizer);

/* i2c_wbrd --
 *     Send one-byte message over I2C bus to specified device and receive
 *     message from the same device during single transfer.
 *
 * PARAMETERS:
 *     bus   - I2C bus number
 *     addr  - address of I2C device
 *     cmd   - one-byte message to be sent over I2C bus
 *     bufr  - buffer for received message
 *     sizer - receive buffer size
 *
 * RETURNS:
 *     transfer status
 */
i2c_message_status
i2c_wbrd(i2c_bus_number bus, i2c_address addr, uint8_t         cmd,
         void *bufr, int sizer);

#endif
