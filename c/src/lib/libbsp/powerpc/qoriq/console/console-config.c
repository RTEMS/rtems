/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief Console configuration.
 */

/*
 * Copyright (c) 2010-2014 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bspIo.h>

#include <libchip/ns16550.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/qoriq.h>
#include <bsp/intercom.h>
#include <bsp/uart-bridge.h>
#include <bsp/console-termios.h>

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
  .irq = QORIQ_IRQ_DUART,
  .initial_baud = BSP_CONSOLE_BAUD
};
#endif

#if QORIQ_UART_1_ENABLE
static ns16550_context qoriq_uart_context_1 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 1"),
  .get_reg = get_register,
  .set_reg = set_register,
  .port = (uintptr_t) &qoriq.uart_1,
  .irq = QORIQ_IRQ_DUART,
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
    #define BRIDGE_0_CONTEXT &bridge_0_context
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
    #define BRIDGE_1_CONTEXT &bridge_1_context
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
    #define BRIDGE_0_CONTEXT &bridge_0_context
  #endif
  #if QORIQ_UART_BRIDGE_1_ENABLE
    static uart_bridge_slave_context bridge_1_context = {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART Bridge 1"),
      .type = INTERCOM_TYPE_UART_1,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_1_context.transmit_fifo
      )
    };
    #define BRIDGE_1_CONTEXT &bridge_1_context
  #endif
#endif

const console_device console_device_table[] = {
  #if QORIQ_UART_0_ENABLE
    {
      .device_file = "/dev/ttyS0",
      .probe = ns16550_probe,
      .handler = DEVICE_FNS,
      .context = &qoriq_uart_context_0.base
    },
  #endif
  #if QORIQ_UART_1_ENABLE
    {
      .device_file = "/dev/ttyS1",
      .probe = ns16550_probe,
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
  rtems_termios_device_context *ctx = console_device_table[0].context;

  if (c == '\n') {
    ns16550_polled_putchar(ctx, '\r');
  }

  ns16550_polled_putchar(ctx, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
