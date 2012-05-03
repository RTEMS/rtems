/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial ports on the mcf5272
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <mcf5272/mcf5272.h>

volatile int g_cnt = 0;

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 *
 */

void console_initialize_hardware(void)
{
}


/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
    uart_regs_t *uart;
    int i;
    if (port == 0) {
        uart = g_uart0_regs;
    } else {
        uart = g_uart1_regs;
    }

    /* wait for the fifo to make room */
/*    while ((uart->usr & MCF5272_USR_TXRDY) == 0) { */
    while ((uart->ucsr & MCF5272_USR_TXRDY) == 0) {
        continue;
    }

    uart->udata = ch;
    for (i = 0; i < 1000; i++) g_cnt++;
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
    uart_regs_t *uart;
    unsigned char c;

    if (port == 0) {
        uart = g_uart0_regs;
    } else {
        uart = g_uart1_regs;
    }

/*    if (uart->usr & MCF5272_USR_RXRDY) { */
    if (uart->ucsr & MCF5272_USR_RXRDY) {
        c = (char)uart->udata;
        return c;
    } else {
        return -1;
    }
}

#include <rtems/bspIo.h>

void mcf5272_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = mcf5272_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;

