/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
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
#include <bsp/usart.h>
#include <bsp/stm32f4.h>

console_tbl Console_Configuration_Ports [] = {
  #ifdef STM32F4_ENABLE_USART_1
    {
      .sDeviceName = "/dev/ttyS0",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &stm32f4_usart_fns,
      .ulCtrlPort1 = (uint32_t) STM32F4_USART_1,
      .ulCtrlPort2 = 0,
      .ulClock = STM32F4_USART_BAUD,
      .ulIntVector = STM32F4_IRQ_USART1
    },
  #endif
  #ifdef STM32F4_ENABLE_USART_2
    {
      .sDeviceName = "/dev/ttyS2",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &stm32f4_usart_fns,
      .ulCtrlPort1 = (uint32_t) STM32F4_USART_2,
      .ulCtrlPort2 = 1,
      .ulClock = STM32F4_USART_BAUD,
      .ulIntVector = STM32F4_IRQ_USART2
    },
  #endif
  #ifdef STM32F4_ENABLE_USART_3
    {
      .sDeviceName = "/dev/ttyS2",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &stm32f4_usart_fns,
      .ulCtrlPort1 = (uint32_t) STM32F4_USART_3,
      .ulCtrlPort2 = 2,
      .ulClock = STM32F4_USART_BAUD,
      .ulIntVector = STM32F4_IRQ_USART3
    },
  #endif
  #ifdef STM32F4_ENABLE_UART_4
    {
      .sDeviceName = "/dev/ttyS3",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &stm32f4_usart_fns,
      .ulCtrlPort1 = (uint32_t) STM32F4_USART_4,
      .ulCtrlPort2 = 3,
      .ulClock = STM32F4_USART_BAUD,
      .ulIntVector = STM32F4_IRQ_UART4
    },
  #endif
  #ifdef STM32F4_ENABLE_UART_5
    {
      .sDeviceName = "/dev/ttyS4",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &stm32f4_usart_fns,
      .ulCtrlPort1 = (uint32_t) STM32F4_USART_5,
      .ulCtrlPort2 = 4,
      .ulClock = STM32F4_USART_BAUD,
      .ulIntVector = STM32F4_IRQ_UART5
    },
  #endif
  #ifdef STM32F4_ENABLE_USART_6
    {
      .sDeviceName = "/dev/ttyS5",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &stm32f4_usart_fns,
      .ulCtrlPort1 = (uint32_t) STM32F4_USART_6,
      .ulCtrlPort2 = 5,
      .ulClock = STM32F4_USART_BAUD,
      .ulIntVector = STM32F4_IRQ_USART6
    },
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
