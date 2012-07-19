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
| MBUS module (I2C bus) driver                                    |
+-----------------------------------------------------------------+
| This file has been adapted from an existing source code file,   |
| see the original file header below for reference                |
\*===============================================================*/

/* MCF5206e MBUS module (I2C bus) driver
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */
#include "mpc5200mbus.h"
#include "../include/mpc5200.h"
#include <bsp/irq.h>
#include "../include/i2c.h"

/* Events of I2C machine */
typedef enum i2c_event {
    EVENT_NONE,      /* Spurious event */
    EVENT_TRANSFER,  /* Start new transfer */
    EVENT_NEXTMSG,   /* Start processing of next message in transfer */
    EVENT_ACK,       /* Sending finished with ACK */
    EVENT_NACK,      /* Sending finished with NACK */
    EVENT_TIMEOUT,   /* Timeout occured */
    EVENT_DATA_RECV, /* Data received */
    EVENT_ARB_LOST,  /* Arbitration lost */
    EVENT_SLAVE      /* Addressed as a slave */
} i2c_event;

/*** Auxillary primitives ***/

/* Change state of finite state machine */
#define next_state(bus,new_state) \
    do {                             \
        (bus)->state = (new_state);  \
    } while (0)

/* Initiate start condition on the I2C bus */
#define mpc5200mbus_start(bus) \
    do {                                                  \
        mpc5200.i2c_regs[bus->bus_idx].mcr |= MPC5200_I2C_MCR_MSTA;  \
    } while (0)

/* Initiate stop condition on the I2C bus */
#define mpc5200mbus_stop(bus) \
    do {                                                    \
        mpc5200.i2c_regs[bus->bus_idx].mcr &= ~MPC5200_I2C_MCR_MSTA; \
    } while (0)

/* Initiate repeat start condition on the I2C bus */
#define mpc5200mbus_rstart(bus) \
    do {                                                    \
        mpc5200.i2c_regs[bus->bus_idx].mcr |= MPC5200_I2C_MCR_RSTA;  \
    } while (0)

/* Send byte to the bus */
#define mpc5200mbus_send(bus,byte) \
    do {                                      \
        mpc5200.i2c_regs[bus->bus_idx].mdr = (byte); \
    } while (0)

/* Set transmit mode */
#define mpc5200mbus_tx_mode(bus) \
    do {                                                     \
        mpc5200.i2c_regs[bus->bus_idx].mcr |= MPC5200_I2C_MCR_MTX;    \
    } while (0)

/* Set receive mode */
#define mpc5200mbus_rx_mode(bus) \
    do {                                                 \
        mpc5200.i2c_regs[bus->bus_idx].mcr &= ~MPC5200_I2C_MCR_MTX;   \
        (void)mpc5200.i2c_regs[bus->bus_idx].mdr;                 \
    } while (0)


/* Transmit acknowledge when byte received */
#define mpc5200mbus_send_ack(bus) \
    do {                                                     \
         mpc5200.i2c_regs[bus->bus_idx].mcr &= ~MPC5200_I2C_MCR_TXAK;  \
    } while (0)

/* DO NOT transmit acknowledge when byte received */
#define mpc5200mbus_send_nack(bus) \
    do {                                                     \
        mpc5200.i2c_regs[bus->bus_idx].mcr |= MPC5200_I2C_MCR_TXAK;   \
    } while (0)

#define mpc5200mbus_error(bus,err_status) \
    do {                                                       \
        do {                                                   \
           (bus)->cmsg->status = (err_status);                 \
           (bus)->cmsg++;                                      \
        } while (((bus)->cmsg - (bus)->msg < (bus)->nmsg) &&   \
                 ((bus)->cmsg->flags & I2C_MSG_ERRSKIP));      \
        bus->cmsg--;                                           \
    } while (0)

/* mpc5200mbus_get_event --
 *     Read MBUS module status register, determine interrupt reason and
 *     return appropriate event.
 *
 * PARAMETERS:
 *     bus - pointer to MBUS module descriptor structure
 *
 * RETURNS:
 *     event code
 */
