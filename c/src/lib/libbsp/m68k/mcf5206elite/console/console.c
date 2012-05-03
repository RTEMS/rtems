/*
 * Console driver for Motorola MCF5206E UART modules
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <termios.h>
#include <bsp.h>
#include <rtems/libio.h>
#include "mcf5206/mcf5206e.h"
#include "mcf5206/mcfuart.h"

/* Descriptor structures for two on-chip UART channels */
static mcfuart uart[2];

/* Console operations mode:
 *     0 - raw (non-termios) polled input/output
 *     1 - termios-based polled input/output
 *     2 - termios-based interrupt-driven input/output
 */
int console_mode = 2;
#define CONSOLE_MODE_RAW  (0)
#define CONSOLE_MODE_POLL (1)
#define CONSOLE_MODE_INT  (2)

/* Wrapper functions for MCF UART generic driver */

/* console_poll_read --
 *     wrapper for poll read function
 *
 * PARAMETERS:
 *     minor - minor device number
 *
 * RETURNS:
 *     character code readed from UART, or -1 if there is no characters
 *     available
 */
static int
console_poll_read(int minor)
{
    return mcfuart_poll_read(&uart[minor]);
}

/* console_interrupt_write --
 *     wrapper for interrupt write function
 *
 * PARAMETERS:
 *     minor - minor device number
 *     buf - output buffer
 *     len - output buffer length
 *
 * RETURNS:
 *     result code
 */
static ssize_t
console_interrupt_write(int minor, const char *buf, size_t len)
{
    return mcfuart_interrupt_write(&uart[minor], buf, len);
}

/* console_poll_write --
 *     wrapper for polling mode write function
 *
 * PARAMETERS:
 *     minor - minor device number
 *     buf - output buffer
 *     len - output buffer length
 *
 * RETURNS:
 *     result code
 */
static ssize_t
console_poll_write(int minor, const char *buf, size_t len)
{
    return mcfuart_poll_write(&uart[minor], buf, len);
}

/* console_set_attributes --
 *     wrapper for hardware-dependent termios attributes setting
 *
 * PARAMETERS:
 *     minor - minor device number
 *     t - pointer to the termios structure
 *
 * RETURNS:
 *     result code
 */
static int
console_set_attributes(int minor, const struct termios *t)
{
    return mcfuart_set_attributes(&uart[minor], t);
}

/* console_stop_remote_tx --
 *     wrapper for stopping data flow from remote party.
 *
 * PARAMETERS:
 *     minor - minor device number
 *
 * RETURNS:
 *     result code
 */
static int
console_stop_remote_tx(int minor)
{
    if (minor < sizeof(uart)/sizeof(uart[0]))
        return mcfuart_stop_remote_tx(&uart[minor]);
    else
        return RTEMS_INVALID_NUMBER;
}

/* console_start_remote_tx --
 *     wrapper for resuming data flow from remote party.
 *
 * PARAMETERS:
 *     minor - minor device number
 *
 */
static int
console_start_remote_tx(int minor)
{
    if (minor < sizeof(uart)/sizeof(uart[0]))
        return mcfuart_start_remote_tx(&uart[minor]);
    else
        return RTEMS_INVALID_NUMBER;
}

/* console_first_open --
 *     wrapper for UART controller initialization functions
 *
 * PARAMETERS:
 *     major - major device number
 *     minor - minor device number
 *     arg - libio device open argument
 *
 * RETURNS:
 *     error code
 */
static int
console_first_open(int major, int minor, void *arg)
{
    rtems_libio_open_close_args_t *args = arg;
    rtems_status_code sc;
    uint8_t         intvec;

    switch (minor)
    {
        case 0: intvec = BSP_INTVEC_UART1; break;
        case 1: intvec = BSP_INTVEC_UART2; break;
        default:
            return RTEMS_INVALID_NUMBER;
    }

    if (console_mode != CONSOLE_MODE_INT)
    {
        intvec = 0;
    }

    sc = mcfuart_init(&uart[minor],          /* uart */
                      args->iop->data1,      /* tty */
                      intvec,                /* interrupt vector number */
                      minor+1);

    if (sc == RTEMS_SUCCESSFUL)
        sc = mcfuart_reset(&uart[minor]);

    return sc;
}

/* console_last_close --
 *     wrapper for UART controller close function
 *
 * PARAMETERS:
 *     major - major device number
 *     minor - minor device number
 *     arg - libio device close argument
 *
 * RETURNS:
 *     error code
 */
static int
console_last_close(int major, int minor, void *arg)
{
    return mcfuart_disable(&uart[minor]);
}

/* console_initialize --
 *     This routine initializes the console IO drivers and register devices
 *     in RTEMS I/O system.
 *
 * PARAMETERS:
 *     major - major console device number
 *     minor - minor console device number (not used)
 *     arg - device initialize argument
 *
 * RETURNS:
 *     RTEMS error code (RTEMS_SUCCESSFUL if device initialized successfuly)
 */
