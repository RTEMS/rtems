/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Console configuration.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/serial.h>
#include <libchip/ns16550.h>

#include <bsp.h>
#include <bsp/lpc32xx.h>
#include <bsp/irq.h>

static uint8_t lpc32xx_uart_get_register(uint32_t addr, uint8_t i)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  return (uint8_t) reg [i];
}

static void lpc32xx_uart_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  reg [i] = val;
}

rtems_device_minor_number Console_Port_Minor = 0;

/* FIXME: Console selection */

console_tbl Console_Port_Tbl [] = {
  #ifdef LPC32XX_CONFIG_U5CLK
    {
      .sDeviceName = "/dev/ttyS5",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) 1,
      .ulCtrlPort1 = LPC32XX_BASE_UART_5,
      .ulCtrlPort2 = 0,
      .ulDataPort = LPC32XX_BASE_UART_5,
      .getRegister = lpc32xx_uart_get_register,
      .setRegister = lpc32xx_uart_set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = LPC32XX_IRQ_UART_5
    },
  #endif
  #ifdef LPC32XX_CONFIG_U3CLK
    {
      .sDeviceName = "/dev/ttyS3",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) 1,
      .ulCtrlPort1 = LPC32XX_BASE_UART_3,
      .ulCtrlPort2 = 0,
      .ulDataPort = LPC32XX_BASE_UART_3,
      .getRegister = lpc32xx_uart_get_register,
      .setRegister = lpc32xx_uart_set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = LPC32XX_IRQ_UART_3
    },
  #endif
  #ifdef LPC32XX_CONFIG_U4CLK
    {
      .sDeviceName = "/dev/ttyS4",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) 1,
      .ulCtrlPort1 = LPC32XX_BASE_UART_4,
      .ulCtrlPort2 = 0,
      .ulDataPort = LPC32XX_BASE_UART_4,
      .getRegister = lpc32xx_uart_get_register,
      .setRegister = lpc32xx_uart_set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = LPC32XX_IRQ_UART_4
    },
  #endif
  #ifdef LPC32XX_CONFIG_U6CLK
    {
      .sDeviceName = "/dev/ttyS6",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) 1,
      .ulCtrlPort1 = LPC32XX_BASE_UART_6,
      .ulCtrlPort2 = 0,
      .ulDataPort = LPC32XX_BASE_UART_6,
      .getRegister = lpc32xx_uart_get_register,
      .setRegister = lpc32xx_uart_set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = LPC32XX_IRQ_UART_6
    },
  #endif
};

#define LPC32XX_UART_COUNT \
  (sizeof(Console_Port_Tbl) / sizeof(Console_Port_Tbl [0]))

unsigned long Console_Port_Count = LPC32XX_UART_COUNT;

console_data Console_Port_Data [LPC32XX_UART_COUNT];