static i2c_event
mpc5200mbus_get_event(mpc5200mbus *bus)
{
    i2c_event event;
    uint8_t   status, control;
    rtems_interrupt_level level;
    rtems_interrupt_disable(level);
    status = mpc5200.i2c_regs[bus->bus_idx].msr;
    control = mpc5200.i2c_regs[bus->bus_idx].mcr;
    if (status & MPC5200_I2C_MSR_MIF) /* Interrupt occured */
    {
        if (status & MPC5200_I2C_MSR_MAAS)
        {
            event = EVENT_SLAVE;
            mpc5200.i2c_regs[bus->bus_idx].mcr = control; /* To clear Addressed As Slave
                                                    condition */
        }
        else if (status & MPC5200_I2C_MSR_MAL) /* Arbitration lost */
        {
            mpc5200.i2c_regs[bus->bus_idx].msr = status & ~MPC5200_I2C_MSR_MAL;
            event = EVENT_ARB_LOST;
        }
        else if (control & MPC5200_I2C_MCR_MTX) /* Trasmit mode */
        {
            if (status & MPC5200_I2C_MSR_RXAK)
                event = EVENT_NACK;
            else
                event = EVENT_ACK;
        }
        else /* Received */
        {
            event = EVENT_DATA_RECV;
        }

        /* Clear interrupt condition */
        mpc5200.i2c_regs[bus->bus_idx].msr &= ~MPC5200_I2C_MSR_MIF;
    }
    else
    {
        event = EVENT_NONE;
    }
    rtems_interrupt_enable(level);
    return event;
}

static void
mpc5200mbus_machine_error(mpc5200mbus *bus, i2c_event event)
{
    return;
}

/* mpc5200mbus_machine --
 *     finite state machine for I2C bus protocol
 *
 * PARAMETERS:
 *     bus - pointer to ColdFire MBUS descriptor structure
 *     event - I2C event
 *
 * RETURNS:
 *     none
 */
