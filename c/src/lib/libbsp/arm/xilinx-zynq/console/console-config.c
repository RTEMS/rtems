/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/serial.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/zynq-uart.h>

console_tbl Console_Configuration_Ports[] = {
  {
    .sDeviceName = "/dev/ttyS0",
    .deviceType = SERIAL_CUSTOM,
    .pDeviceFns = &zynq_uart_fns,
    .deviceProbe = NULL,
    .pDeviceFlow = NULL,
    .ulMargin = 0,
    .ulHysteresis = 0,
    .pDeviceParams = (void *) 115200,
    .ulCtrlPort1 = 0xe0000000,
    .ulCtrlPort2 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL,
    .getData = NULL,
    .setData = NULL,
    .ulClock = 0,
    .ulIntVector = ZYNQ_IRQ_UART_0
  }, {
    .sDeviceName = "/dev/ttyS1",
    .deviceType = SERIAL_CUSTOM,
    .pDeviceFns = &zynq_uart_fns,
    .deviceProbe = NULL,
    .pDeviceFlow = NULL,
    .ulMargin = 0,
    .ulHysteresis = 0,
    .pDeviceParams = (void *) 115200,
    .ulCtrlPort1 = 0xe0001000,
    .ulCtrlPort2 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL,
    .getData = NULL,
    .setData = NULL,
    .ulClock = 0,
    .ulIntVector = ZYNQ_IRQ_UART_1
  }
};

unsigned long Console_Configuration_Count =
  RTEMS_ARRAY_SIZE(Console_Configuration_Ports);

static void output_char(char c)
{
  int minor = (int) Console_Port_Minor;
  const console_tbl *ct = Console_Port_Tbl != NULL ?
    Console_Port_Tbl[minor] : &Console_Configuration_Ports[minor];
  const console_fns *cf = ct->pDeviceFns;

  if (c == '\n') {
    (*cf->deviceWritePolled)(minor, '\r');
  }

  (*cf->deviceWritePolled)(minor, c);
}

static void output_char_init(char c)
{
  if (Console_Port_Tbl == NULL) {
    int minor;
    const console_fns *cf;

    bsp_console_select();

    minor = (int) Console_Port_Minor;
    cf = Console_Configuration_Ports[minor].pDeviceFns;

    (*cf->deviceInitialize)(minor);
  }

  BSP_output_char = output_char;
  output_char(c);
}

BSP_output_char_function_type BSP_output_char = output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
