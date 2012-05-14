/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief Console configuration.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>

#include <rtems/bspIo.h>

#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include "../../../shared/console_private.h"

#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/qoriq.h>
#include <bsp/intercom.h>
#include <bsp/uart-bridge.h>

#define CONSOLE_COUNT \
  (QORIQ_UART_0_ENABLE \
    + QORIQ_UART_1_ENABLE \
    + QORIQ_UART_BRIDGE_0_ENABLE \
    + QORIQ_UART_BRIDGE_1_ENABLE)

#if (QORIQ_UART_0_ENABLE + QORIQ_UART_BRIDGE_0_ENABLE == 2) \
  || (QORIQ_UART_1_ENABLE + QORIQ_UART_BRIDGE_1_ENABLE == 2)
  #define BRIDGE_MASTER
#elif QORIQ_UART_BRIDGE_0_ENABLE || QORIQ_UART_BRIDGE_1_ENABLE
  #define BRIDGE_SLAVE
#endif

#ifdef BRIDGE_MASTER
  #define BRIDGE_FNS &qoriq_uart_bridge_master
  #if QORIQ_UART_BRIDGE_0_ENABLE
    static uart_bridge_master_control bridge_0_control = {
      .device_path = "/dev/ttyS0",
      .type = INTERCOM_TYPE_UART_0,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_0_control.transmit_fifo
      )
    };
    #define BRIDGE_0_CONTROL &bridge_0_control
  #endif
  #if QORIQ_UART_BRIDGE_1_ENABLE
    static uart_bridge_master_control bridge_1_control = {
      .device_path = "/dev/ttyS1",
      .type = INTERCOM_TYPE_UART_1,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_1_control.transmit_fifo
      )
    };
    #define BRIDGE_1_CONTROL &bridge_1_control
  #endif
#endif

#ifdef BRIDGE_SLAVE
  #define BRIDGE_FNS &qoriq_uart_bridge_slave
  #if QORIQ_UART_BRIDGE_0_ENABLE
    static uart_bridge_slave_control bridge_0_control = {
      .type = INTERCOM_TYPE_UART_0,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_0_control.transmit_fifo
      )
    };
    #define BRIDGE_0_CONTROL &bridge_0_control
  #endif
  #if QORIQ_UART_BRIDGE_1_ENABLE
    static uart_bridge_slave_control bridge_1_control = {
      .type = INTERCOM_TYPE_UART_1,
      .transmit_fifo = RTEMS_CHAIN_INITIALIZER_EMPTY(
        bridge_1_control.transmit_fifo
      )
    };
    #define BRIDGE_1_CONTROL &bridge_1_control
  #endif
#endif

#ifdef BSP_USE_UART_INTERRUPTS
  #define DEVICE_FNS &ns16550_fns
#else
  #define DEVICE_FNS &ns16550_fns_polled
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

unsigned long Console_Configuration_Count = CONSOLE_COUNT;
console_tbl Console_Configuration_Ports [CONSOLE_COUNT] = {
  #if QORIQ_UART_0_ENABLE
    {
      .sDeviceName = "/dev/ttyS0",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = DEVICE_FNS,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) BSP_CONSOLE_BAUD,
      .ulCtrlPort1 = (uintptr_t) &qoriq.uart_0,
      .ulCtrlPort2 = 0,
      .ulDataPort =  (uintptr_t) &qoriq.uart_0,
      .getRegister = get_register,
      .setRegister = set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 0,
      .ulIntVector = QORIQ_IRQ_DUART
    },
  #endif
  #if QORIQ_UART_1_ENABLE
    {
      .sDeviceName = "/dev/ttyS1",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = DEVICE_FNS,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) BSP_CONSOLE_BAUD,
      .ulCtrlPort1 = (uintptr_t) &qoriq.uart_1,
      .ulCtrlPort2 = 0,
      .ulDataPort =  (uintptr_t) &qoriq.uart_1,
      .getRegister = get_register,
      .setRegister = set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 0,
      .ulIntVector = QORIQ_IRQ_DUART
    },
  #endif
  #if QORIQ_UART_BRIDGE_0_ENABLE
    {
      #if QORIQ_UART_1_ENABLE
        .sDeviceName = "/dev/ttyB0",
      #else
        .sDeviceName = "/dev/ttyS0",
      #endif
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = BRIDGE_FNS,
      .pDeviceParams = BRIDGE_0_CONTROL
    },
  #endif
  #if QORIQ_UART_BRIDGE_1_ENABLE
    {
      #if QORIQ_UART_1_ENABLE
        .sDeviceName = "/dev/ttyB1",
      #else
        .sDeviceName = "/dev/ttyS1",
      #endif
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = BRIDGE_FNS,
      .pDeviceParams = BRIDGE_1_CONTROL
    }
  #endif
};

static void output_char(char c)
{
  const console_fns *con = Console_Port_Tbl [Console_Port_Minor]->pDeviceFns;
  
  if (c == '\n') {
    con->deviceWritePolled((int) Console_Port_Minor, '\r');
  }
  con->deviceWritePolled((int) Console_Port_Minor, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