static void
mpc5200mbus_machine(mpc5200mbus *bus, i2c_event event)
{
    uint8_t   b;
    switch (bus->state)
    {

        case STATE_UNINITIALIZED:
	  /* this should never happen. */
	  mpc5200mbus_machine_error(bus, event);
	  break;
        case STATE_IDLE:
            switch (event)
            {
                case EVENT_NEXTMSG:  /* Start new message processing */
                    bus->cmsg++;
                    /* FALLTHRU */

                case EVENT_TRANSFER: /* Initiate new transfer */
                    if (bus->cmsg - bus->msg >= bus->nmsg)
                    {
                        mpc5200mbus_stop(bus);
                        next_state(bus, STATE_IDLE);
                        bus->msg = bus->cmsg = NULL;
                        bus->nmsg = bus->byte = 0;
                        bus->done(bus->done_arg_ptr);
                        break;
                    }

                    /* Initiate START or REPEATED START condition on the bus */
                    if (event == EVENT_TRANSFER)
                    {
                        mpc5200mbus_start(bus);
                    }
                    else /* (event == EVENT_NEXTMSG) */
                    {
                        mpc5200mbus_rstart(bus);
                    }

                    bus->byte = 0;
                    mpc5200mbus_tx_mode(bus);

                    /* Initiate slave address sending */
                    if (bus->cmsg->flags & I2C_MSG_ADDR_10)
                    {
                        i2c_address a = bus->cmsg->addr;
                        b = 0xf0 | (((a >> 8) & 0x03) << 1);
                        if (bus->cmsg->flags & I2C_MSG_WR)
                        {
                            mpc5200mbus_send(bus, b);
                            next_state(bus, STATE_ADDR_1_W);
                        }
                        else
                        {
                            mpc5200mbus_send(bus, b | 1);
                            next_state(bus, STATE_ADDR_1_R);
                        }
                    }
                    else
                    {
                        b = (bus->cmsg->addr & ~0x01);

                        if (bus->cmsg->flags & I2C_MSG_WR)
                        {
                            next_state(bus, STATE_SENDING);
                        }
                        else
                        {
                            next_state(bus, STATE_ADDR_7);
                            b |= 1;
                        }

                        mpc5200mbus_send(bus, b);
                    }
                    break;

                default:
                    mpc5200mbus_machine_error(bus, event);
                    break;
            }
            break;

        case STATE_ADDR_7:
            switch (event)
            {
                case EVENT_ACK:
                    mpc5200mbus_rx_mode(bus);
                    if (bus->cmsg->len <= 1)
                        mpc5200mbus_send_nack(bus);
                    else
                        mpc5200mbus_send_ack(bus);
                    next_state(bus, STATE_RECEIVING);
                    break;

                case EVENT_NACK:
                    mpc5200mbus_error(bus, I2C_NO_DEVICE);
                    next_state(bus, STATE_IDLE);
                    mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    break;

                case EVENT_ARB_LOST:
                    mpc5200mbus_error(bus, I2C_ARBITRATION_LOST);
                    next_state(bus, STATE_IDLE);
                    mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    break;

                default:
                    mpc5200mbus_machine_error(bus, event);
                    break;
            }
            break;

        case STATE_ADDR_1_R:
        case STATE_ADDR_1_W:
            switch (event)
            {
                case EVENT_ACK:
                {
                    uint8_t   b = (bus->cmsg->addr & 0xff);
                    mpc5200mbus_send(bus, b);
                    if (bus->state == STATE_ADDR_1_W)
                    {
                        next_state(bus, STATE_SENDING);
                    }
                    else
                    {
                        i2c_address a;
                        mpc5200mbus_rstart(bus);
                        mpc5200mbus_tx_mode(bus);
                        a = bus->cmsg->addr;
                        b = 0xf0 | (((a >> 8) & 0x03) << 1) | 1;
                        mpc5200mbus_send(bus, b);
                        next_state(bus, STATE_ADDR_7);
                    }
                    break;
                }

                case EVENT_NACK:
                    mpc5200mbus_error(bus, I2C_NO_DEVICE);
                    next_state(bus, STATE_IDLE);
                    mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    break;

                case EVENT_ARB_LOST:
                    mpc5200mbus_error(bus, I2C_ARBITRATION_LOST);
                    next_state(bus, STATE_IDLE);
                    mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    break;

                default:
                    mpc5200mbus_machine_error(bus, event);
                    break;
            }
            break;

        case STATE_SENDING:
            switch (event)
            {
                case EVENT_ACK:
                    if (bus->byte == bus->cmsg->len)
                    {
                        next_state(bus, STATE_IDLE);
                        mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    }
                    else
                    {
                        mpc5200mbus_send(bus, bus->cmsg->buf[bus->byte++]);
                        next_state(bus, STATE_SENDING);
                    }
                    break;

                case EVENT_NACK:
                    if (bus->byte == 0)
                    {
                        mpc5200mbus_error(bus, I2C_NO_DEVICE);
                    }
                    else
                    {
                        mpc5200mbus_error(bus, I2C_NO_ACKNOWLEDGE);
                    }
                    next_state(bus, STATE_IDLE);
                    mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    break;

                case EVENT_ARB_LOST:
                    mpc5200mbus_error(bus, I2C_ARBITRATION_LOST);
                    next_state(bus, STATE_IDLE);
                    mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    break;

                default:
                    mpc5200mbus_machine_error(bus, event);
                    break;

            }
            break;

        case STATE_RECEIVING:
            switch (event)
            {
                case EVENT_DATA_RECV:
                    if (bus->cmsg->len - bus->byte <= 2)
                    {
                        mpc5200mbus_send_nack(bus);
                        if (bus->cmsg->len - bus->byte <= 1)
                        {
                            if (bus->cmsg - bus->msg + 1 == bus->nmsg)
                                mpc5200mbus_stop(bus);
                            else
                                mpc5200mbus_rstart(bus);
                        }
                    }
                    else
                    {
                        mpc5200mbus_send_ack(bus);
                    }
                    bus->cmsg->buf[bus->byte++] = mpc5200.i2c_regs[bus->bus_idx].mdr;
                    if (bus->cmsg->len == bus->byte)
                    {
                        next_state(bus,STATE_IDLE);
                        mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    }
                    else
                    {
                        next_state(bus,STATE_RECEIVING);
                    }
                    break;

                case EVENT_ARB_LOST:
                    mpc5200mbus_error(bus, I2C_ARBITRATION_LOST);
                    next_state(bus, STATE_IDLE);
                    mpc5200mbus_machine(bus, EVENT_NEXTMSG);
                    break;

                default:
                    mpc5200mbus_machine_error(bus, event);
                    break;
            }
            break;
    }
}

