/*
 * Raw Console Driver for SH4 simulator
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <termios.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <syscall.h>


/* Console operations mode:
 *     0 - raw (non-termios) polled input/output
 *     1 - termios-based polled input/output
 *     2 - termios-based interrupt-driven input/output
 */
#define CONSOLE_MODE_RAW  (0)
#define CONSOLE_MODE_POLL (1)
#define CONSOLE_MODE_INT  (2)

int console_mode = CONSOLE_MODE_RAW;


/* console_poll_read --
 *     poll read operation for simulator console through trap 34 mechanism.
 *
 * PARAMETERS:
 *     minor - minor device number
 *
 * RETURNS:
 *     character code red from UART, or -1 if there is no characters
 *     available
 */
static int
console_poll_read(int minor)
{
    unsigned char buf;
    int rc;
    rc = __trap34(SYS_read, 0, &buf, sizeof(buf));
    if (rc != -1)
        return buf;
    else
        return -1;
}

/* console_interrupt_write --
 *     interrupt write operation for simulator console: not supported.
 *
 * PARAMETERS:
 *     minor - minor device number
 *     buf - output buffer
 *     len - output buffer length
 *
 * RETURNS:
 *     result code
 */
static int
console_interrupt_write(int minor, const char *buf, int len)
{
    return -1;
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
 *     result code (0)
 */
static int
console_poll_write(int minor, const char *buf, int len)
{
    int rc;
    if (minor == 0)
    {
        rc = __trap34(SYS_write, 1, (char *)buf, len);
        return 0;
    }
    return -1;
}

/* console_set_attributes --
 *     wrapper for hardware-dependent termios attributes setting.
 *     Null implementation for simulator BSP.
 *
 * PARAMETERS:
 *     minor - minor device number
 *     t - pointer to the termios structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
static int
console_set_attributes(int minor, const struct termios *t)
{
    return RTEMS_SUCCESSFUL;
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
    if (minor == 0)
        return RTEMS_SUCCESSFUL;
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
    if (minor == 0)
        return RTEMS_SUCCESSFUL;
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
    if (minor != 0)
    {
        return RTEMS_INVALID_NUMBER; /* Single console supported */
    }
    
    return RTEMS_SUCCESSFUL;
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
    return RTEMS_SUCCESSFUL;
}

/* console_reserve_resources --
 *     reserve termios resources for 2 UART channels
 *
 * PARAMETERS:
 *     configuration -- pointer to the RTEMS configuration table
 *
 * RETURNS:
 *     none
 */
void
console_reserve_resources(rtems_configuration_table *configuration)
{
    if (console_mode != CONSOLE_MODE_RAW)
        rtems_termios_reserve_resources (configuration, 1);
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
                c = console_poll_read(minor);
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
                console_poll_write(minor, &cr, 1);
            console_poll_write(minor, buf, 1);
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

    return RTEMS_SUCCESSFUL;
}
