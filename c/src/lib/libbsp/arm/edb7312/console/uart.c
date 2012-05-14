/*
 * Cirrus EP7312 Console Driver
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>                /* Must be before libio.h */
#include <rtems/libio.h>
#include <termios.h>
#include <rtems/bspIo.h>

#include <ep7312.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>

#define NUM_DEVS 1
int     uart_poll_read(int minor);

static int     uart_first_open(int major, int minor, void *arg);
static int     uart_last_close(int major, int minor, void *arg);
static int     uart_read(int minor);
static ssize_t uart_write(int minor, const char *buf, size_t len);
static void    uart_init(int minor);
static void    uart_write_polled(int minor, char c);
static int     uart_set_attributes(int minor, const struct termios *t);

unsigned long Console_Configuration_Count = NUM_DEVS;

console_fns uart_fns =
{
    libchip_serial_default_probe,
    uart_first_open,
    uart_last_close,
    uart_read,
    uart_write,
    uart_init,
    uart_write_polled,
    uart_set_attributes,
    FALSE
};
console_tbl Console_Configuration_Ports[] = {
    {
        "/dev/com0",                      /* sDeviceName */
        SERIAL_CUSTOM,                    /* deviceType */
        &uart_fns,                        /* pDeviceFns */
        NULL,                             /* deviceProbe */
        NULL,                             /* pDeviceFlow */
        16,                               /* ulMargin */
        8,                                /* ulHysteresis */
        NULL,                             /* pDeviceParams */
        (uint32_t)EP7312_UARTCR1,       /* ulCtrlPort1 */
        (uint32_t)EP7312_SYSFLG1,       /* ulCtrlPort2 */
        (uint32_t)EP7312_UARTDR1,       /* ulDataPort */
        0,                                /* getRegister */
        0,                                /* setRegister */
        0,                                /* getData */
        0,                                /* setData */
        0,                                /* ulClock */
        0                                 /* ulIntVector */
    }};

static int     uart_first_open(int major, int minor, void *arg) {return 0;}
static int     uart_last_close(int major, int minor, void *arg) {return 0;}
static int     uart_read(int minor)
{
    return uart_poll_read(minor);
}

static void    uart_write_polled(int minor, char c)
{
    uart_write(minor, &c, 1);
}

static int     uart_set_attributes(int minor, const struct termios *t)
{
    return 0;
}

int uart_poll_read(int minor)
{
    volatile uint32_t   *data_reg;
    volatile uint32_t   *ctrl_reg1;
    volatile uint32_t   *ctrl_reg2;
    char        c;
    int         err;

    data_reg  = (uint32_t *)Console_Port_Tbl[minor]->ulDataPort;
    ctrl_reg1 = (uint32_t *)Console_Port_Tbl[minor]->ulCtrlPort1;
    ctrl_reg2 = (uint32_t *)Console_Port_Tbl[minor]->ulCtrlPort2;

    if ((*ctrl_reg2 & EP7312_UART_URXFE1) != 0) {
        return -1;
    }

    err  = *data_reg;
    c    = err & 0xff;
    err &= (EP7312_UART_FRMERR | EP7312_UART_PARERR | EP7312_UART_OVERR);

    return c;
}

static ssize_t uart_write(int minor, const char *buf, size_t len)
{
    volatile uint32_t   *data_reg;
    volatile uint32_t   *ctrl_reg1;
    volatile uint32_t   *ctrl_reg2;
    size_t i;
    char c;

    data_reg  = (uint32_t *)Console_Port_Tbl[minor]->ulDataPort;
    ctrl_reg1 = (uint32_t *)Console_Port_Tbl[minor]->ulCtrlPort1;
    ctrl_reg2 = (uint32_t *)Console_Port_Tbl[minor]->ulCtrlPort2;

    for (i = 0; i < len; i++) {
        /* Wait for fifo to have room */
        while ((*ctrl_reg2 & EP7312_UART_UTXFF1) != 0) {
            continue;
        }

        c = (char) buf[i];
        *data_reg = c;
    }

    return len;
}

static void uart_init(int minor)
{
    volatile uint32_t   *data_reg;
    volatile uint32_t   *ctrl_reg1;
    volatile uint32_t   *ctrl_reg2;

    data_reg  = (uint32_t *)Console_Port_Tbl[minor]->ulDataPort;
    ctrl_reg1 = (uint32_t *)Console_Port_Tbl[minor]->ulCtrlPort1;
    ctrl_reg2 = (uint32_t *)Console_Port_Tbl[minor]->ulCtrlPort2;

    /*   *ctrl_reg = (BSP_UART_DATA8       |
                 BSP_UART_STOP1       |
                 BSP_UART_PARITY_NONE |
                 EP7312_UART_FIFOEN     |
                 BSP_UART_BAUD_9600);
    */
    *ctrl_reg1 = (EP7312_UART_WRDLEN8    |
                 EP7312_UART_FIFOEN     |
                 0x17);              /* 9600 baud */

}

/*
 * Debug IO support
 */
static void _BSP_null_char(char c)
{
  uart_write_polled(0, c);
}

static int _BSP_get_char(void)
{
  return uart_poll_read(0);
}

BSP_output_char_function_type BSP_output_char = _BSP_null_char;

BSP_polling_getchar_function_type BSP_poll_char = _BSP_get_char;
