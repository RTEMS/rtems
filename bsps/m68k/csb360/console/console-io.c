/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial ports on the mcf5272
 */

/*
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <bsp/console-polled.h>
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

static void mcf5272_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = mcf5272_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;

