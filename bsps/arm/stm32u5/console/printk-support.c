/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32U5
 *
 * @brief Support for polled prints on STM32U5.
 *
 * Implementation of the STM32U5 printk (polled) output.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <bsp/fatal.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>

#include <stm32u5/hal.h>

static void stm32u5_output_char( char c )
{
  stm32u5_uart_polled_write( &STM32U5_PRINTK_INSTANCE.device, c );
}

static void stm32u5_output_char_init( void )
{
  UART_HandleTypeDef *uart;
  HAL_StatusTypeDef   status;

  uart   = &STM32U5_PRINTK_INSTANCE.uart;
  status = HAL_UART_Init( uart );
  if ( status != HAL_OK ) {
    bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_0 );
  }
  status = HAL_UARTEx_SetTxFifoThreshold( uart, UART_TXFIFO_THRESHOLD_1_8 );
  if ( status != HAL_OK ) {
    bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_0 );
  }
  status = HAL_UARTEx_SetRxFifoThreshold( uart, UART_RXFIFO_THRESHOLD_1_8 );
  if ( status != HAL_OK ) {
    bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_0 );
  }
  status = HAL_UARTEx_EnableFifoMode( uart );
  if ( status != HAL_OK ) {
    bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_0 );
  }

  BSP_output_char = stm32u5_output_char;
}

static void stm32u5_output_char_init_early( char c )
{
  stm32u5_output_char_init();
  stm32u5_output_char( c );
}

static int stm32u5_poll_char( void )
{
  return stm32u5_uart_polled_read( &STM32U5_PRINTK_INSTANCE.device );
}

BSP_output_char_function_type BSP_output_char = stm32u5_output_char_init_early;

BSP_polling_getchar_function_type BSP_poll_char = stm32u5_poll_char;

RTEMS_SYSINIT_ITEM(
  stm32u5_output_char_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
