/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief Console configuration.
 */

/*
 * Copyright (c) 2010, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <string.h>

#include <libfdt.h>

#include <rtems/bspIo.h>

#include <libchip/ns16550.h>

#include <asm/epapr_hcalls.h>

#include <bsp.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>
#include <bsp/qoriq.h>
#include <bsp/intercom.h>
#include <bsp/uart-bridge.h>
#include <bsp/console-termios.h>

static void output_char(char c);

#ifdef QORIQ_IS_HYPERVISOR_GUEST
typedef struct {
  rtems_termios_device_context base;
  uint32_t handle;
} qoriq_bc_context;

static bool qoriq_bc_probe(rtems_termios_device_context *base)
{
  qoriq_bc_context *ctx;
  const void *fdt;
  int node;
  const uint32_t *handle;
  int len;

  fdt = bsp_fdt_get();

  node = fdt_node_offset_by_compatible(fdt, -1, "epapr,hv-byte-channel");
  if (node < 0) {
    return false;
  }

  handle = fdt_getprop(fdt, node, "hv-handle", &len);
  if (handle == NULL || len != 4) {
    return false;
  }

  ctx = (qoriq_bc_context *) base;
  ctx->handle = fdt32_to_cpu(*handle);

  BSP_output_char = output_char;
  return true;
}

static int qoriq_bc_read_polled(rtems_termios_device_context *base)
{
  qoriq_bc_context *ctx;
  char buf[EV_BYTE_CHANNEL_MAX_BYTES];
  unsigned int count;
  unsigned int status;

  ctx = (qoriq_bc_context *) base;
  count = 1;
  status = ev_byte_channel_receive(ctx->handle, &count, buf);

  if (status != EV_SUCCESS || count == 0) {
    return -1;
  }

  return (unsigned char) buf[0];
}

static void qoriq_bc_write_polled(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  qoriq_bc_context *ctx;
  uint32_t handle;

  ctx = (qoriq_bc_context *) base;
  handle = ctx->handle;

  while (len > 0) {
    unsigned int count;
    unsigned int status;
    char buf2[EV_BYTE_CHANNEL_MAX_BYTES];
    const char *out;

    if (len < EV_BYTE_CHANNEL_MAX_BYTES) {
      count = len;
      out = memcpy(buf2, buf, len);
    } else {
      count = EV_BYTE_CHANNEL_MAX_BYTES;
      out = buf;
    }

    status = ev_byte_channel_send(handle, &count, out);

    if (status == EV_SUCCESS) {
      len -= count;
      buf += count;
    }
  }
}

static const rtems_termios_device_handler qoriq_bc_handler_polled = {
  .poll_read = qoriq_bc_read_polled,
  .write = qoriq_bc_write_polled,
  .mode = TERMIOS_POLLED
};

static qoriq_bc_context qoriq_bc_context_0 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("BC 0"),
};
#endif /* QORIQ_IS_HYPERVISOR_GUEST */

#if (QORIQ_UART_0_ENABLE + QORIQ_UART_BRIDGE_0_ENABLE == 2) \
  || (QORIQ_UART_1_ENABLE + QORIQ_UART_BRIDGE_1_ENABLE == 2)
  #define BRIDGE_MASTER
#elif QORIQ_UART_BRIDGE_0_ENABLE || QORIQ_UART_BRIDGE_1_ENABLE
  #define BRIDGE_SLAVE
#endif

#ifdef BSP_USE_UART_INTERRUPTS
  #define DEVICE_FNS &ns16550_handler_interrupt
#else
  #define DEVICE_FNS &ns16550_handler_polled
#endif

#if QORIQ_UART_0_ENABLE || QORIQ_UART_1_ENABLE
  static bool uart_probe(rtems_termios_device_context *base)
  {
    ns16550_context *ctx = (ns16550_context *) base;

    ctx->clock = BSP_bus_frequency;

    return ns16550_probe(base);
  }

  static uint8_t get_register(uintptr_t addr, uint8_t i)
  {
    volatile uint8_t *reg = (uint8_t *) addr;

    return reg [i];
  }

  static void set_register(uintptr_t addr, uint8_t i, uint8_t val)
  {
    volatile uint8_t *reg = (uint8_t *) addr;

    reg [i] = val;
  }
#endif

#if QORIQ_UART_0_ENABLE
static ns16550_context qoriq_uart_context_0 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 0"),
  .get_reg = get_register,
  .set_reg = set_register,
  .port = (uintptr_t) &qoriq.uart_0,
  .irq = QORIQ_IRQ_DUART_1,
  .initial_baud = BSP_CONSOLE_BAUD
};
#endif

