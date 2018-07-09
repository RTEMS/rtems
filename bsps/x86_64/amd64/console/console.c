/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libchip/ns16550.h>
#include <rtems/bspIo.h>
#include <bsp.h>
#include <bsp/console-termios.h>
#include <rtems/score/cpuimpl.h>

static uint8_t amd64_uart_get_register(uintptr_t addr, uint8_t i)
{
  return inport_byte(addr + i);
}

static void amd64_uart_set_register(uintptr_t addr, uint8_t i, uint8_t val)
{
  outport_byte(addr + i, val);
}

static ns16550_context amd64_uart_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART"),
  .get_reg = amd64_uart_get_register,
  .set_reg = amd64_uart_set_register,
  .port = (uintptr_t) COM1_BASE_IO,
  .initial_baud = COM1_CLOCK_RATE
};

/*
 * XXX: We should use the interrupt based handler once interrupts are supported
 */
const console_device console_device_table[] = {
  {
    .device_file = "/dev/console",
    .probe = console_device_probe_default,
    .handler = &ns16550_handler_polled,
    .context = &amd64_uart_context.base
  }
};
const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);

static void output_char(char c)
{
  rtems_termios_device_context *ctx = console_device_table[0].context;

  ns16550_polled_putchar(ctx, c);
}

BSP_output_char_function_type BSP_output_char   = output_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;
