/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Console configuration.
 */

/*
 * Copyright (C) 2008, 2014 embedded brains GmbH & Co. KG
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
