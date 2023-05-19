/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVNOEL
 *
 * @brief This source file contains NOEL-V/APBUART definitions of
 *   ::BSP_output_char_function_type and :: BSP_output_char and an
 *   implementation of console_initialize().
 */

/*
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Copyright (C) 2018 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/sysinit.h>
#include <rtems/termiostypes.h>

#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>
#include <bsp/riscv.h>

#include <libfdt.h>

#include <grlib/apbuart.h>
#include <grlib/apbuart_termios.h>
static struct apbuart_context apbuarts[RISCV_CONSOLE_MAX_APBUART_DEVICES];
static size_t apbuart_devices = 0;

static struct {
  rtems_termios_device_context *context;
  void (*putchar)(rtems_termios_device_context *base, char c);
  int (*getchar)(rtems_termios_device_context *base);
} riscv_console;

static void riscv_output_char(char c)
{
  (*riscv_console.putchar)(riscv_console.context, c);
}

static void apbuart_putchar(rtems_termios_device_context *base, char c)
{
  struct apbuart_context *ctx = (struct apbuart_context *) base;
  apbuart_outbyte_polled(ctx->regs, c);
}

static int apbuart_getchar(rtems_termios_device_context *base)
{
  struct apbuart_context *ctx = (struct apbuart_context *) base;
  return apbuart_inbyte_nonblocking(ctx->regs);
}

#define RISCV_CONSOLE_IS_COMPATIBLE(actual, actual_len, desired) \
  (actual_len == sizeof(desired) \
     && memcmp(actual, desired, sizeof(desired) - 1) == 0)

static uint32_t get_core_frequency(void)
{
  uint32_t node;
  const char *fdt;
  int len;
  const fdt32_t *val;

  fdt = bsp_fdt_get();
  node = fdt_node_offset_by_compatible(fdt, -1, "fixed-clock");

  val = fdt_getprop(fdt, node, "clock-frequency", &len);
  if (val == NULL && len != 4) {
    bsp_fatal(RISCV_FATAL_NO_APBUART_CLOCK_FREQUENCY_IN_DEVICE_TREE);
  }

  return fdt32_to_cpu(*val);
}

static void riscv_console_probe(void)
{
  const void *fdt;
  int node;

  fdt = bsp_fdt_get();

  node = fdt_next_node(fdt, -1, NULL);

  while (node >= 0) {
    const char *compat;
    int compat_len;

    compat = fdt_getprop(fdt, node, "compatible", &compat_len);
    if (compat == NULL) {
      compat_len = 0;
    }

    if (
      RISCV_CONSOLE_IS_COMPATIBLE(compat, compat_len, "gaisler,apbuart")
        && (apbuart_devices < RISCV_CONSOLE_MAX_APBUART_DEVICES)
    ) {
      struct apbuart_context *ctx;
      fdt32_t *val;
      int len;

      ctx = &apbuarts[apbuart_devices];

      ctx->regs = riscv_fdt_get_address(fdt, node);
      if (ctx->regs == NULL) {
        bsp_fatal(RISCV_FATAL_NO_APBUART_REG_IN_DEVICE_TREE);
      }

      ctx->freq_hz = get_core_frequency();

      val = (fdt32_t *) fdt_getprop(fdt, node, "interrupts", &len);
      if (val == NULL || len != 4) {
        bsp_fatal(RISCV_FATAL_NO_APBUART_INTERRUPTS_IN_DEVICE_TREE);
      }
      ctx->irq = RISCV_INTERRUPT_VECTOR_EXTERNAL(fdt32_to_cpu(val[0]));

      if (apbuart_devices == 0) {
        riscv_console.context = &ctx->base;
        riscv_console.putchar = apbuart_putchar;
        riscv_console.getchar = apbuart_getchar;
      }

      rtems_termios_device_context_initialize(&ctx->base, "APBUART");

      apbuart_devices++;
    };

    node = fdt_next_node(fdt, node, NULL);
  }

  BSP_output_char = riscv_output_char;
}

static void riscv_output_char_init(char c)
{
  riscv_console_probe();
  riscv_output_char(c);
}

BSP_output_char_function_type BSP_output_char = riscv_output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  char path[] = "/dev/ttyS?";

  rtems_termios_initialize();

  const rtems_termios_device_handler *handler = &apbuart_handler_polled;

  if (BSP_CONSOLE_USE_INTERRUPTS) {
    handler = &apbuart_handler_interrupt;
  }
  for (size_t i = 0; i < apbuart_devices; ++i) {
    struct apbuart_context *ctx;

    ctx = &apbuarts[i];
    path[sizeof(path) - 2] = (char) ('0' + i);
    rtems_termios_device_install(path, handler, NULL, &ctx->base);

    if (&ctx->base == riscv_console.context) {
      link(path, CONSOLE_DEVICE_NAME);
    }
  }

  return RTEMS_SUCCESSFUL;
}

RTEMS_SYSINIT_ITEM(
  riscv_console_probe,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
