/*
 *  MCF5206e MBUS module (I2C bus) driver
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include "mcf5206/mcfmbus.h"
#include "mcf5206/mcf5206e.h"
#include "i2c.h"

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

static mcfmbus *mbus;

/*** Auxillary primitives ***/

/* Change state of finite state machine */
#define next_state(bus,new_state) \
    do {                             \
        (bus)->state = (new_state);  \
    } while (0)

/* Initiate start condition on the I2C bus */
#define mcfmbus_start(bus) \
    do {                                                    \
        *MCF5206E_MBCR((bus)->base) |= MCF5206E_MBCR_MSTA;  \
    } while (0)

/* Initiate stop condition on the I2C bus */
#define mcfmbus_stop(bus) \
    do {                                                    \
        *MCF5206E_MBCR((bus)->base) &= ~MCF5206E_MBCR_MSTA; \
    } while (0)

/* Initiate repeat start condition on the I2C bus */
#define mcfmbus_rstart(bus) \
    do {                                                    \
        *MCF5206E_MBCR((bus)->base) |= MCF5206E_MBCR_RSTA;  \
    } while (0)

/* Send byte to the bus */
#define mcfmbus_send(bus,byte) \
    do {                                      \
        *MCF5206E_MBDR((bus)->base) = (byte); \
    } while (0)

/* Set transmit mode */
#define mcfmbus_tx_mode(bus) \
    do {                                                     \
        *MCF5206E_MBCR((bus)->base) |= MCF5206E_MBCR_MTX;    \
    } while (0)

/* Set receive mode */
#define mcfmbus_rx_mode(bus) \
    do {                                                     \
        *MCF5206E_MBCR((bus)->base) &= ~MCF5206E_MBCR_MTX;   \
        (void)*MCF5206E_MBDR((bus)->base);                   \
    } while (0)


/* Transmit acknowledge when byte received */
#define mcfmbus_send_ack(bus) \
    do {                                                     \
        *MCF5206E_MBCR((bus)->base) &= ~MCF5206E_MBCR_TXAK;  \
    } while (0)

/* DO NOT transmit acknowledge when byte received */
#define mcfmbus_send_nack(bus) \
    do {                                                     \
        *MCF5206E_MBCR((bus)->base) |= MCF5206E_MBCR_TXAK;   \
    } while (0)

#define mcfmbus_error(bus,err_status) \
    do {                                                       \
        do {                                                   \
           (bus)->cmsg->status = (err_status);                 \
           (bus)->cmsg++;                                      \
        } while (((bus)->cmsg - (bus)->msg < (bus)->nmsg) &&   \
                 ((bus)->cmsg->flags & I2C_MSG_ERRSKIP));      \
        bus->cmsg--;                                           \
    } while (0)

/* mcfmbus_get_event --
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
mcfmbus_get_event(mcfmbus *bus)
{
    i2c_event event;
    uint8_t   status, control;
    rtems_interrupt_level level;
    rtems_interrupt_disable(level);
    status = *MCF5206E_MBSR(bus->base);
    control = *MCF5206E_MBCR(bus->base);
    if (status & MCF5206E_MBSR_MIF) /* Interrupt occured */
    {
        if (status & MCF5206E_MBSR_MAAS)
        {
            event = EVENT_SLAVE;
            *MCF5206E_MBCR(bus->base) = control; /* To clear Addressed As Slave
                                                    condition */
        }
        else if (status & MCF5206E_MBSR_MAL) /* Arbitration lost */
        {
            *MCF5206E_MBSR(bus->base) = status & ~MCF5206E_MBSR_MAL;
            event = EVENT_ARB_LOST;
        }
        else if (control & MCF5206E_MBCR_MTX) /* Trasmit mode */
        {
            if (status & MCF5206E_MBSR_RXAK)
                event = EVENT_NACK;
            else
                event = EVENT_ACK;
        }
        else /* Received */
        {
            event = EVENT_DATA_RECV;
        }

        /* Clear interrupt condition */
        *MCF5206E_MBSR(bus->base) &= ~MCF5206E_MBSR_MIF;
    }
    else
    {
        event = EVENT_NONE;
    }
    rtems_interrupt_enable(level);
    return event;
}

static void
mcfmbus_machine_error(mcfmbus *bus, i2c_event event)
{
    return;
}

