/**
 * @file
 *
 * @ingroup beagle
 *
 * @brief Console configuration.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/serial.h>
#include <libchip/ns16550.h>

#include <bsp.h>
#include <bsp/beagle.h>
#include <bsp/irq.h>

#define UART3_THR (*(volatile unsigned int *)0x49020000)
#define UART3_RHR (*(volatile unsigned int *)0x49020000)
#define UART3_LSR (*(volatile unsigned int *)0x49020014)
#define TX_FIFO_E (1<<5)
#define RX_FIFO_E (1<<0)

static uint8_t beagle_uart_get_register(uint32_t addr, uint8_t i)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  return (uint8_t) reg [i];
}

static void beagle_uart_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  reg [i] = val;
}

/* FIXME: Console selection */

console_tbl Console_Configuration_Ports [] = {
  #ifdef BEAGLE_CONFIG_U5CLK
    {
      .sDeviceName = "/dev/ttyS5",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = &ns16550_fns_polled,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) 1,
      .ulCtrlPort1 = BEAGLE_BASE_UART_5,
      .ulCtrlPort2 = 0,
      .ulDataPort = BEAGLE_BASE_UART_5,
      .getRegister = beagle_uart_get_register,
      .setRegister = beagle_uart_set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = BEAGLE_IRQ_UART_5
    },
  #endif
  #ifdef BEAGLE_CONFIG_U3CLK
    {
      .sDeviceName = "/dev/ttyS3",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) 1,
      .ulCtrlPort1 = BEAGLE_BASE_UART_3,
      .ulCtrlPort2 = 0,
      .ulDataPort = BEAGLE_BASE_UART_3,
      .getRegister = beagle_uart_get_register,
      .setRegister = beagle_uart_set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = BEAGLE_IRQ_UART_3
    },
  #endif
  #ifdef BEAGLE_CONFIG_U4CLK
    {
      .sDeviceName = "/dev/ttyS4",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) 1,
      .ulCtrlPort1 = BEAGLE_BASE_UART_4,
      .ulCtrlPort2 = 0,
      .ulDataPort = BEAGLE_BASE_UART_4,
      .getRegister = beagle_uart_get_register,
      .setRegister = beagle_uart_set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = BEAGLE_IRQ_UART_4
    },
  #endif
  #ifdef BEAGLE_CONFIG_U6CLK
    {
      .sDeviceName = "/dev/ttyS6",
      .deviceType = SERIAL_NS16550,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) 1,
      .ulCtrlPort1 = BEAGLE_BASE_UART_6,
      .ulCtrlPort2 = 0,
      .ulDataPort = BEAGLE_BASE_UART_6,
      .getRegister = beagle_uart_get_register,
      .setRegister = beagle_uart_set_register,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = BEAGLE_IRQ_UART_6
    },
  #endif
  #ifdef BEAGLE_UART_1_BAUD
    {
      .sDeviceName = "/dev/ttyS1",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) BEAGLE_UART_1_BAUD,
      .ulCtrlPort1 = BEAGLE_BASE_UART_1,
      .ulCtrlPort2 = 0,
      .ulDataPort = 0,
      .getRegister = NULL,
      .setRegister = NULL,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = BEAGLE_IRQ_UART_1
    },
  #endif
  #ifdef BEAGLE_UART_2_BAUD
    {
      .sDeviceName = "/dev/ttyS2",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) BEAGLE_UART_2_BAUD,
      .ulCtrlPort1 = BEAGLE_BASE_UART_2,
      .ulCtrlPort2 = 0,
      .ulDataPort = 0,
      .getRegister = NULL,
      .setRegister = NULL,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = BEAGLE_IRQ_UART_2
    },
  #endif
  #ifdef BEAGLE_UART_7_BAUD
    {
      .sDeviceName = "/dev/ttyS7",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &ns16550_fns,
      .deviceProbe = NULL,
      .pDeviceFlow = NULL,
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) BEAGLE_UART_7_BAUD,
      .ulCtrlPort1 = BEAGLE_BASE_UART_7,
      .ulCtrlPort2 = 0,
      .ulDataPort = 0,
      .getRegister = NULL,
      .setRegister = NULL,
      .getData = NULL,
      .setData = NULL,
      .ulClock = 16,
      .ulIntVector = BEAGLE_IRQ_UART_7
    },
  #endif
};

#define BEAGLE_UART_COUNT \
  (sizeof(Console_Configuration_Ports) / \
  sizeof(Console_Configuration_Ports [0]))

unsigned long Console_Configuration_Count = BEAGLE_UART_COUNT;

static void uart_write_polled( char c ) {
  // wait until TX-Buffer is empty
  while( ( UART3_LSR & TX_FIFO_E ) == 0 )
  ;

  // send character
  UART3_THR = c;
  return c;
}


/*
* Write a character to the console. This is used by printk() and
* maybe other low level functions. It should not use interrupts or any
* RTEMS system calls. It needs to be very simple
*/
static void _BSP_put_char( char c ) {
   uart_write_polled( c );
   if (c == '\n') {
       uart_write_polled('\r');
   }
}

BSP_output_char_function_type BSP_output_char = _BSP_put_char;
