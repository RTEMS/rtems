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

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <sys/param.h>
#include <bsp/console-termios.h>
#include <bsp/fatal.h>
#include <bspopts.h>
#include <dev/serial/uartlite.h>

#ifdef BSP_MICROBLAZE_FPGA_USE_FDT
#include <bsp/fdt.h>
#include <libfdt.h>
#endif

#include <rtems/console.h>

#ifndef BSP_MICROBLAZE_FPGA_USE_FDT
static uart_lite_context uart_lite_instances[] = {
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "UARTLITE" ),
    .initial_baud = 115200,
    .address = BSP_MICROBLAZE_FPGA_UART_BASE,
#if BSP_MICROBLAZE_FPGA_USE_UART
    .enabled = 1,
#endif
#ifdef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
    .irq = BSP_MICROBLAZE_FPGA_UART_IRQ
#endif
  }
};

const console_device console_device_table[] = {
  {
    .device_file = "/dev/ttyS0",
    .handler = &microblaze_uart_fns,
    .context = &uart_lite_instances[0].base
  }
};

const size_t console_device_count = RTEMS_ARRAY_SIZE( console_device_table );
#else
static uart_lite_context uart_lite_instances[BSP_MICROBLAZE_FPGA_MAX_UARTS];
console_device *dynamic_console_device_table;
size_t dynamic_console_device_count;

/* Override the console_device_table and console_device_count */
#define console_device_table dynamic_console_device_table
#define console_device_count dynamic_console_device_count
#endif /* BSP_MICROBLAZE_FPGA_USE_FDT */

#ifdef BSP_MICROBLAZE_FPGA_USE_FDT
static int microblaze_fpga_get_stdout_node(const void *fdt)
{
  int node;
  int len;
  int offset;
  const char *console;
  const char *q;

  node = fdt_path_offset( fdt, "/chosen" );
  if ( node < 0 ) {
    return 0;
  }

  console = fdt_getprop( fdt, node, "stdout-path", NULL );
  if ( console == NULL ) {
    return 0;
  }

  q = strchr(console, ':');
  if ( !q ) {
    return 0;
  }

  len = q - console;

  /* Get the node specified by stdout-path */
  offset = fdt_path_offset_namelen( fdt, console, len );
  if (offset < 0) {
    return 0;
  }

  return offset;
}

static void initialize_uart_arrays(uint32_t max_uarts) {
  dynamic_console_device_table = calloc(max_uarts, sizeof(console_device));
  dynamic_console_device_count = max_uarts;

  for (uint32_t i = 0; i < max_uarts; i++) {
    rtems_termios_device_context_initialize(&uart_lite_instances[i].base, "UARTLITE");
    uart_lite_instances[i].initial_baud = 115200;

    dynamic_console_device_table[i].device_file = malloc(11);
    snprintf((char *)console_device_table[i].device_file, 11, "/dev/ttyS%u", i);
    dynamic_console_device_table[i].handler = &microblaze_uart_fns;
    dynamic_console_device_table[i].context = &uart_lite_instances[i].base;
  }
}
#endif

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  uint32_t port;
  uint32_t stdout_port = BSP_MICROBLAZE_FPGA_CONSOLE_UART;

#ifdef BSP_MICROBLAZE_FPGA_USE_FDT
  initialize_uart_arrays(BSP_MICROBLAZE_FPGA_MAX_UARTS);

  const char compatible[] = "xlnx,xps-uartlite-1.00.a";
  const void *fdt = bsp_fdt_get();
  int len;
  int stdout_node = microblaze_fpga_get_stdout_node(fdt);
  int node = fdt_node_offset_by_compatible( fdt, -1, compatible);

  while ( node != -FDT_ERR_NOTFOUND ) {
    const uint32_t *prop;
    const void *status;
    uint32_t disabled = 0;
    port = console_device_count;

    /* check if node device status has been set to disabled */
    status = fdt_getprop( fdt, node, "status", &len );
    if ( status != NULL ) {
      if ( strncmp( status, "disabled", MIN( 9, len) ) == 0 ) {
        disabled = 1;
      }
    }

    if ( !disabled ) {
      /* use port number property as the device table index */
      prop = fdt_getprop( fdt, node, "port-number", NULL );
      if ( prop != NULL ) {
        port = fdt32_to_cpu( prop[0] );
      }

      if ( port < console_device_count ) {
        prop = fdt_getprop( fdt, node, "reg", NULL );
        if ( prop != NULL ) {
          uint32_t address = fdt32_to_cpu( prop[0] );
          uart_lite_instances[ port ].address = address;
          uart_lite_instances[ port ].enabled = 1;
          if ( node == stdout_node ) {
            stdout_port = port;
          }
        }

#ifdef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
        prop = fdt_getprop( fdt, node, "interrupts", NULL );
        if ( prop != NULL ) {
          uint32_t irq = fdt32_to_cpu( prop[0] );
          uart_lite_instances[ port ].irq = irq;
        }
#endif
      }
    }

    node = fdt_node_offset_by_compatible( fdt, node, compatible );

    if ( disabled || ( port >= console_device_count ) )
      continue;
  }
#endif /* BSP_MICROBLAZE_FPGA_USE_FDT */

  rtems_termios_initialize();

  for ( port = 0; port < console_device_count; port++ ) {
    const console_device *ctx = &console_device_table[ port ];
    rtems_status_code     sc;

    if ( !uart_lite_instances[ port ].enabled )
      continue;

    sc = rtems_termios_device_install(
      ctx->device_file,
      ctx->handler,
      ctx->flow,
      ctx->context
    );
    if ( sc != RTEMS_SUCCESSFUL ) {
      bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_0 );
    }

    if ( port == stdout_port ) {
      if ( link( ctx->device_file, CONSOLE_DEVICE_NAME ) != 0 ) {
        bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_1 );
      }
    }
  }

  return RTEMS_SUCCESSFUL;
}