/* mpc5200mbus_interrupt_handler --
 *     MBUS module interrupt handler routine
 *
 * PARAMETERS:
 *     handle: pointer to mbus structure
 *
 * RETURNS:
 *     none
 */
void mpc5200mbus_interrupt_handler(rtems_irq_hdl_param handle)
{
    i2c_event event;
    mpc5200mbus *bus = handle;

    event = mpc5200mbus_get_event(bus);
    /*
     * clear interrupt bit
     */
    mpc5200.i2c_regs[bus->bus_idx].msr &= ~MPC5200_I2C_MSR_MIF;

    mpc5200mbus_machine(bus, event);
}

/*
 * mpc5200_mbus_irq_enable
 *    enable irq for mbus
 */
void mpc5200mbus_irq_enable(const rtems_irq_connect_data* ptr)
{
  int minor = ((mpc5200mbus*)(ptr->handle))->bus_idx;

  mpc5200.i2c_regs[minor].mcr |= MPC5200_I2C_MCR_MIEN;
}

/*
 * mpc5200_mbus_irq_disable
 *    enable irq for mbus
 */
void mpc5200mbus_irq_disable(const rtems_irq_connect_data* ptr)
{
  int minor = ((mpc5200mbus*)(ptr->handle))->bus_idx;

  mpc5200.i2c_regs[minor].mcr &= ~MPC5200_I2C_MCR_MIEN;
}

/*
 * mpc5200_mbus_isOn
 *    check, whether irq is enabled
 */
int mpc5200mbus_irq_isOn(const rtems_irq_connect_data* ptr)
{
  int minor = ((mpc5200mbus*)(ptr->handle))->bus_idx;

  return (0 != (mpc5200.i2c_regs[minor].mcr & MPC5200_I2C_MCR_MIEN));
}

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
void
mpc5200mbus_poll(mpc5200mbus *bus)
{
    i2c_event event;
    event = mpc5200mbus_get_event(bus);
    if (event != EVENT_NONE)
        mpc5200mbus_machine(bus, event);
}

/* mpc5200mbus_select_clock_divider --
 *     Select divider for system clock which is used for I2C bus clock
 *     generation. Not each divider can be selected for I2C bus; this
 *     function select nearest larger or equal divider.
 *
 * PARAMETERS:
 *     i2c_bus - pointer to the bus descriptor structure
 *     divider - system frequency divider for I2C serial clock.
 * RETURNS:
 *     RTEMS_SUCCESSFUL, if operation performed successfully, or
 *     RTEMS error code when failed.
 */
rtems_status_code
mpc5200mbus_select_clock_divider(mpc5200mbus *bus, int divider)
{
    int i;
    int mbc;
    struct {
        int divider;
        int mbc;
    } dividers[] ={
        { 20,   0x20 }, { 22,   0x21 }, { 24,   0x22 }, { 26,   0x23 },
        { 28,   0x00 }, { 30,   0x01 }, { 32,   0x25 }, { 34,   0x02 },
        { 36,   0x26 }, { 40,   0x03 }, { 44,   0x04 }, { 48,   0x05 },
        { 56,   0x06 }, { 64,   0x2a }, { 68,   0x07 }, { 72,   0x2B },
        { 80,   0x08 }, { 88,   0x09 }, { 96,   0x2D }, { 104,  0x0A },
        { 112,  0x2E }, { 128,  0x0B }, { 144,  0x0C }, { 160,  0x0D },
        { 192,  0x0E }, { 224,  0x32 }, { 240,  0x0F }, { 256,  0x33 },
        { 288,  0x10 }, { 320,  0x11 }, { 384,  0x12 }, { 448,  0x36 },
        { 480,  0x13 }, { 512,  0x37 }, { 576,  0x14 }, { 640,  0x15 },
        { 768,  0x16 }, { 896,  0x3A }, { 960,  0x17 }, { 1024, 0x3B },
        { 1152, 0x18 }, { 1280, 0x19 }, { 1536, 0x1A }, { 1792, 0x3E },
        { 1920, 0x1B }, { 2048, 0x3F }, { 2304, 0x1C }, { 2560, 0x1D },
        { 3072, 0x1E }, { 3840, 0x1F }
    };

    if (bus == NULL)
        return RTEMS_INVALID_ADDRESS;

    for (i = 0, mbc = -1; i < sizeof(dividers)/sizeof(dividers[0]); i++)
    {
        mbc = dividers[i].mbc;
        if (dividers[i].divider >= divider)
        {
            break;
        }
    }
    mpc5200.i2c_regs[bus->bus_idx].mfdr = mbc;
    return RTEMS_SUCCESSFUL;
}

