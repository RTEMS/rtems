/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze debug IO support
 */

/*
 * Copyright (C) 2015 Hesham Almatary
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <dev/serial/uartlite_l.h>
#include <rtems/bspIo.h>

#include <bsp.h>
#include <bspopts.h>

static uint32_t mblaze_uart_base = 0;

static void output_char( char c )
{
  if (mblaze_uart_base == 0 ) {
    mblaze_uart_base = try_get_prop_from_device_tree(
      "xlnx,xps-uartlite-1.00.a",
      "reg",
      BSP_MICROBLAZE_FPGA_UART_BASE
    );
  }

    if ( c == '\n' ) {
    XUartLite_SendByte( mblaze_uart_base, '\r' );
  }
  XUartLite_SendByte( mblaze_uart_base, c );
}

static int xUartLite_RecvByte( int minor )
{
  if (mblaze_uart_base == 0 ) {
    mblaze_uart_base = try_get_prop_from_device_tree(
      "xlnx,xps-uartlite-1.00.a",
      "reg",
      BSP_MICROBLAZE_FPGA_UART_BASE
    );
  }

  if ( XUartLite_IsReceiveEmpty( mblaze_uart_base ) ) {
    return -1;
  }

  return XUartLite_ReadReg( mblaze_uart_base, XUL_RX_FIFO_OFFSET );
}

static int get_char( void )
{
  return xUartLite_RecvByte( 0 );
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = get_char;
