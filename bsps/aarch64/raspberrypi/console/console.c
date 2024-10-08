/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Raspberrypi4
 *
 * @brief Console Configuration
 */

/*
 * Copyright (C) 2022 Mohd Noor Aman
 *
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

#include <bsp.h>
#include <dev/serial/arm-pl011.h>
#include <bsp/console-termios.h>
#include <bsp/irq.h>

#include <bspopts.h>

arm_pl011_context raspberrypi_4_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("PL011"),
  .regs = (arm_pl011_uart *)BSP_RPI4_PL011_BASE,
  .initial_baud = 115200,
  .clock = 48000000,
  .irq = BCM2711_IRQ_PL011_UART
};

const console_device console_device_table[] = {
  {
    .device_file = "/dev/ttyS0",
    .probe = console_device_probe_default,
    .handler = &arm_pl011_fns,
    .context = &raspberrypi_4_context.base
  }
};

const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);

static void output_char( char c )
{
  arm_pl011_write_polled(&raspberrypi_4_context.base, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
