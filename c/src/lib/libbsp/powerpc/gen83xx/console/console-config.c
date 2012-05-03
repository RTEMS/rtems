/**
 * @file
 *
 * @brief Console configuration.
 */

/*
 * Copyright (c) 2008, 2010 embedded brains GmbH.  All rights reserved.
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
#include <libchip/ns16550.h>
#include "../../../shared/console_private.h"

#include <mpc83xx/mpc83xx.h>

#include <bspopts.h>
#include <bsp/irq.h>

#ifdef BSP_USE_UART2
  #define PORT_COUNT 2
#else
  #define PORT_COUNT 1
#endif

#ifdef BSP_USE_UART_INTERRUPTS
  #define DEVICE_FNS &ns16550_fns
#else
  #define DEVICE_FNS &ns16550_fns_polled
#endif

static uint8_t gen83xx_console_get_register(uint32_t addr, uint8_t i)
{
  volatile uint8_t *reg = (volatile uint8_t *) addr;

  return reg [i];
}

static void gen83xx_console_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  volatile uint8_t *reg = (volatile uint8_t *) addr;

  reg [i] = val; 
}

unsigned long Console_Configuration_Count = PORT_COUNT;

console_tbl Console_Configuration_Ports [PORT_COUNT] = {
  {
    .sDeviceName = "/dev/ttyS0",
    .deviceType = SERIAL_NS16550,
    .pDeviceFns = DEVICE_FNS,
    .deviceProbe = NULL,
    .pDeviceFlow = NULL,
    .ulMargin = 16,
    .ulHysteresis = 8,
    .pDeviceParams = (void *) BSP_CONSOLE_BAUD,
    .ulCtrlPort1 = (uint32_t) &mpc83xx.duart [0],
    .ulCtrlPort2 = 0,
    .ulDataPort =  (uint32_t) &mpc83xx.duart [0],
    .getRegister = gen83xx_console_get_register,
    .setRegister = gen83xx_console_set_register,
    .getData = NULL,
    .setData = NULL,
    .ulClock = 0,
#if MPC83XX_CHIP_TYPE / 10 == 830
    .ulIntVector = BSP_IPIC_IRQ_UART
#else
    .ulIntVector = BSP_IPIC_IRQ_UART1
#endif
  }
#ifdef BSP_USE_UART2
  , {
    .sDeviceName = "/dev/ttyS1",
    .deviceType = SERIAL_NS16550,
    .pDeviceFns = DEVICE_FNS,
    .deviceProbe = NULL,
    .pDeviceFlow = NULL,
    .ulMargin = 16,
    .ulHysteresis = 8,
    .pDeviceParams = (void *) BSP_CONSOLE_BAUD,
    .ulCtrlPort1 = (uint32_t) &mpc83xx.duart [1],
    .ulCtrlPort2 = 0,
    .ulDataPort =  (uint32_t) &mpc83xx.duart [1],
    .getRegister = gen83xx_console_get_register,
    .setRegister = gen83xx_console_set_register,
    .getData = NULL,
    .setData = NULL,
    .ulClock = 0,
#if MPC83XX_CHIP_TYPE / 10 == 830
    .ulIntVector = BSP_IPIC_IRQ_UART
#else
    .ulIntVector = BSP_IPIC_IRQ_UART2
#endif
  }
#endif
};

static void gen83xx_output_char(char c)
{
  const console_fns *console = Console_Port_Tbl [Console_Port_Minor]->pDeviceFns;
  
  if (c == '\n') {
    console->deviceWritePolled((int) Console_Port_Minor, '\r');
  }
  console->deviceWritePolled((int) Console_Port_Minor, c);
}

BSP_output_char_function_type  BSP_output_char = gen83xx_output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