#if QORIQ_UART_1_ENABLE
static ns16550_context qoriq_uart_context_1 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 1"),
  .get_reg = get_register,
  .set_reg = set_register,
  .port = (uintptr_t) &qoriq.uart_1,
  .irq = QORIQ_IRQ_DUART_1,
  .initial_baud = BSP_CONSOLE_BAUD
};
#endif

#ifdef BRIDGE_MASTER
  #define BRIDGE_PROBE qoriq_uart_bridge_master_probe
  #define BRIDGE_FNS &qoriq_uart_bridge_master
  #if QORIQ_UART_BRIDGE_0_ENABLE
    static uart_bridge_master_context bridge_0_context = {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART Bridge 0"),
      .device_path = "/dev/ttyS0",
      .type = INTERCOM_TYPE_UART_0,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_0_context.transmit_fifo
      )
    };
    #define BRIDGE_0_CONTEXT &bridge_0_context.base
  #endif
  #if QORIQ_UART_BRIDGE_1_ENABLE
    static uart_bridge_master_context bridge_1_context = {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART Bridge 1"),
      .device_path = "/dev/ttyS1",
      .type = INTERCOM_TYPE_UART_1,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_1_context.transmit_fifo
      )
    };
    #define BRIDGE_1_CONTEXT &bridge_1_context.base
  #endif
#endif

#ifdef BRIDGE_SLAVE
  #define BRIDGE_PROBE console_device_probe_default
  #define BRIDGE_FNS &qoriq_uart_bridge_slave
  #if QORIQ_UART_BRIDGE_0_ENABLE
    static uart_bridge_slave_context bridge_0_context = {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART Bridge 0"),
      .type = INTERCOM_TYPE_UART_0,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_0_context.transmit_fifo
      )
    };
    #define BRIDGE_0_CONTEXT &bridge_0_context.base
  #endif
  #if QORIQ_UART_BRIDGE_1_ENABLE
    static uart_bridge_slave_context bridge_1_context = {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART Bridge 1"),
      .type = INTERCOM_TYPE_UART_1,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_1_context.transmit_fifo
      )
    };
    #define BRIDGE_1_CONTEXT &bridge_1_context.base
  #endif
#endif

const console_device console_device_table[] = {
  #ifdef QORIQ_IS_HYPERVISOR_GUEST
    {
      .device_file = "/dev/ttyBC0",
      .probe = qoriq_bc_probe,
      .handler = &qoriq_bc_handler_polled,
      .context = &qoriq_bc_context_0.base
    },
  #endif
  #if QORIQ_UART_0_ENABLE
    {
      .device_file = "/dev/ttyS0",
      .probe = uart_probe,
      .handler = DEVICE_FNS,
      .context = &qoriq_uart_context_0.base
    },
  #endif
  #if QORIQ_UART_1_ENABLE
    {
      .device_file = "/dev/ttyS1",
      .probe = uart_probe,
      .handler = DEVICE_FNS,
      .context = &qoriq_uart_context_1.base
    },
  #endif
  #if QORIQ_UART_BRIDGE_0_ENABLE
    {
      #if QORIQ_UART_1_ENABLE
        .device_file = "/dev/ttyB0",
      #else
        .device_file = "/dev/ttyS0",
      #endif
      .probe = BRIDGE_PROBE,
      .handler = BRIDGE_FNS,
      .context = BRIDGE_0_CONTEXT
    },
  #endif
  #if QORIQ_UART_BRIDGE_1_ENABLE
    {
      #if QORIQ_UART_1_ENABLE
        .device_file = "/dev/ttyB1",
      #else
        .device_file = "/dev/ttyS1",
      #endif
      .probe = BRIDGE_PROBE,
      .handler = BRIDGE_FNS,
      .context = BRIDGE_1_CONTEXT
    }
  #endif
};

const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);

static void output_char(char c)
{
  rtems_termios_device_context *base = console_device_table[0].context;

#ifdef QORIQ_IS_HYPERVISOR_GUEST
  qoriq_bc_write_polled(base, &c, 1);
#else
  ns16550_polled_putchar(base, c);
#endif
}

#ifdef QORIQ_IS_HYPERVISOR_GUEST
static void qoriq_bc_output_char_init(char c)
{
  rtems_termios_device_context *base = console_device_table[0].context;

  qoriq_bc_probe(base);
  output_char(c);
}

BSP_output_char_function_type BSP_output_char = qoriq_bc_output_char_init;
#else
BSP_output_char_function_type BSP_output_char = output_char;
#endif

BSP_polling_getchar_function_type BSP_poll_char = NULL;
