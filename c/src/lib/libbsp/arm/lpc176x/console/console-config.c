/**
 * @file
 *
 * @ingroup lpc176x
 *
 * @brief Console configuration.
 */

/*
 * Copyright (c) 2008-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/ns16550.h>

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/irq.h>
#include <bsp/console-termios.h>

/**
 * @brief Gets the uart register according to the current address.
 *
 * @param  addr Register address.
 * @param  i Index register.
 * @return  Uart register.
 */
static inline uint8_t lpc176x_uart_get_register(
  const uintptr_t addr,
  const uint8_t   i
)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  return (uint8_t) reg[ i ];
}

/**
 * @brief Sets the uart register address according to the value passed.
 *
 * @param  addr Register address.
 * @param  i Index register.
 * @param val Value to set.
 */
static inline void lpc176x_uart_set_register(
  const uintptr_t addr,
  const uint8_t   i,
  const uint8_t   val
)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  reg[ i ] = val;
}

#ifdef LPC176X_CONFIG_CONSOLE
static ns16550_context lpc176x_uart_context_0 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 0"),
  .get_reg = lpc176x_uart_get_register,
  .set_reg = lpc176x_uart_set_register,
  .port = UART0_BASE_ADDR,
  .irq = LPC176X_IRQ_UART_0,
  .clock = LPC176X_PCLK,
  .initial_baud = LPC176X_UART_BAUD,
  .has_fractional_divider_register = true
};
#endif

#ifdef LPC176X_CONFIG_UART_1
static ns16550_context lpc176x_uart_context_1 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 1"),
  .get_reg = lpc176x_uart_get_register,
  .set_reg = lpc176x_uart_set_register,
  .port = UART1_BASE_ADDR,
  .irq = LPC176X_IRQ_UART_1,
  .clock = LPC176X_PCLK,
  .initial_baud = LPC176X_UART_BAUD,
  .has_fractional_divider_register = true
};
#endif

#ifdef LPC176X_CONFIG_UART_2
static ns16550_context lpc176x_uart_context_2 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 2"),
  .get_reg = lpc176x_uart_get_register,
  .set_reg = lpc176x_uart_set_register,
  .port = UART2_BASE_ADDR,
  .irq = LPC176X_IRQ_UART_2,
  .clock = LPC176X_PCLK,
  .initial_baud = LPC176X_UART_BAUD,
  .has_fractional_divider_register = true
};
#endif

#ifdef LPC176X_CONFIG_UART_3
static ns16550_context lpc176x_uart_context_3 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 3"),
  .get_reg = lpc176x_uart_get_register,
  .set_reg = lpc176x_uart_set_register,
  .port = UART3_BASE_ADDR,
  .irq = LPC176X_IRQ_UART_3,
  .clock = LPC176X_PCLK,
  .initial_baud = LPC176X_UART_BAUD,
  .has_fractional_divider_register = true
};
#endif

const console_device console_device_table[] = {
  #ifdef LPC176X_CONFIG_CONSOLE
    {
      .device_file = "/dev/ttyS0",
      .probe = console_device_probe_default,
      .handler = &ns16550_handler_interrupt,
      .context = &lpc176x_uart_context_0.base
    },
  #endif
  #ifdef LPC176X_CONFIG_UART_1
    {
      .device_file = "/dev/ttyS1",
      .probe = ns16550_probe,
      .handler = &ns16550_handler_interrupt,
      .context = &lpc176x_uart_context_1.base
    },
  #endif
  #ifdef LPC176X_CONFIG_UART_2
    {
      .device_file = "/dev/ttyS2",
      .probe = ns16550_probe,
      .handler = &ns16550_handler_interrupt,
      .context = &lpc176x_uart_context_2.base
    },
  #endif
  #ifdef LPC176X_CONFIG_UART_3
    {
      .device_file = "/dev/ttyS3",
      .probe = ns16550_probe,
      .handler = &ns16550_handler_interrupt,
      .context = &lpc176x_uart_context_3.base
    },
  #endif
};

const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);
