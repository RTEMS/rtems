/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze console configuration
 */

/*
 * Copyright (C) 2015 Hesham Almatary
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <bsp/console-termios.h>
#include <bsp/microblaze-fdt-support.h>
#include <dev/serial/uartlite.h>

#include <bspopts.h>

uart_lite_context microblaze_qemu_uart_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "UARTLITE" ),
  .initial_baud = 115200
};

static bool fill_uart_base(rtems_termios_device_context *context)
{
  uint32_t mblaze_uart_base;

  mblaze_uart_base = try_get_prop_from_device_tree(
    "xlnx,xps-uartlite-1.00.a",
    "reg",
    BSP_MICROBLAZE_FPGA_UART_BASE
  );

  microblaze_qemu_uart_context.address = mblaze_uart_base;

  return true;
}

const console_device console_device_table[] = {
  {
    .device_file = "/dev/ttyS0",
    .probe = fill_uart_base,
    .handler = &microblaze_uart_fns,
    .context = &microblaze_qemu_uart_context.base
  }
};

const size_t console_device_count = RTEMS_ARRAY_SIZE( console_device_table );
