/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVESP32
 *
 * @brief This source file contains definitions of
 *   ::BSP_output_char_function_type and :: BSP_output_char and an
 *   implementation of console_initialize().
 */

/*
 * Copyright (C) 2026 Kinsey Moore <wkmoore@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp/console-polled.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/utility.h>
#include <bspopts.h>
#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/sysinit.h>
#include <rtems/termiostypes.h>
#include <string.h>
#include <stdint.h>

static uart_tx_one_char_t uart_tx_one_char = UART_TX_ONE_CHAR_ADDR;
static uart_rx_one_char_t uart_rx_one_char = UART_RX_ONE_CHAR_ADDR;
static uart_tx_flush_t    uart_tx_flush = UART_TX_FLUSH_ADDR;

#define USB_SERIAL_JTAG_EP1_REG              0x0
#define USB_SERIAL_JTAG_RDWR_BYTE( val )     BSP_FLD32( val, 0, 7 )
#define USB_SERIAL_JTAG_RDWR_BYTE_GET( reg ) BSP_FLD32GET( reg, 0, 7 )
#define USB_SERIAL_JTAG_RDWR_BYTE_SET( reg, val ) \
  BSP_FLD32SET( reg, val, 0, 7 )

#define USB_SERIAL_JTAG_EP1_CONF_REG             0x4
#define USB_SERIAL_JTAG_SERIAL_OUT_EP_DATA_AVAIL BSP_BIT32( 2 )
#define USB_SERIAL_JTAG_SERIAL_IN_EP_DATA_FREE   BSP_BIT32( 1 )
#define USB_SERIAL_JTAG_WR_DONE                  BSP_BIT32( 0 )

#define USB_READ( reg )         READ_REG( USB_SERIAL_JTAG_BASE, reg )
#define USB_WRITE( reg, value ) WRITE_REG( USB_SERIAL_JTAG_BASE, reg, value )

#define READ_REG( base, reg ) *(volatile uint32_t *) ( base + reg )
#define WRITE_REG( base, reg, value ) \
  *(volatile uint32_t *) ( base + reg ) = value

static int esp_uart_poll_char( void )
{
#ifndef ESPRESSIF_USE_USB_CONSOLE
  /* Use ROM functions to read from UART0 */
  unsigned char char_in = 0;
  unsigned char char_get_res = uart_rx_one_char( &char_in );

  if ( char_get_res == 0 ) {
    return char_in;
  }
  return -1;
#else
  if (
    ( USB_READ( USB_SERIAL_JTAG_EP1_CONF_REG ) &
      USB_SERIAL_JTAG_SERIAL_OUT_EP_DATA_AVAIL ) == 0
  ) {
    return -1;
  }
  return USB_READ( USB_SERIAL_JTAG_EP1_REG );
#endif
}

static void esp_uart_output_char( char c )
{
#ifdef ESPRESSIF_USE_USB_CONSOLE
  /* wait for tx space to be available */
  while (
    ( USB_READ( USB_SERIAL_JTAG_EP1_CONF_REG ) &
      USB_SERIAL_JTAG_SERIAL_IN_EP_DATA_FREE ) == 0
  ) {
    ;
  }
  USB_WRITE( USB_SERIAL_JTAG_EP1_REG, c );
  USB_WRITE( USB_SERIAL_JTAG_EP1_CONF_REG, USB_SERIAL_JTAG_WR_DONE );
#else
  /* Use ROM functions to write to UART0 */
  uart_tx_one_char( c );
  uart_tx_flush( 0 );
#endif
}

void console_outbyte_polled( int port, char ch )
{
  (void) port;

  esp_uart_output_char( ch );
}

int console_inbyte_nonblocking( int port )
{
  (void) port;

  return esp_uart_poll_char();
}

void console_initialize_hardware( void ) {}

BSP_output_char_function_type     BSP_output_char = esp_uart_output_char;
BSP_polling_getchar_function_type BSP_poll_char = esp_uart_poll_char;
