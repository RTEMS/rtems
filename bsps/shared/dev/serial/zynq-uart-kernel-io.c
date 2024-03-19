/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup zynq_uart
 *
 * @brief This source file contains the definition of ::BSP_output_char and
 *   ::BSP_poll_char.
 */

/*
 * Copyright (C) 2013, 2024 embedded brains GmbH & Co. KG
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
#include <dev/serial/zynq-uart-regs.h>
#include <rtems/sysinit.h>

static void zynq_uart_kernel_output_char(char c)
{
  volatile zynq_uart *regs =
    (volatile zynq_uart *) ZYNQ_UART_KERNEL_IO_BASE_ADDR;

  zynq_uart_write_char_polled(regs, c);
}

static int zynq_uart_kernel_poll_char(void)
{
  volatile zynq_uart *regs =
    (volatile zynq_uart *) ZYNQ_UART_KERNEL_IO_BASE_ADDR;

  return zynq_uart_read_char_polled(regs);
}

static void zynq_uart_kernel_early_init(char c);

static void zynq_uart_kernel_init(void)
{
  volatile zynq_uart *regs =
    (volatile zynq_uart *) ZYNQ_UART_KERNEL_IO_BASE_ADDR;

  if (BSP_output_char != zynq_uart_kernel_early_init) {
    return;
  }

  zynq_uart_initialize(regs);
  BSP_output_char = zynq_uart_kernel_output_char;
}

static void zynq_uart_kernel_early_init(char c)
{
  zynq_uart_kernel_init();
  zynq_uart_kernel_output_char(c);
}

BSP_output_char_function_type BSP_output_char = zynq_uart_kernel_early_init;

BSP_polling_getchar_function_type BSP_poll_char = zynq_uart_kernel_poll_char;

RTEMS_SYSINIT_ITEM(
  zynq_uart_kernel_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