/* mcfmbus_machine --
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
mcfmbus_machine(mcfmbus *bus, i2c_event event)
{
    uint8_t   b;
    switch (bus->state)
    {
        case STATE_IDLE:
            switch (event)
            {
                case EVENT_NEXTMSG:  /* Start new message processing */
                    bus->cmsg++;
                    /* FALLTHRU */

                case EVENT_TRANSFER: /* Initiate new transfer */
                    if (bus->cmsg - bus->msg >= bus->nmsg)
                    {
                        mcfmbus_stop(bus);
                        next_state(bus, STATE_IDLE);
                        bus->msg = bus->cmsg = NULL;
                        bus->nmsg = bus->byte = 0;
                        bus->done(bus->done_arg_ptr);
                        break;
                    }

                    /* Initiate START or REPEATED START condition on the bus */
                    if (event == EVENT_TRANSFER)
                    {
                        mcfmbus_start(bus);
                    }
                    else /* (event == EVENT_NEXTMSG) */
                    {
                        mcfmbus_rstart(bus);
                    }

                    bus->byte = 0;
                    mcfmbus_tx_mode(bus);

                    /* Initiate slave address sending */
                    if (bus->cmsg->flags & I2C_MSG_ADDR_10)
                    {
                        i2c_address a = bus->cmsg->addr;
                        b = 0xf0 | (((a >> 8) & 0x03) << 1);
                        if (bus->cmsg->flags & I2C_MSG_WR)
                        {
                            mcfmbus_send(bus, b);
                            next_state(bus, STATE_ADDR_1_W);
                        }
                        else
                        {
                            mcfmbus_send(bus, b | 1);
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

                        mcfmbus_send(bus, b);
                    }
                    break;

                default:
                    mcfmbus_machine_error(bus, event);
                    break;
            }
            break;

        case STATE_ADDR_7:
            switch (event)
            {
                case EVENT_ACK:
                    mcfmbus_rx_mode(bus);
                    if (bus->cmsg->len <= 1)
                        mcfmbus_send_nack(bus);
                    else
                        mcfmbus_send_ack(bus);
                    next_state(bus, STATE_RECEIVING);
                    break;

                case EVENT_NACK:
                    mcfmbus_error(bus, I2C_NO_DEVICE);
                    next_state(bus, STATE_IDLE);
                    mcfmbus_machine(bus, EVENT_NEXTMSG);
                    break;

                case EVENT_ARB_LOST:
                    mcfmbus_error(bus, I2C_ARBITRATION_LOST);
                    next_state(bus, STATE_IDLE);
                    mcfmbus_machine(bus, EVENT_NEXTMSG);
                    break;

                default:
                    mcfmbus_machine_error(bus, event);
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
                    mcfmbus_send(bus, b);
                    if (bus->state == STATE_ADDR_1_W)
                    {
                        next_state(bus, STATE_SENDING);
                    }
                    else
                    {
                        i2c_address a;
                        mcfmbus_rstart(bus);
                        mcfmbus_tx_mode(bus);
                        a = bus->cmsg->addr;
                        b = 0xf0 | (((a >> 8) & 0x03) << 1) | 1;
                        mcfmbus_send(bus, b);
                        next_state(bus, STATE_ADDR_7);
                    }
                    break;
                }

                case EVENT_NACK:
                    mcfmbus_error(bus, I2C_NO_DEVICE);
                    next_state(bus, STATE_IDLE);
                    mcfmbus_machine(bus, EVENT_NEXTMSG);
                    break;

                case EVENT_ARB_LOST:
                    mcfmbus_error(bus, I2C_ARBITRATION_LOST);
                    next_state(bus, STATE_IDLE);
                    mcfmbus_machine(bus, EVENT_NEXTMSG);
                    break;

                default:
                    mcfmbus_machine_error(bus, event);
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
                        mcfmbus_machine(bus, EVENT_NEXTMSG);
                    }
                    else
                    {
                        mcfmbus_send(bus, bus->cmsg->buf[bus->byte++]);
                        next_state(bus, STATE_SENDING);
                    }
                    break;

                case EVENT_NACK:
                    if (bus->byte == 0)
                    {
                        mcfmbus_error(bus, I2C_NO_DEVICE);
                    }
                    else
                    {
                        mcfmbus_error(bus, I2C_NO_ACKNOWLEDGE);
                    }
                    next_state(bus, STATE_IDLE);
                    mcfmbus_machine(bus, EVENT_NEXTMSG);
                    break;

                case EVENT_ARB_LOST:
                    mcfmbus_error(bus, I2C_ARBITRATION_LOST);
                    next_state(bus, STATE_IDLE);
                    mcfmbus_machine(bus, EVENT_NEXTMSG);
                    break;

                default:
                    mcfmbus_machine_error(bus, event);
                    break;

            }
            break;

        case STATE_RECEIVING:
            switch (event)
            {
                case EVENT_DATA_RECV:
                    if (bus->cmsg->len - bus->byte <= 2)
                    {
                        mcfmbus_send_nack(bus);
                        if (bus->cmsg->len - bus->byte <= 1)
                        {
                            if (bus->cmsg - bus->msg + 1 == bus->nmsg)
                                mcfmbus_stop(bus);
                            else
                                mcfmbus_rstart(bus);
                        }
                    }
                    else
                    {
                        mcfmbus_send_ack(bus);
                    }
                    bus->cmsg->buf[bus->byte++] = *MCF5206E_MBDR(bus->base);
                    if (bus->cmsg->len == bus->byte)
                    {
                        next_state(bus,STATE_IDLE);
                        mcfmbus_machine(bus, EVENT_NEXTMSG);
                    }
                    else
                    {
                        next_state(bus,STATE_RECEIVING);
                    }
                    break;

                case EVENT_ARB_LOST:
                    mcfmbus_error(bus, I2C_ARBITRATION_LOST);
                    next_state(bus, STATE_IDLE);
                    mcfmbus_machine(bus, EVENT_NEXTMSG);
                    break;

                default:
                    mcfmbus_machine_error(bus, event);
                    break;
            }
            break;
    }
}

