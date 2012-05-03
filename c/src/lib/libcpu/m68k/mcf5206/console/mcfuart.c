/*
 * Generic UART Serial driver for Motorola Coldfire processors
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russian Fed.
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems.h>
#include <termios.h>
#include <rtems/libio.h>
#include "mcf5206/mcfuart.h"

/*
 * int_driven_uart -- mapping between interrupt vector number and
 * UART descriptor structures
 */
static struct {
    mcfuart *uart;
    int      vec;
} int_driven_uart[2];

/* Forward function declarations */
static rtems_isr
mcfuart_interrupt_handler(rtems_vector_number vec);

/*
 * mcfuart_init --
 *     This function verifies the input parameters and perform initialization
 *     of the Motorola Coldfire on-chip UART descriptor structure.
 *
 * PARAMETERS:
 *     uart - pointer to the UART channel descriptor structure
 *     tty - pointer to termios structure
 *     int_driven - interrupt-driven (1) or polled (0) I/O mode
 *     chn - channel number (0/1)
 *     rx_buf - pointer to receive buffer
 *     rx_buf_len - receive buffer length
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if all parameters are valid, or error code
 */
rtems_status_code
mcfuart_init(mcfuart *uart, void *tty, uint8_t   intvec,
             uint32_t   chn)
{
    if (uart == NULL)
        return RTEMS_INVALID_ADDRESS;

    if ((chn <= 0) || (chn > MCF5206E_UART_CHANNELS))
        return RTEMS_INVALID_NUMBER;

    uart->chn = chn;
    uart->intvec = intvec;
    uart->tty = tty;

    return RTEMS_SUCCESSFUL;
}

/* mcfuart_set_baudrate --
 *     Program the UART timer to specified baudrate
 *
 * PARAMETERS:
 *     uart - pointer to UART descriptor structure
 *     baud - termios baud rate (B50, B9600, etc...)
 *
 * RETURNS:
 *     none
 */
static void
mcfuart_set_baudrate(mcfuart *uart, speed_t baud)
{
    uint32_t   div;
    uint32_t   rate;
    switch (baud)
    {
        case B50:     rate = 50; break;
        case B75:     rate = 75; break;
        case B110:    rate = 110; break;
        case B134:    rate = 134; break;
        case B150:    rate = 150; break;
        case B200:    rate = 200; break;
        case B300:    rate = 300; break;
        case B600:    rate = 600; break;
        case B1200:   rate = 1200; break;
        case B2400:   rate = 2400; break;
        case B4800:   rate = 4800; break;
        case B9600:   rate = 9600; break;
        case B19200:  rate = 19200; break;
        case B38400:  rate = 38400; break;
        case B57600:  rate = 57600; break;
#ifdef B115200
        case B115200: rate = 115200; break;
#endif
#ifdef B230400
        case B230400: rate = 230400; break;
#endif
        default:      rate = 9600; break;
    }

    div = SYSTEM_CLOCK_FREQUENCY / (rate * 32);

    *MCF5206E_UBG1(MBAR,uart->chn) = (uint8_t)((div >> 8) & 0xff);
    *MCF5206E_UBG2(MBAR,uart->chn) = (uint8_t)(div & 0xff);
}

/*
 * mcfuart_reset --
 *     This function perform the hardware initialization of Motorola
 *     Coldfire processor on-chip UART controller using parameters
 *     filled by the mcfuart_init function.
 *
 * PARAMETERS:
 *     uart - pointer to UART channel descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if channel is initialized successfully, error
 *     code in other case
 *
 * ALGORITHM:
 *     This function in general follows to algorith described in MCF5206e
 *     User's Manual, 12.5 UART Module Initialization Sequence
 */
