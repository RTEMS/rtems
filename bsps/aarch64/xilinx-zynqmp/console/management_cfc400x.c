/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the management console implementation.
 */

/*
 * Copyright (C) 2024 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <bsp/aarch64-mmu.h>
#include <bsp/fdt.h>

#include <libchip/ns16550.h>
#include <libfdt.h>

#include <rtems/endian.h>
#include <rtems/sysinit.h>

uint32_t mgmt_uart_reg_shift = 0;

static uint8_t get_register(uintptr_t addr, uint8_t i)
{
  volatile uint8_t *reg = (uint8_t *) addr;

  i <<= mgmt_uart_reg_shift;
  return reg [i];
}

static void set_register(uintptr_t addr, uint8_t i, uint8_t val)
{
  volatile uint8_t *reg = (uint8_t *) addr;

  i <<= mgmt_uart_reg_shift;
  reg [i] = val;
}

static ns16550_context zynqmp_mgmt_uart_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("Management UART 0"),
  .get_reg = get_register,
  .set_reg = set_register,
  .port = 0,
  .irq = 0,
  .clock = 0,
  .initial_baud = 0,
};

__attribute__ ((weak)) void zynqmp_configure_management_console(rtems_termios_device_context *base)
{
  /* This SLIP-encoded watchdog command sets timeouts to 0xFFFFFFFF seconds. */
  const char mgmt_watchdog_cmd[] =
    "\xc0\xda\x00\x00\xff\xff\xff\xff\xff\x00\xff\xff\xff\xffM#\xc0";

  /* Send the system watchdog configuration command */
  for (int i = 0; i < sizeof(mgmt_watchdog_cmd); i++) {
    ns16550_polled_putchar(base, mgmt_watchdog_cmd[i]);
  }
}

static void zynqmp_management_console_init(void)
{
  /* Find the management console in the device tree */
  const void *fdt = bsp_fdt_get();
  const uint32_t *prop;
  uint32_t outprop[4];
  int proplen;
  int node;

  const char *alias = fdt_get_alias(fdt, "mgmtport");
  if (alias == NULL) {
    return;
  }
  node = fdt_path_offset(fdt, alias);

  prop = fdt_getprop(fdt, node, "clock-frequency", &proplen);
  if ( prop == NULL || proplen != 4 ) {
    zynqmp_mgmt_uart_context.port = 0;
    return;
  }
  outprop[0] = rtems_uint32_from_big_endian((const uint8_t *) &prop[0]);
  zynqmp_mgmt_uart_context.clock = outprop[0];

  prop = fdt_getprop(fdt, node, "current-speed", &proplen);
  if ( prop == NULL || proplen != 4 ) {
    zynqmp_mgmt_uart_context.port = 0;
    return;
  }
  outprop[0] = rtems_uint32_from_big_endian((const uint8_t *) &prop[0]);
  zynqmp_mgmt_uart_context.initial_baud = outprop[0];

  prop = fdt_getprop(fdt, node, "interrupts", &proplen);
  if ( prop == NULL || proplen != 12 ) {
    zynqmp_mgmt_uart_context.port = 0;
    return;
  }
  outprop[0] = rtems_uint32_from_big_endian((const uint8_t *) &prop[0]);
  outprop[1] = rtems_uint32_from_big_endian((const uint8_t *) &prop[1]);
  outprop[2] = rtems_uint32_from_big_endian((const uint8_t *) &prop[2]);
  /* proplen is in bytes, interrupt mapping expects a length in 32-bit cells */
  zynqmp_mgmt_uart_context.irq = bsp_fdt_map_intr(outprop, proplen / 4);
  if ( zynqmp_mgmt_uart_context.irq == 0 ) {
    zynqmp_mgmt_uart_context.port = 0;
    return;
  }

  prop = fdt_getprop(fdt, node, "reg", &proplen);
  if ( prop == NULL || proplen != 16 ) {
    zynqmp_mgmt_uart_context.port = 0;
    return;
  }
  outprop[0] = rtems_uint32_from_big_endian((const uint8_t *) &prop[0]);
  outprop[1] = rtems_uint32_from_big_endian((const uint8_t *) &prop[1]);
  outprop[2] = rtems_uint32_from_big_endian((const uint8_t *) &prop[2]);
  outprop[3] = rtems_uint32_from_big_endian((const uint8_t *) &prop[3]);
  zynqmp_mgmt_uart_context.port = ( ( (uint64_t) outprop[0] ) << 32 ) | outprop[1];
  uintptr_t uart_base = zynqmp_mgmt_uart_context.port;
  size_t uart_size = ( ( (uint64_t) outprop[2] ) << 32 ) | outprop[3];

  rtems_status_code sc = aarch64_mmu_map( uart_base,
                                          uart_size,
                                          AARCH64_MMU_DEVICE);
  if ( sc != RTEMS_SUCCESSFUL ) {
    zynqmp_mgmt_uart_context.port = 0;
    return;
  }

  prop = fdt_getprop(fdt, node, "reg-offset", &proplen);
  if ( prop == NULL || proplen != 4 ) {
    zynqmp_mgmt_uart_context.port = 0;
    return;
  }
  outprop[0] = rtems_uint32_from_big_endian((const uint8_t *) &prop[0]);
  zynqmp_mgmt_uart_context.port += outprop[0];

  prop = fdt_getprop(fdt, node, "reg-shift", &proplen);
  if ( prop == NULL || proplen != 4 ) {
    zynqmp_mgmt_uart_context.port = 0;
    return;
  }
  outprop[0] = rtems_uint32_from_big_endian((const uint8_t *) &prop[0]);
  mgmt_uart_reg_shift = outprop[0];

  ns16550_probe(&zynqmp_mgmt_uart_context.base);

  zynqmp_configure_management_console(&zynqmp_mgmt_uart_context.base);
}

RTEMS_SYSINIT_ITEM(
  zynqmp_management_console_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FIRST
);

void zynqmp_management_console_termios_init(void)
{
  if ( zynqmp_mgmt_uart_context.port != 0 ) {
    rtems_termios_device_install(
      "/dev/ttyMGMT0",
      &ns16550_handler_interrupt,
      NULL,
      &zynqmp_mgmt_uart_context.base
    );
  }
}
