/* I2C bus common (driver-independent) primitives implementation.
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
#include <i2c.h>

/* i2c_transfer_sema_done_func --
 *     This function called from I2C driver layer to signal that I2C
 *     transfer is finished. This function resumes of task execution which
 *     has invoked blocking I2C primitive.
 *
 * PARAMETERS:
 *     arg - done function argument; it is RTEMS semaphore ID.
 */
static void
i2c_transfer_sema_done_func(void * arg)
{
    rtems_id sema = *(rtems_id *)arg;
    rtems_semaphore_release(sema);
}

/* i2c_transfer_poll_done_func --
 *     This function called from I2C driver layer to signal that I2C
 *     transfer is finished. This function set the flag polled by waiting
 *     function.
 *
 * PARAMETERS:
 *     arg - done function argument; address of poll_done_flag
 */
static void
i2c_transfer_poll_done_func(void *arg)
{
    bool *poll_done_flag = (bool *)arg;
    *poll_done_flag = true;
}

/* i2c_transfer_wait_sema --
 *     Initiate I2C bus transfer and block on temporary created semaphore
 *     until this transfer will be finished.
 *
 * PARAMETERS:
 *     bus - I2C bus number
 *     msg - pointer to transfer messages array
 *     nmsg - number of messages in transfer
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL, if tranfer finished successfully,
 *     or RTEMS status code if semaphore operations has failed.
 */
static i2c_message_status
i2c_transfer_wait_sema(i2c_bus_number bus, i2c_message *msg, int nmsg)
{
    rtems_status_code sc;
    rtems_id sema;
    sc = rtems_semaphore_create(
        rtems_build_name('I', '2', 'C', 'S'),
        0,
        RTEMS_COUNTING_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY |
        RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL,
        0,
        &sema
    );
    if (sc != RTEMS_SUCCESSFUL)
        return I2C_RESOURCE_NOT_AVAILABLE;
    sc = i2c_transfer(bus, nmsg, msg,
		      i2c_transfer_sema_done_func, &sema);
    if (sc != RTEMS_SUCCESSFUL)
    {
        rtems_semaphore_delete(sema);
        return sc;
    }
    rtems_semaphore_obtain(sema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    sc = rtems_semaphore_delete(sema);
    return sc;
}

/* i2c_transfer_wait_poll --
 *     Initiate I2C bus transfer and wait by poll transaction done flag until
 *     this transfer will be finished.
 *
 * PARAMETERS:
 *     bus - I2C bus number
 *     msg - pointer to transfer messages array
 *     nmsg - number of messages in transfer
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
static rtems_status_code
i2c_transfer_wait_poll(i2c_bus_number bus, i2c_message *msg, int nmsg)
{
  /*
   * this looks nasty, but is correct:
   * we wait in this function, until the poll_done_flag is
   * set deep inside the i2c_poll() function
   */
    volatile bool poll_done_flag;
    rtems_status_code sc;
    poll_done_flag = false;
    sc = i2c_transfer(bus, nmsg, msg,
		      i2c_transfer_poll_done_func,(void *)&poll_done_flag);
    if (sc != RTEMS_SUCCESSFUL)
        return sc;
    while (poll_done_flag == false)
    {
        i2c_poll(bus);
    }
    return RTEMS_SUCCESSFUL;
}

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
i2c_transfer_wait(i2c_bus_number bus, i2c_message *msg, int nmsg)
{
    rtems_status_code sc;
    int i;
    if (_System_state_Is_up(_System_state_Get()))
    {
        sc = i2c_transfer_wait_sema(bus, msg, nmsg);
    }
    else
    {
        sc = i2c_transfer_wait_poll(bus, msg, nmsg);
    }

    if (sc != RTEMS_SUCCESSFUL)
        return I2C_RESOURCE_NOT_AVAILABLE;

    for (i = 0; i < nmsg; i++)
    {
        if (msg[i].status != I2C_SUCCESSFUL)
        {
            return msg[i].status;
        }
    }
    return I2C_SUCCESSFUL;
}

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
i2c_write(i2c_bus_number bus, i2c_address addr, void *buf, int size)
{
    i2c_message msg;
    msg.addr = addr;
    msg.flags = I2C_MSG_WR;
    if (addr > 0xff)
        msg.flags |= I2C_MSG_ADDR_10;
    msg.status = 0;
    msg.len = size;
    msg.buf = buf;
    return i2c_transfer_wait(bus, &msg, 1);
}

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
i2c_wrbyte(i2c_bus_number bus, i2c_address addr, uint8_t         cmd)
{
    i2c_message msg;
    uint8_t         data = cmd;
    msg.addr = addr;
    msg.flags = I2C_MSG_WR;
    if (addr > 0xff)
        msg.flags |= I2C_MSG_ADDR_10;
    msg.status = 0;
    msg.len = sizeof(data);
    msg.buf = &data;
    return i2c_transfer_wait(bus, &msg, 1);
}

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
i2c_read(i2c_bus_number bus, i2c_address addr, void *buf, int size)
{
    i2c_message msg;
    msg.addr = addr;
    msg.flags = 0;
    if (addr > 0xff)
        msg.flags |= I2C_MSG_ADDR_10;
    msg.status = 0;
    msg.len = size;
    msg.buf = buf;
    return i2c_transfer_wait(bus, &msg, 1);
}

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
         void *bufr, int sizer)
{
    i2c_message msg[2];
    msg[0].addr = addr;
    msg[0].flags = I2C_MSG_WR | I2C_MSG_ERRSKIP;
    if (addr > 0xff)
        msg[0].flags |= I2C_MSG_ADDR_10;
    msg[0].status = 0;
    msg[0].len = sizew;
    msg[0].buf = bufw;

    msg[1].addr = addr;
    msg[1].flags = 0;
    if (addr > 0xff)
        msg[1].flags |= I2C_MSG_ADDR_10;
    msg[1].status = 0;
    msg[1].len = sizer;
    msg[1].buf = bufr;

    return i2c_transfer_wait(bus, msg, 2);
}

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
         void *bufr, int sizer)
{
    i2c_message msg[2];
    uint8_t         bufw = cmd;
    msg[0].addr = addr;
    msg[0].flags = I2C_MSG_WR | I2C_MSG_ERRSKIP;
    if (addr > 0xff)
        msg[0].flags |= I2C_MSG_ADDR_10;
    msg[0].status = 0;
    msg[0].len = sizeof(bufw);
    msg[0].buf = &bufw;

    msg[1].addr = addr;
    msg[1].flags = I2C_MSG_ERRSKIP;
    if (addr > 0xff)
        msg[1].flags |= I2C_MSG_ADDR_10;
    msg[1].status = 0;
    msg[1].len = sizer;
    msg[1].buf = bufr;

    return i2c_transfer_wait(bus, msg, 2);
}
