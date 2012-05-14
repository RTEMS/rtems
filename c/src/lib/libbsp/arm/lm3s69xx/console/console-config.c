/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/bspIo.h>

#include <libchip/serial.h>

#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/uart.h>
#include <bsp/lm3s69xx.h>

console_tbl Console_Configuration_Ports [] = {
  #ifdef LM3S69XX_ENABLE_UART_0
    {
      .sDeviceName = "/dev/ttyS0",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &lm3s69xx_uart_fns,
      .ulCtrlPort1 = LM3S69XX_UART_0_BASE,
      .ulClock = LM3S69XX_UART_BAUD,
      .ulIntVector = LM3S69XX_IRQ_UART_0
    },
  #endif
  #ifdef LM3S69XX_ENABLE_UART_1
    {
      .sDeviceName = "/dev/ttyS1",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &lm3s69xx_uart_fns,
      .ulCtrlPort1 = LM3S69XX_UART_1_BASE,
      .ulClock = LM3S69XX_UART_BAUD,
      .ulIntVector = LM3S69XX_IRQ_UART_1
    }
  #endif
  #ifdef LM3S69XX_ENABLE_UART_2
    {
      .sDeviceName = "/dev/ttyS2",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &lm3s69xx_uart_fns,
      .ulCtrlPort1 = LM3S69XX_UART_2_BASE,
      .ulClock = LM3S69XX_UART_BAUD,
      .ulIntVector = LM3S69XX_IRQ_UART_2
    }
  #endif
};

#define PORT_COUNT \
  (sizeof(Console_Configuration_Ports) \
    / sizeof(Console_Configuration_Ports [0]))

unsigned long Console_Configuration_Count = PORT_COUNT;

static void output_char(char c)
{
  const console_fns *con =
    Console_Configuration_Ports [Console_Port_Minor].pDeviceFns;
  
  if (c == '\n') {
    con->deviceWritePolled((int) Console_Port_Minor, '\r');
  }
  con->deviceWritePolled((int) Console_Port_Minor, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
