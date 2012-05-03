/*
 * Console driver for pxa255 full function port by Yang Xi <hiyangxi@gmail.com>
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <termios.h>

#include <pxa255.h>
#include <ffuart.h>
#include <rtems/bspIo.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>

volatile int dbg_dly;

/* static function prototypes */
static int     ffuart_first_open(int major, int minor, void *arg);
static int     ffuart_last_close(int major, int minor, void *arg);
static int     ffuart_read(int minor);
static ssize_t ffuart_write(int minor, const char *buf, size_t len);
static void    ffuart_init(int minor);
static void    ffuart_write_polled(int minor, char c);
static int     ffuart_set_attributes(int minor, const struct termios *t);

/* Pointers to functions for handling the UART. */
console_fns ffuart_fns =
{
    libchip_serial_default_probe,
    ffuart_first_open,
    ffuart_last_close,
    ffuart_read,
    ffuart_write,
    ffuart_init,
    ffuart_write_polled,   /* not used in this driver */
    ffuart_set_attributes,
    FALSE      /* TRUE if interrupt driven, FALSE if not. */
};


/*
 * This is called the first time each device is opened. Since
 * the driver is polled, we don't have to do anything. If the driver
 * were interrupt driven, we'd enable interrupts here.
 */
static int ffuart_first_open(int major, int minor, void *arg)
{
    return 0;
}


/*
 * This is called the last time each device is closed.  Since
 * the driver is polled, we don't have to do anything. If the driver
 * were interrupt driven, we'd disable interrupts here.
 */
static int ffuart_last_close(int major, int minor, void *arg)
{
    return 0;
}


/*
 * Read one character from UART.
 *
 * return -1 if there's no data, otherwise return
 * the character in lowest 8 bits of returned int.
 */
static int ffuart_read(int minor)
{
    char c;
    console_tbl *console_entry;
    ffuart_reg_t *ffuart;

    console_entry = BSP_get_uart_from_minor(minor);

    if (console_entry == NULL) {
        return -1;
    }

    ffuart = (ffuart_reg_t *)console_entry->ulCtrlPort1;

    if (!(ffuart->lsr & FULL_RECEIVE)) {
        return -1;
    }

    c  = ffuart->rbr & 0xff;

    return c;
}


/*
 * Write buffer to UART
 *
 * return 1 on success, -1 on error
 */
static ssize_t ffuart_write(int minor, const char *buf, size_t len)
{
    size_t i, x;
    char c;
    console_tbl *console_entry;
    ffuart_reg_t *ffuart;

    console_entry = BSP_get_uart_from_minor(minor);

    if (console_entry == NULL) {
        return -1;
    }

    ffuart = (ffuart_reg_t *)console_entry->ulCtrlPort1;

    for (i = 0; i < len; i++) {

        while(1) {
            if (ffuart->lsr & SEND_EMPTY) {
                break;
            }
        }

        c = (char) buf[i];
#if ON_SKYEYE != 1
	if(c=='\n'){
	  ffuart->rbr = '\r';
	  for (x = 0; x < 100; x++) {
            dbg_dly++; /* using a global so this doesn't get optimized out */
	  }
	  while(1){
	    if(ffuart->lsr & SEND_EMPTY){
	      break;
	    }
	  }
	}
#endif
        ffuart->rbr = c;

        /* the TXRDY flag does not seem to update right away (is this true?) */
        /* so we wait a bit before continuing */
        for (x = 0; x < 100; x++) {
            dbg_dly++; /* using a global so this doesn't get optimized out */
        }
    }

    return 1;
}


static void ffuart_init(int minor)
{


    console_tbl *console_entry;
    ffuart_reg_t  *ffuart;
    unsigned int divisor;

    console_entry = BSP_get_uart_from_minor(minor);



    if (console_entry == NULL) {
        return;
    }

    ffuart = (ffuart_reg_t *)console_entry->ulCtrlPort1;
    ffuart->lcr |= DLAB;
    /*Set the Bound*/
    ffuart->lcr |= DLAB;
    divisor = FREQUENCY_UART / (115200*16);
    ffuart->rbr = divisor & 0xff;
    ffuart->ier = (divisor >> 8)&0xff;
    /*Disable FIFO*/
    ffuart->iir = 0;
    ffuart->lcr &=~DLAB;
    /*Enable UART*/
    ffuart->ier = 0x40;
    ffuart->lcr = EIGHT_BITS_NOPARITY_1STOPBIT;

}

/* I'm not sure this is needed for the shared console driver. */
static void ffuart_write_polled(int minor, char c)
{
    ffuart_write(minor, &c, 1);
}

/* This is for setting baud rate, bits, etc. */
static int ffuart_set_attributes(int minor, const struct termios *t)
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
int ffuart_poll_read(int minor)
{
    return ffuart_read(minor);
}


/*
 * Write a character to the console. This is used by printk() and
 * maybe other low level functions. It should not use interrupts or any
 * RTEMS system calls. It needs to be very simple
 */
static void _BSP_put_char( char c ) {
    ffuart_write_polled(0, c);
}

static int _BSP_poll_char(void) {
  return ffuart_poll_read(0);
}

BSP_output_char_function_type     BSP_output_char = _BSP_put_char;
BSP_polling_getchar_function_type BSP_poll_char = _BSP_poll_char;