rtems_status_code
mcfuart_reset(mcfuart *uart)
{
    register uint32_t   chn;
    rtems_status_code rc;

    if (uart == NULL)
        return RTEMS_INVALID_ADDRESS;

    chn = uart->chn;

    /* Reset the receiver and transmitter */
    *MCF5206E_UCR(MBAR,chn) = MCF5206E_UCR_MISC_RESET_RX;
    *MCF5206E_UCR(MBAR,chn) = MCF5206E_UCR_MISC_RESET_TX;

    /*
     * Program the vector number for a UART module interrupt, or
     * disable UART interrupts if polled I/O. Enable the desired
     * interrupt sources.
     */
    if (uart->intvec != 0)
    {
        int_driven_uart[chn - 1].uart = uart;
        int_driven_uart[chn - 1].vec = uart->intvec;
        rc = rtems_interrupt_catch(mcfuart_interrupt_handler, uart->intvec,
                                   &uart->old_handler);
        if (rc != RTEMS_SUCCESSFUL)
            return rc;
        *MCF5206E_UIVR(MBAR,chn) = uart->intvec;
        *MCF5206E_UIMR(MBAR,chn) = MCF5206E_UIMR_FFULL;
        *MCF5206E_UACR(MBAR,chn) = MCF5206E_UACR_IEC;
        *MCF5206E_IMR(MBAR) &= ~MCF5206E_INTR_BIT(uart->chn == 1 ?
                                                  MCF5206E_INTR_UART_1 :
                                                  MCF5206E_INTR_UART_2);
    }
    else
    {
        *MCF5206E_UIMR(MBAR,chn) = 0;
    }

    /* Select the receiver and transmitter clock. */
    mcfuart_set_baudrate(uart, B19200); /* dBUG defaults (unfortunately,
                                           it is differ to termios default */
    *MCF5206E_UCSR(MBAR,chn) =
        MCF5206E_UCSR_RCS_TIMER | MCF5206E_UCSR_TCS_TIMER;

    /* Mode Registers 1,2 - set termios defaults (8N1) */
    *MCF5206E_UCR(MBAR,chn) = MCF5206E_UCR_MISC_RESET_MR;
    *MCF5206E_UMR(MBAR,chn) =
/*      MCF5206E_UMR1_RXRTS | */
        MCF5206E_UMR1_PM_NO_PARITY |
        MCF5206E_UMR1_BC_8;
    *MCF5206E_UMR(MBAR,chn) =
        MCF5206E_UMR2_CM_NORMAL |
/*      MCF5206E_UMR2_TXCTS | */
        MCF5206E_UMR2_SB_1;

    /* Enable Receiver and Transmitter */
    *MCF5206E_UCR(MBAR,chn) = MCF5206E_UCR_MISC_RESET_ERR;
    *MCF5206E_UCR(MBAR,chn) = MCF5206E_UCR_TC_ENABLE;
    *MCF5206E_UCR(MBAR,chn) = MCF5206E_UCR_RC_ENABLE;

    return RTEMS_SUCCESSFUL;
}

/*
 * mcfuart_disable --
 *     This function disable the operations on Motorola Coldfire UART
 *     controller
 *
 * PARAMETERS:
 *     uart - pointer to UART channel descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if UART closed successfuly, or error code in
 *     other case
 */
rtems_status_code
mcfuart_disable(mcfuart *uart)
{
    rtems_status_code rc;
    *MCF5206E_UCR(MBAR,uart->chn) =
        MCF5206E_UCR_TC_DISABLE |
        MCF5206E_UCR_RC_DISABLE;
    if (uart->intvec != 0)
    {
        *MCF5206E_IMR(MBAR) |=  MCF5206E_INTR_BIT(uart->chn == 1 ?
                                                  MCF5206E_INTR_UART_1 :
                                                  MCF5206E_INTR_UART_2);
        rc = rtems_interrupt_catch(uart->old_handler, uart->intvec, NULL);
        int_driven_uart[uart->chn - 1].uart = NULL;
        int_driven_uart[uart->chn - 1].vec = 0;
        if (rc != RTEMS_SUCCESSFUL)
            return rc;
    }
    return RTEMS_SUCCESSFUL;
}