rtems_device_driver
console_initialize(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void *arg)
{
    rtems_status_code status;

    /*
     * Set up TERMIOS
     */
    if (console_mode != CONSOLE_MODE_RAW)
        rtems_termios_initialize ();

    /*
     * Register the devices
     */
    status = rtems_io_register_name ("/dev/console", major, 0);
    if (status != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (status);
    status = rtems_io_register_name ("/dev/aux", major, 1);
    if (status != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred (status);

    if (console_mode == CONSOLE_MODE_RAW)
    {
        rtems_status_code sc;
        sc = mcfuart_init(&uart[0],              /* uart */
                          NULL,                  /* tty */
                          0,                     /* interrupt vector number */
                          1);                    /* UART channel number */

        if (sc == RTEMS_SUCCESSFUL)
            sc = mcfuart_reset(&uart[0]);

        sc = mcfuart_init(&uart[1],              /* uart */
                          NULL,                  /* tty */
                          0,                     /* interrupt vector number */
                          2);                    /* UART channel number */

        if (sc == RTEMS_SUCCESSFUL)
            sc = mcfuart_reset(&uart[1]);
        return sc;
    }

    return RTEMS_SUCCESSFUL;
}

/* console_open --
 *     Open console device driver. Pass appropriate termios callback
 *     functions to termios library.
 *
 * PARAMETERS:
 *     major - major device number for console devices
 *     minor - minor device number for console
 *     arg - device opening argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver
console_open(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void *arg)
{
    static const rtems_termios_callbacks intr_callbacks = {
        console_first_open,        /* firstOpen */
        console_last_close,        /* lastClose */
        NULL,                      /* pollRead */
        console_interrupt_write,   /* write */
        console_set_attributes,    /* setAttributes */
        console_stop_remote_tx,    /* stopRemoteTx */
        console_start_remote_tx,   /* startRemoteTx */
        1                          /* outputUsesInterrupts */
    };
    static const rtems_termios_callbacks poll_callbacks = {
        console_first_open,        /* firstOpen */
        console_last_close,        /* lastClose */
        console_poll_read,         /* pollRead */
        console_poll_write,        /* write */
        console_set_attributes,    /* setAttributes */
        console_stop_remote_tx,    /* stopRemoteTx */
        console_start_remote_tx,   /* startRemoteTx */
        0                          /* outputUsesInterrupts */
    };

    switch (console_mode)
    {
        case CONSOLE_MODE_RAW:
            return RTEMS_SUCCESSFUL;

        case CONSOLE_MODE_INT:
            return rtems_termios_open(major, minor, arg, &intr_callbacks);

        case CONSOLE_MODE_POLL:
            return rtems_termios_open(major, minor, arg, &poll_callbacks);

        default:
            rtems_fatal_error_occurred(0xC07A1310);
    }
    return RTEMS_INTERNAL_ERROR;
}

/* console_close --
 *     Close console device.
 *
 * PARAMETERS:
 *     major - major device number for console devices
 *     minor - minor device number for console
 *     arg - device close argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver
console_close(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void *arg)
{
    if (console_mode != CONSOLE_MODE_RAW)
        return rtems_termios_close (arg);
    else
        return RTEMS_SUCCESSFUL;
}

/* console_read --
 *     Read from the console device
 *
 * PARAMETERS:
 *     major - major device number for console devices
 *     minor - minor device number for console
 *     arg - device read argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver
console_read(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void *arg)
{
    if (console_mode != CONSOLE_MODE_RAW)
    {
        return rtems_termios_read (arg);
    }
    else
    {
        rtems_libio_rw_args_t *argp = arg;
        char *buf = argp->buffer;
        int count = argp->count;
        int n = 0;
        int c;
        while (n < count)
        {
            do {
                c = mcfuart_poll_read(&uart[minor]);
            } while (c == -1);
            if (c == '\r')
                c = '\n';
            *(buf++) = c;
            n++;
            if (c == '\n')
                break;
        }
        argp->bytes_moved = n;
        return RTEMS_SUCCESSFUL;
    }
}

/* console_write --
 *     Write to the console device
 *
 * PARAMETERS:
 *     major - major device number for console devices
 *     minor - minor device number for console
 *     arg - device write argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver
console_write(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void *arg
)
{
    if (console_mode != CONSOLE_MODE_RAW)
    {
        return rtems_termios_write (arg);
    }
    else
    {
        rtems_libio_rw_args_t *argp = arg;
        char cr = '\r';
        char *buf = argp->buffer;
        int count = argp->count;
        int i;
        for (i = 0; i < count; i++)
        {
            if (*buf == '\n')
                mcfuart_poll_write(&uart[minor], &cr, 1);
            mcfuart_poll_write(&uart[minor], buf, 1);
            buf++;
        }
        argp->bytes_moved = count;
        return RTEMS_SUCCESSFUL;
    }
}

/* console_control --
 *     Handle console device I/O control (IOCTL)
 *
 * PARAMETERS:
 *     major - major device number for console devices
 *     minor - minor device number for console
 *     arg - device ioctl argument
 *
 * RETURNS:
 *     RTEMS error code
 */
rtems_device_driver
console_control(rtems_device_major_number major,
                rtems_device_minor_number minor,
                void *arg)
{
    if (console_mode != CONSOLE_MODE_RAW)
    {
        return rtems_termios_ioctl (arg);
    }
    else
    {
        return RTEMS_SUCCESSFUL;
    }
}
