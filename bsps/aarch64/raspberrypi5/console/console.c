/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RaspberryPi5
 *
 * @brief This source file contains console configuration for the
 *   Raspberry Pi 5 BSP.
 */

/*
 * Copyright (C) 2026 Preetam Das <riki10112001@gmail.com>
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

#include <bspopts.h>
#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/termiosdevice.h>
#include <dev/serial/arm-pl011.h>

#include <bsp/irq.h>
#include <bsp/fatal.h>
#include <bsp/raspberrypi5.h>

static arm_pl011_context rpi5_uart_debug_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART DEBUG"),
  .regs = (volatile arm_pl011_uart*) BCM2712_UART_DEBUG_BASE,
  .irq = BCM2712_IRQ_PL011_UART,
  .initial_baud = BCM2712_UART_DEBUG_BAUD,
  .clock = BSP_PL011_CLOCK_FREQ
};

static void output_char (char c) {
  arm_pl011_write_polled(&rpi5_uart_debug_context.base, c);
}

static int poll_char (void) {
  return arm_pl011_read_polled(&rpi5_uart_debug_context.base);
}

rtems_device_driver console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  const rtems_termios_device_handler *handler;
  rtems_status_code                   sc;

  handler = &arm_pl011_fns;

  rtems_termios_initialize();

  sc = rtems_termios_device_install( "/dev/console", handler, NULL, &rpi5_uart_debug_context.base );
  if ( sc != RTEMS_SUCCESSFUL ) {
    bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_1 );
  }

  return RTEMS_SUCCESSFUL;
}

BSP_output_char_function_type BSP_output_char = output_char;
BSP_polling_getchar_function_type BSP_poll_char = poll_char;
