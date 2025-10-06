/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Modified by Ben Gras <beng@shrike-systems.com> to make
 * interrupt-driven uart i/o work for beagleboards; beaglebone support added.
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
