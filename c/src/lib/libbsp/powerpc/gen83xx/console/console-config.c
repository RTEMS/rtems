/**
 * @file
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
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>

#include <libchip/ns16550.h>

#include <mpc83xx/mpc83xx.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/console-termios.h>

#ifdef BSP_USE_UART_INTERRUPTS
  #define DEVICE_FNS &ns16550_handler_interrupt
#else
  #define DEVICE_FNS &ns16550_handler_polled
#endif

static uint8_t gen83xx_console_get_register(uintptr_t addr, uint8_t i)
{
  volatile uint8_t *reg = (volatile uint8_t *) addr;

  return reg [i];
}

static void gen83xx_console_set_register(uintptr_t addr, uint8_t i, uint8_t val)
{
  volatile uint8_t *reg = (volatile uint8_t *) addr;

  reg [i] = val;
}

static ns16550_context gen83xx_uart_context_0 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 0"),
  .get_reg = gen83xx_console_get_register,
  .set_reg = gen83xx_console_set_register,
  .port = (uintptr_t) &mpc83xx.duart[0],
#if MPC83XX_CHIP_TYPE / 10 == 830
   .irq = BSP_IPIC_IRQ_UART,
#else
   .irq = BSP_IPIC_IRQ_UART1,
#endif
  .initial_baud = BSP_CONSOLE_BAUD
};

#ifdef BSP_USE_UART2
static ns16550_context gen83xx_uart_context_1 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 1"),
  .get_reg = gen83xx_console_get_register,
  .set_reg = gen83xx_console_set_register,
  .port = (uintptr_t) &mpc83xx.duart[1],
#if MPC83XX_CHIP_TYPE / 10 == 830
  .irq = BSP_IPIC_IRQ_UART,
#else
  .irq = BSP_IPIC_IRQ_UART2,
#endif
  .initial_baud = BSP_CONSOLE_BAUD
};
#endif

const console_device console_device_table[] = {
  {
    .device_file = "/dev/ttyS0",
    .probe = ns16550_probe,
    .handler = DEVICE_FNS,
    .context = &gen83xx_uart_context_0.base
  }
#ifdef BSP_USE_UART2
  , {
    .device_file = "/dev/ttyS1",
    .probe = ns16550_probe,
    .handler = DEVICE_FNS,
    .context = &gen83xx_uart_context_1.base
  }
#endif
};

const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);

static void gen83xx_output_char(char c)
{
  rtems_termios_device_context *ctx = console_device_table[0].context;

  ns16550_polled_putchar(ctx, c);
}

BSP_output_char_function_type BSP_output_char = gen83xx_output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
