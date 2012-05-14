/*
 *  console driver for S3C2400 UARTs
 *
 *  This driver uses the shared console driver in
 *  ...../libbsp/shared/console.c
 *
 *  If you want the driver to be interrupt driven, you
 *  need to write the ISR, and in the ISR insert the
 *  chars into termios's queue.
 *
 *  Copyright (c) 2004 Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
*/
#include <bsp.h>                /* Must be before libio.h */
#include <rtems/libio.h>
#include <termios.h>
#include <rtems/bspIo.h>

/* Put the CPU (or UART) specific header file #include here */
#include <s3c24xx.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>

/* How many serial ports? */
#define NUM_DEVS       1

int     uart_poll_read(int minor);

int dbg_dly;

/* static function prototypes */
static int     uart_first_open(int major, int minor, void *arg);
static int     uart_last_close(int major, int minor, void *arg);
static int     uart_read(int minor);
static ssize_t uart_write(int minor, const char *buf, size_t len);
static void    uart_init(int minor);
static void    uart_write_polled(int minor, char c);
static int     uart_set_attributes(int minor, const struct termios *t);

/* These are used by code in console.c */
unsigned long Console_Configuration_Count = NUM_DEVS;

/* Pointers to functions for handling the UART. */
console_fns uart_fns =
{
    libchip_serial_default_probe,
    uart_first_open,
    uart_last_close,
    uart_read,
    uart_write,
    uart_init,
    uart_write_polled,   /* not used in this driver */
    uart_set_attributes,
    FALSE      /* TRUE if interrupt driven, FALSE if not. */
};

/*
 * There's one item in array for each UART.
 *
 * Some of these fields are marked "NOT USED". They are not used
 * by console.c, but may be used by drivers in libchip
 *
 */
console_tbl Console_Configuration_Ports[] = {
    {
        "/dev/com0",                      /* sDeviceName */
        SERIAL_CUSTOM,                    /* deviceType */
        &uart_fns,                        /* pDeviceFns */
        NULL,                             /* deviceProbe */
        NULL,                             /* pDeviceFlow */
        0,                                /* ulMargin - NOT USED */
        0,                                /* ulHysteresis - NOT USED */
        NULL,                             /* pDeviceParams */
        0,                                /* ulCtrlPort1  - NOT USED */
        0,                                /* ulCtrlPort2  - NOT USED */
        0,                                /* ulDataPort  - NOT USED */
        NULL,                             /* getRegister - NOT USED */
        NULL,                             /* setRegister - NOT USED */
        NULL,                             /* getData - NOT USED */
        NULL,                             /* setData - NOT USED */
        0,                                /* ulClock - NOT USED */
        0                                 /* ulIntVector - NOT USED */
    }
};

/*********************************************************************/
/* Functions called via termios callbacks (i.e. the ones in uart_fns */
/*********************************************************************/

/*
 * This is called the first time each device is opened. If the driver
 * is interrupt driven, you should enable interrupts here. Otherwise,
 * it's probably safe to do nothing.
 *
 * Since micromonitor already set up the UART, we do nothing.
 */
static int uart_first_open(int major, int minor, void *arg)
{
    return 0;
}


/*
 * This is called the last time each device is closed. If the driver
 * is interrupt driven, you should disable interrupts here. Otherwise,
 * it's probably safe to do nothing.
 */
static int uart_last_close(int major, int minor, void *arg)
{
    return 0;
}


/*
 * Read one character from UART.
 *
 * Return -1 if there's no data, otherwise return
 * the character in lowest 8 bits of returned int.
 */
static int uart_read(int minor)
{
    char c;

    if (minor == 0) {
        if (rUTRSTAT0 & 0x1) {
            c = rURXH0 & 0xff;
            return c;
        } else {
            return -1;
        }
    } else {
        printk("Unknown console minor number: %d\n", minor);
        return -1;
    }

}


/*
 * Write buffer to UART
 *
 * return 1 on success, -1 on error
 */
static ssize_t uart_write(int minor, const char *buf, size_t len)
{
    int i;

    if (minor == 0) {
        for (i = 0; i < len; i++) {
            /* Wait for fifo to have room */
            while(!(rUTRSTAT0 & 0x2)) {
                continue;
            }

           rUTXH0 = (char) buf[i];
        }
    } else {
        printk("Unknown console minor number: %d\n", minor);
        return -1;
    }

    return 1;
}


/* Set up the UART. */
static void uart_init(int minor)
{
	int i;
	unsigned int reg = 0;

	/* enable UART0 */
	rCLKCON|=0x100;

	/* value is calculated so : (int)(PCLK/16./baudrate) -1 */
	reg = get_PCLK() / (16 * 115200) - 1;

	/* FIFO enable, Tx/Rx FIFO clear */
	rUFCON0 = 0x07;
	rUMCON0 = 0x0;
	/* Normal,No parity,1 stop,8 bit */
	rULCON0 = 0x3;
	/*
	 * tx=level,rx=edge,disable timeout int.,enable rx error int.,
	 * normal,interrupt or polling
	 */
	rUCON0 = 0x245;
	rUBRDIV0 = reg;

	for (i = 0; i < 100; i++);

}

/* I'm not sure this is needed for the shared console driver. */
static void    uart_write_polled(int minor, char c)
{
    uart_write(minor, &c, 1);
}

/* This is for setting baud rate, bits, etc. */
static int     uart_set_attributes(int minor, const struct termios *t)
{
    return 0;
}

/***********************************************************************/
/*
 * The following functions are not used by TERMIOS, but other RTEMS
 * functions use them instead.
 */
/***********************************************************************/
/*
 * Read from UART. This is used in the exit code, and can't
 * rely on interrupts.
*/
int uart_poll_read(int minor)
{
    return uart_read(minor);
}


/*
 * Write a character to the console. This is used by printk() and
 * maybe other low level functions. It should not use interrupts or any
 * RTEMS system calls. It needs to be very simple
 */
static void _BSP_put_char( char c ) {
    uart_write_polled(0, c);
    if (c == '\n') {
        uart_write_polled(0, '\r');
    }
}

BSP_output_char_function_type BSP_output_char = _BSP_put_char;

static int _BSP_get_char(void)
{
  return uart_poll_read(0);
}

BSP_polling_getchar_function_type BSP_poll_char = _BSP_get_char;
