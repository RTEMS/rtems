/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 Pragnesh Patel <pragnesh.patel@sifive.com>
 * Copyright (c) 2019 Sachin Ghadi <sachin.ghadi@sifive.com>
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

#ifndef FE310_UART_H
#define FE310_UART_H

#define TXRXREADY (1 << 31)

#include <rtems/termiostypes.h>
#include <rtems/irq.h>

typedef struct {
  uint32_t txdata;
  uint32_t rxdata;
  uint32_t txctrl;
  uint32_t rxctrl;
  uint32_t ie;
  uint32_t ip;
  uint32_t div;
} fe310_uart_t;

/* Low-level driver specific data structure */
typedef struct {
  rtems_termios_device_context base;
  const char *device_name;
  volatile fe310_uart_t *regs;
} fe310_uart_context;

int fe310_uart_read(rtems_termios_device_context *base);
void fe310_console_putchar(rtems_termios_device_context * context,char c);

extern const rtems_termios_device_handler fe310_uart_handler;

extern fe310_uart_context driver_context;

#endif /* FE310_UART_H */
