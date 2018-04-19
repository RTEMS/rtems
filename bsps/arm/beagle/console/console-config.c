/**
 * @file
 *
 * @ingroup arm_beagle
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
 * http://www.rtems.org/license/LICENSE.
 *
 * Modified by Ben Gras <beng@shrike-systems.com> to make
 * interrupt-driven uart i/o work for beagleboards; beaglebone support added.
 */

#include <libchip/serial.h>
#include <libchip/ns16550.h>

#include <rtems/bspIo.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/uart-output-char.h>

#define CONSOLE_UART_THR (*(volatile unsigned int *)BSP_CONSOLE_UART_BASE)
#define CONSOLE_UART_RHR (*(volatile unsigned int *)BSP_CONSOLE_UART_BASE)
#define CONSOLE_UART_LSR (*(volatile unsigned int *)(BSP_CONSOLE_UART_BASE+0x14))
#define CONSOLE_SYSC (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x54))
#define CONSOLE_SYSS (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x58))

#define TX_FIFO_E (1<<5)
#define RX_FIFO_E (1<<0)

static uint8_t beagle_uart_get_register(uintptr_t addr, uint8_t i)
{
  uint8_t v;
  volatile uint32_t *reg_r = (volatile uint32_t *) addr + i;

  if(reg_r == (uint32_t*) BSP_CONSOLE_UART_BASE /* reading RHR */ ) {
    /* check there should be anything in the RHR before accessing it */
    if(!(CONSOLE_UART_LSR & 0x01)) {
      return 0;
    }
  }

  v = (uint8_t) *reg_r;

  return v;
}

static void beagle_uart_set_register(uintptr_t addr, uint8_t i, uint8_t val)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  reg [i] = val;
}

console_tbl Console_Configuration_Ports [] = {
    {
      .sDeviceName = "/dev/ttyS0",
      .deviceType = SERIAL_NS16550,
#if CONSOLE_POLLED	/* option to facilitate running the tests */
      .pDeviceFns = &ns16550_fns_polled,
#else
      .pDeviceFns = &ns16550_fns,
#endif
      .ulMargin = 16,
      .ulHysteresis = 8,
      .pDeviceParams = (void *) CONSOLE_BAUD,
      .ulCtrlPort1 = BSP_CONSOLE_UART_BASE,
      .ulDataPort = BSP_CONSOLE_UART_BASE,
      .ulIntVector = BSP_CONSOLE_UART_IRQ,
      .getRegister = beagle_uart_get_register,
      .setRegister = beagle_uart_set_register,
      .ulClock = UART_CLOCK,  /* 48MHz base clock */
    },
};

unsigned long Console_Configuration_Count = 1;

static int init_needed = 1; // don't rely on bss being 0

static void beagle_console_init(void)
{
  if(init_needed) {
    const uint32_t div = UART_CLOCK / 16 / CONSOLE_BAUD;
    CONSOLE_SYSC = 2;
    while ((CONSOLE_SYSS & 1) == 0)
      ;
    if ((CONSOLE_LSR & (CONSOLE_LSR_THRE | CONSOLE_LSR_TEMT)) == CONSOLE_LSR_THRE) {
      CONSOLE_LCR = 0x83;
      CONSOLE_DLL = div;
      CONSOLE_DLM = (div >> 8) & 0xff;
      CONSOLE_LCR = 0x03;
      CONSOLE_ACR = 0x00;
    }

    while ((CONSOLE_LSR & CONSOLE_LSR_TEMT) == 0)
      ;

    CONSOLE_LCR = 0x80 | 0x03;
    CONSOLE_DLL = 0x00;
    CONSOLE_DLM = 0x00;
    CONSOLE_LCR = 0x03;
    CONSOLE_MCR = 0x03;
    CONSOLE_FCR = 0x07;
    CONSOLE_LCR = 0x83;
    CONSOLE_DLL = div;
    CONSOLE_DLM = (div >> 8) & 0xff;
    CONSOLE_LCR = 0x03;
    CONSOLE_ACR = 0x00;
    init_needed = 0;
  }
}

#define CONSOLE_THR8 (*(volatile uint8_t *) (BSP_CONSOLE_UART_BASE + 0x00))

static void uart_write_polled( char c )
{
  if(init_needed) beagle_console_init();

  while( ( CONSOLE_LSR & TX_FIFO_E ) == 0 )
    ;
  CONSOLE_THR8 = c;
}

static void _BSP_put_char( char c ) {
   uart_write_polled( c );
}

static int _BSP_get_char(void)
{
  if ((CONSOLE_LSR & CONSOLE_LSR_RDR) != 0) {
    return CONSOLE_RBR;
  } else {
    return -1;
  }
}

BSP_output_char_function_type BSP_output_char = _BSP_put_char;

BSP_polling_getchar_function_type BSP_poll_char = _BSP_get_char;