/* mpc5200mbus_initialize --
 *     Initialize MPC5200 MBUS I2C bus controller.
 *
 * PARAMETERS:
 *     i2c_bus - pointer to the bus descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL, or RTEMS error code when initialization failed.
 */
rtems_status_code
mpc5200mbus_initialize(mpc5200mbus *i2c_bus)
{
    rtems_interrupt_level level;
    rtems_status_code sc;
    rtems_irq_connect_data mbusIrqData;

    if (i2c_bus == NULL)
        return RTEMS_INVALID_ADDRESS;

    if (i2c_bus->state != STATE_UNINITIALIZED) /* Check if already initialized */
        return RTEMS_RESOURCE_IN_USE;

    i2c_bus->state = STATE_IDLE;
    i2c_bus->msg = NULL;
    i2c_bus->cmsg = NULL;
    i2c_bus->nmsg = 0;
    i2c_bus->byte = 0;

    /*
     * install interrupt handler
     */
    mbusIrqData.on     = mpc5200mbus_irq_enable;
    mbusIrqData.off    = mpc5200mbus_irq_disable;
    mbusIrqData.isOn   = mpc5200mbus_irq_isOn;
    mbusIrqData.handle = (rtems_irq_hdl_param)i2c_bus;
    mbusIrqData.hdl    = mpc5200mbus_interrupt_handler;
    switch(i2c_bus->bus_idx) {
    case 0:
      mbusIrqData.name   = BSP_SIU_IRQ_I2C1;
      break;
    case 1:
      mbusIrqData.name   = BSP_SIU_IRQ_I2C2;
      break;
    }
    if (!BSP_install_rtems_irq_handler (&mbusIrqData)) {
      sc = RTEMS_UNSATISFIED;
      return sc;
    }

    rtems_interrupt_disable(level);

    mpc5200.i2c_regs[i2c_bus->bus_idx].mcr &= ~MPC5200_I2C_MCR_MEN;
    mpc5200.i2c_regs[i2c_bus->bus_idx].msr = 0;
    mpc5200.i2c_regs[i2c_bus->bus_idx].mdr = 0x1F; /* Maximum possible divider is 3840 */
    mpc5200.i2c_regs[i2c_bus->bus_idx].mcr |= MPC5200_I2C_MCR_MEN;

    rtems_interrupt_enable(level);

    return RTEMS_SUCCESSFUL;
}

/* mpc5200mbus_i2c_transfer --
 *     Initiate multiple-messages transfer over I2C bus via ColdFire MBUS
 *     controller.
 *
 * PARAMETERS:
 *     bus - pointer to MBUS controller descriptor
 *     nmsg - number of messages
 *     msg - pointer to messages array
 *     done - function which is called when transfer is finished
 *     done_arg_ptr - arbitrary argument pointer passed to done funciton
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if transfer initiated successfully, or error
 *     code when failed.
 */
rtems_status_code
mpc5200mbus_i2c_transfer(mpc5200mbus *bus, int nmsg, i2c_message *msg,
                     i2c_transfer_done done, void *done_arg_ptr)
{
    if (bus->state == STATE_UNINITIALIZED)
        return RTEMS_NOT_CONFIGURED;

    bus->done = done;
    bus->done_arg_ptr = done_arg_ptr;
    bus->cmsg = bus->msg = msg;
    bus->nmsg = nmsg;
    bus->byte = 0;
    bus->state = STATE_IDLE;
    mpc5200mbus_machine(bus, EVENT_TRANSFER);
    return RTEMS_SUCCESSFUL;
}


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
mpc5200mbus_i2c_done(mpc5200mbus *i2c_bus)
{
    rtems_status_code sc = RTEMS_SUCCESSFUL;

    if (i2c_bus->state == STATE_UNINITIALIZED)
        return RTEMS_NOT_CONFIGURED;

    mpc5200.i2c_regs[i2c_bus->bus_idx].mcr = 0;

    return sc;
}