/*
 * mcfuart_set_attributes --
 *     This function parse the termios attributes structure and perform
 *     the appropriate settings in hardware.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *     t - pointer to termios parameters
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
int
mcfuart_set_attributes(mcfuart *uart, const struct termios *t)
{
    int level;
    speed_t baud;
    uint8_t   umr1, umr2;

    baud = cfgetospeed(t);
    umr1 = 0;
    umr2 = MCF5206E_UMR2_CM_NORMAL;

    /* Set flow control */
    if ((t->c_cflag & CRTSCTS) != 0)
    {
        umr1 |= MCF5206E_UMR1_RXRTS;
        umr2 |= MCF5206E_UMR2_TXCTS;
    }

    /* Set character size */
    switch (t->c_cflag & CSIZE)
    {
        case CS5: umr1 |= MCF5206E_UMR1_BC_5; break;
        case CS6: umr1 |= MCF5206E_UMR1_BC_6; break;
        case CS7: umr1 |= MCF5206E_UMR1_BC_7; break;
        case CS8: umr1 |= MCF5206E_UMR1_BC_8; break;
    }

    /* Set number of stop bits */
    if ((t->c_cflag & CSTOPB) != 0)
    {
        if ((t->c_cflag & CSIZE) == CS5)
        {
            umr2 |= MCF5206E_UMR2_SB5_2;
        }
        else
        {
            umr2 |= MCF5206E_UMR2_SB_2;
        }
    }
    else
    {
        if ((t->c_cflag & CSIZE) == CS5)
        {
            umr2 |= MCF5206E_UMR2_SB5_1;
        }
        else
        {
            umr2 |= MCF5206E_UMR2_SB_1;
        }
    }

    /* Set parity mode */
    if ((t->c_cflag & PARENB) != 0)
    {
        if ((t->c_cflag & PARODD) != 0)
        {
            umr1 |= MCF5206E_UMR1_PM_ODD;
        }
        else
        {
            umr1 |= MCF5206E_UMR1_PM_EVEN;
        }
    }
    else
    {
        umr1 |= MCF5206E_UMR1_PM_NO_PARITY;
    }

    rtems_interrupt_disable(level);
    *MCF5206E_UCR(MBAR,uart->chn) =
        MCF5206E_UCR_TC_DISABLE | MCF5206E_UCR_RC_DISABLE;
    mcfuart_set_baudrate(uart, baud);
    *MCF5206E_UCR(MBAR,uart->chn) = MCF5206E_UCR_MISC_RESET_MR;
    *MCF5206E_UMR(MBAR,uart->chn) = umr1;
    *MCF5206E_UMR(MBAR,uart->chn) = umr2;
    if ((t->c_cflag & CREAD) != 0)
    {
        *MCF5206E_UCR(MBAR,uart->chn) =
            MCF5206E_UCR_TC_ENABLE | MCF5206E_UCR_RC_ENABLE;
    }
    else
    {
        *MCF5206E_UCR(MBAR,uart->chn) = MCF5206E_UCR_TC_ENABLE;
    }
    rtems_interrupt_enable(level);

    return RTEMS_SUCCESSFUL;
}

/*
 * mcfuart_poll_read --
 *     This function tried to read character from MCF UART and perform
 *     error handling. When parity or framing error occured, return
 *     value dependent on termios input mode flags:
 *         - received character, if IGNPAR == 1
 *         - 0, if IGNPAR == 0 and PARMRK == 0
 *         - 0xff and 0x00 on next poll_read invocation, if IGNPAR == 0 and
 *           PARMRK == 1
 *
 * PARAMETERS:
 *     uart - pointer to UART descriptor structure
 *
 * RETURNS:
 *     code of received character or -1 if no characters received.
 */
int
mcfuart_poll_read(mcfuart *uart)
{
    uint8_t   usr;
    int ch;
    if (uart->parerr_mark_flag == true)
    {
        uart->parerr_mark_flag = false;
        return 0;
    }
    usr = *MCF5206E_USR(MBAR,uart->chn);
    if ((usr & MCF5206E_USR_RXRDY) != 0)
    {
        if (((usr & (MCF5206E_USR_FE | MCF5206E_USR_PE)) != 0) &&
            !(uart->c_iflag & IGNPAR))
        {
            ch = *MCF5206E_URB(MBAR,uart->chn); /* Clear error bits */
            if (uart->c_iflag & PARMRK)
            {
                uart->parerr_mark_flag = true;
                ch = 0xff;
            }
            else
            {
                ch = 0;
            }
        }
        else
        {
            ch = *MCF5206E_URB(MBAR,uart->chn);
        }
    }
    else
        ch = -1;
    return ch;
}

/*
 * mcfuart_poll_write --
 *     This function transmit buffer byte-by-byte in polling mode.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *     buf - pointer to transmit buffer
 *     len - transmit buffer length
 *
 * RETURNS:
 *     0
 */
ssize_t
mcfuart_poll_write(mcfuart *uart, const char *buf, size_t len)
{
  size_t retval = len;

    while (len--)
    {
        while ((*MCF5206E_USR(MBAR, uart->chn) & MCF5206E_USR_TXRDY) == 0);
        *MCF5206E_UTB(MBAR, uart->chn) = *buf++;
    }
    return retval;
}

/* mcfuart_interrupt_handler --
 *     UART interrupt handler routine
 *
 * PARAMETERS:
 *     vec - interrupt vector number
 *
 * RETURNS:
 *     none
 */
