/**
 * @file
 *
 * @ingroup raspberrypi_usart
 *
 * @brief Console Configuration.
 */

/*
 * Copyright (c) 2015 Yang Qiao
 * based on work by:
 * Copyright (c) 2013 Alan Cudmore
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <rtems/bspIo.h>

#include <libchip/serial.h>

#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/usart.h>
#include <bsp/raspberrypi.h>
#include <bsp/fbcons.h>

console_tbl Console_Configuration_Ports [] = {
    {
      .sDeviceName = "/dev/ttyS0",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &bcm2835_usart_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulCtrlPort1 = BCM2835_UART0_BASE,
      .ulCtrlPort2 = 0,
      .ulClock = USART0_DEFAULT_BAUD,
      .ulIntVector = BCM2835_IRQ_ID_UART
    },
    {
      .sDeviceName ="/dev/fbcons",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &fbcons_fns,
      .deviceProbe = fbcons_probe,
      .pDeviceFlow = NULL,
    },
};

#define PORT_COUNT \
  (sizeof(Console_Configuration_Ports) \
    / sizeof(Console_Configuration_Ports [0]))

unsigned long Console_Configuration_Count = PORT_COUNT;

static void output_char(char c)
{
  const console_fns *con =
    Console_Configuration_Ports [Console_Port_Minor].pDeviceFns;

  con->deviceWritePolled((int) Console_Port_Minor, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