/* mcfmbus_interrupt_handler --
 *     MBUS module interrupt handler routine
 *
 * PARAMETERS:
 *     vector - interrupt vector number (not used)
 *
 * RETURNS:
 *     none
 */
rtems_isr
mcfmbus_interrupt_handler(rtems_vector_number vector)
{
    i2c_event event;
    event = mcfmbus_get_event(mbus);
    mcfmbus_machine(mbus, event);
}

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
void
mcfmbus_poll(mcfmbus *bus)
{
    i2c_event event;
    event = mcfmbus_get_event(bus);
    if (event != EVENT_NONE)
        mcfmbus_machine(bus, event);
}

/* mcfmbus_select_clock_divider --
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
mcfmbus_select_clock_divider(mcfmbus *i2c_bus, int divider)
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

    if (i2c_bus == NULL)
        return RTEMS_INVALID_ADDRESS;

    for (i = 0, mbc = -1; i < sizeof(dividers)/sizeof(dividers[0]); i++)
    {
        mbc = dividers[i].mbc;
        if (dividers[i].divider >= divider)
        {
            break;
        }
    }
    *MCF5206E_MFDR(i2c_bus->base) = mbc;
    return RTEMS_SUCCESSFUL;
}

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
mcfmbus_initialize(mcfmbus *i2c_bus, uint32_t   base)
{
    rtems_interrupt_level level;
    rtems_status_code sc;

    if (mbus != NULL) /* Check if already initialized */
        return RTEMS_RESOURCE_IN_USE;

    if (i2c_bus == NULL)
        return RTEMS_INVALID_ADDRESS;


    i2c_bus->base = base;
    i2c_bus->state = STATE_IDLE;
    i2c_bus->msg = NULL;
    i2c_bus->cmsg = NULL;
    i2c_bus->nmsg = 0;
    i2c_bus->byte = 0;

    sc = rtems_interrupt_catch(
        mcfmbus_interrupt_handler,
        24 + ((*MCF5206E_ICR(base, MCF5206E_INTR_MBUS) & MCF5206E_ICR_IL) >>
                                   MCF5206E_ICR_IL_S),
        &i2c_bus->oldisr
    );
    if (sc != RTEMS_SUCCESSFUL)
        return sc;

    mbus = i2c_bus;
    rtems_interrupt_disable(level);
    *MCF5206E_IMR(base) &= ~MCF5206E_INTR_BIT(MCF5206E_INTR_MBUS);
    *MCF5206E_MBCR(base) = 0;
    *MCF5206E_MBSR(base) = 0;
    *MCF5206E_MBDR(base) = 0x1F; /* Maximum possible divider is 3840 */
    *MCF5206E_MBCR(base) = MCF5206E_MBCR_MEN | MCF5206E_MBCR_MIEN;
    rtems_interrupt_enable(level);

    return RTEMS_SUCCESSFUL;
}

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
                     i2c_transfer_done done, void *done_arg_ptr)
{
    if (bus != mbus)
        return RTEMS_NOT_CONFIGURED;

    bus->done = done;
    bus->done_arg_ptr = done_arg_ptr;
    bus->cmsg = bus->msg = msg;
    bus->nmsg = nmsg;
    bus->byte = 0;
    bus->state = STATE_IDLE;
    mcfmbus_machine(bus, EVENT_TRANSFER);
    return RTEMS_SUCCESSFUL;
}


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
mcfmbus_i2c_done(mcfmbus *i2c_bus)
{
    rtems_status_code sc;
    uint32_t   base;
    if (mbus == NULL)
        return RTEMS_NOT_CONFIGURED;

    if (mbus != i2c_bus)
        return RTEMS_INVALID_ADDRESS;

    base = i2c_bus->base;

    *MCF5206E_IMR(base) |= MCF5206E_INTR_BIT(MCF5206E_INTR_MBUS);
    *MCF5206E_MBCR(base) = 0;

    sc = rtems_interrupt_catch(
        i2c_bus->oldisr,
        24 + ((*MCF5206E_ICR(base, MCF5206E_INTR_MBUS) & MCF5206E_ICR_IL) >>
                                   MCF5206E_ICR_IL_S),
        NULL
    );
    return sc;
}