static rtems_isr
mcfuart_interrupt_handler(rtems_vector_number vec)
{
    mcfuart *uart;
    register uint8_t   usr;
    register uint8_t   uisr;
    register int chn;
    register int bp = 0;

    /* Find UART descriptor from vector number */
    if (int_driven_uart[0].vec == vec)
        uart = int_driven_uart[0].uart;
    else if (int_driven_uart[1].vec == vec)
        uart = int_driven_uart[1].uart;
    else
        return;

    chn = uart->chn;

    uisr = *MCF5206E_UISR(MBAR, chn);
    if (uisr & MCF5206E_UISR_DB)
    {
        *MCF5206E_UCR(MBAR, chn) = MCF5206E_UCR_MISC_RESET_BRK;
    }

    /* Receiving */
    while (1)
    {
        char buf[32];
        usr = *MCF5206E_USR(MBAR,chn);
        if ((bp < sizeof(buf) - 1) && ((usr & MCF5206E_USR_RXRDY) != 0))
        {
            /* Receive character and handle frame/parity errors */
            if (((usr & (MCF5206E_USR_FE | MCF5206E_USR_PE)) != 0) &&
                !(uart->c_iflag & IGNPAR))
            {
                if (uart->c_iflag & PARMRK)
                {
                    buf[bp++] = 0xff;
                    buf[bp++] = 0x00;
                }
                else
                {
                    buf[bp++] = 0x00;
                }
            }
            else
            {
                buf[bp++] = *MCF5206E_URB(MBAR, chn);
            }

            /* Reset error condition if any errors has been detected */
            if (usr & (MCF5206E_USR_RB | MCF5206E_USR_FE |
                       MCF5206E_USR_PE | MCF5206E_USR_OE))
            {
                *MCF5206E_UCR(MBAR, chn) = MCF5206E_UCR_MISC_RESET_ERR;
            }
        }
        else
        {
            if (bp != 0)
                rtems_termios_enqueue_raw_characters(uart->tty, buf, bp);
            break;
        }
    }

    /* Transmitting */
    while (1)
    {
        if ((*MCF5206E_USR(MBAR, chn) & MCF5206E_USR_TXRDY) == 0)
            break;
        if (uart->tx_buf != NULL)
        {
            if (uart->tx_ptr >= uart->tx_buf_len)
            {
                register int dequeue = uart->tx_buf_len;
                *MCF5206E_UIMR(MBAR, uart->chn) = MCF5206E_UIMR_FFULL;
                uart->tx_buf = NULL;
                uart->tx_ptr = uart->tx_buf_len = 0;
                rtems_termios_dequeue_characters(uart->tty, dequeue);
            }
            else
            {
                *MCF5206E_UTB(MBAR, chn) = uart->tx_buf[uart->tx_ptr++];
            }
        }
        else
            break;
    }
}

/* mcfuart_interrupt_write --
 *     This function initiate transmitting of the buffer in interrupt mode.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *     buf - pointer to transmit buffer
 *     len - transmit buffer length
 *
 * RETURNS:
 *     0
 */
ssize_t
mcfuart_interrupt_write(mcfuart *uart, const char *buf, size_t len)
{
    int level;
    rtems_interrupt_disable(level);
    uart->tx_buf = buf;
    uart->tx_buf_len = len;
    uart->tx_ptr = 0;
    *MCF5206E_UIMR(MBAR, uart->chn) =
            MCF5206E_UIMR_FFULL | MCF5206E_UIMR_TXRDY;
    while (((*MCF5206E_USR(MBAR,uart->chn) & MCF5206E_USR_TXRDY) != 0) &&
           (uart->tx_ptr < uart->tx_buf_len))
    {
        *MCF5206E_UTB(MBAR,uart->chn) = uart->tx_buf[uart->tx_ptr++];
    }
    rtems_interrupt_enable(level);
    return 0;
}

/* mcfuart_stop_remote_tx --
 *     This function stop data flow from remote device.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
int
mcfuart_stop_remote_tx(mcfuart *uart)
{
    *MCF5206E_UOP0(MBAR, uart->chn) = 1;
    return RTEMS_SUCCESSFUL;
}

/* mcfuart_start_remote_tx --
 *     This function resume data flow from remote device.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
int
mcfuart_start_remote_tx(mcfuart *uart)
{
    *MCF5206E_UOP1(MBAR, uart->chn) = 1;
    return RTEMS_SUCCESSFUL;
}
