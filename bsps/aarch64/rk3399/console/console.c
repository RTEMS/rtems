/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RK3399
 *
 * @brief Console Configuration
 */

/*
 * Copyright (C) 2026 Ning Yang <yangn0@qq.com>
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

#include <bsp.h>
#include <bsp/console-termios.h>
#include <bsp/rk3399.h>

#include <rtems/bspIo.h>
#include <rtems/console.h>

#include <libchip/ns16550.h>

uint32_t rk3399_uart_reg_shift = 2;

static uint8_t get_register(uintptr_t addr, uint8_t i)
{
  volatile uint8_t *reg = (uint8_t *) addr;

  i <<= rk3399_uart_reg_shift;
  return reg [i];
}

static void set_register(uintptr_t addr, uint8_t i, uint8_t val)
{
  volatile uint8_t *reg = (uint8_t *) addr;
  i <<= rk3399_uart_reg_shift;
  reg [i] = val;
}

static ns16550_context rk3399_uart_instances[5] = {
  {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("RK3399 UART 0"),
  .get_reg = get_register,
  .set_reg = set_register,
  .initial_baud = RK3399_UART_DEFAULT_BAUD,
  .port = RK3399_UART0_BASE
  }, { 
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("RK3399 UART 1"),
  .get_reg = get_register,
  .set_reg = set_register,
  .initial_baud = RK3399_UART_DEFAULT_BAUD,
  .port = RK3399_UART1_BASE
  }, { 
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("RK3399 UART 2"),
  .get_reg = get_register,
  .set_reg = set_register,
  .initial_baud = RK3399_UART_DEFAULT_BAUD,
  .port = RK3399_UART2_BASE
  }, { 
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("RK3399 UART 3"),
  .get_reg = get_register,
  .set_reg = set_register,
  .initial_baud = RK3399_UART_DEFAULT_BAUD,
  .port = RK3399_UART3_BASE
  }, { 
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("RK3399 UART 4"),
  .get_reg = get_register,
  .set_reg = set_register,
  .initial_baud = RK3399_UART_DEFAULT_BAUD,
  .port = RK3399_UART4_BASE
  }
};

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  size_t i;

  rtems_termios_initialize();

  for (i = 0; i < RTEMS_ARRAY_SIZE(rk3399_uart_instances); ++i) {
    ns16550_context *ctx = &rk3399_uart_instances[i];
    char uart[] = "/dev/ttySX";

    uart[sizeof(uart) - 2] = (char) ('0' + i);

    rtems_termios_device_install(
      &uart[0],
      &ns16550_handler_polled,
      NULL,
      &ctx->base
    );

    if (i == BSP_CONSOLE_MINOR) {
      link(&uart[0], CONSOLE_DEVICE_NAME);
    }
  }

  return RTEMS_SUCCESSFUL;
}

static void output_char(char c)
{
  rtems_termios_device_context *ctx = &rk3399_uart_instances[BSP_CONSOLE_MINOR].base;
  ns16550_polled_putchar(ctx, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
