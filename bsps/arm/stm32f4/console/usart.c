/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012 Sebastian Huber
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

#include <libchip/sersupp.h>

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/rcc.h>
#include <bsp/irq.h>
#include <bsp/usart.h>
#include <bsp/stm32f4.h>
#include <termios.h>
#include <string.h>

static volatile stm32f4_usart *usart_get_regs(const console_tbl *ct)
{
  return (stm32f4_usart *) ct->ulCtrlPort1;
}

#if 0
static rtems_vector_number usart_get_irq_number(const console_tbl *ct)
{
  return ct->ulIntVector;
}
#endif

#ifdef BSP_CONSOLE_USE_INTERRUPTS
/**
 * Read characters in an interrupt
 */
static void stm32f4_usart_interrupt(void *arg)
{
  rtems_termios_tty *tty = (rtems_termios_tty *) arg;
  const console_tbl *ct = Console_Port_Tbl [tty->minor];
  volatile stm32f4_usart *usart = usart_get_regs(ct);

  while ((usart->sr & STM32F4_USART_SR_RXNE) == STM32F4_USART_SR_RXNE)
  {
    char data = STM32F4_USART_DR_GET(usart->dr);
    rtems_termios_enqueue_raw_characters(tty, &data, sizeof(data));
  }
}
#endif

static const stm32f4_rcc_index usart_rcc_index [] = {
  STM32F4_RCC_USART1,
  STM32F4_RCC_USART2,
  STM32F4_RCC_USART3,
  STM32F4_RCC_UART4,
  STM32F4_RCC_UART5,
#ifdef STM32F4_FAMILY_F4XXXX
  STM32F4_RCC_USART6
#endif /* STM32F4_FAMILY_F4XXXX */
};

static stm32f4_rcc_index usart_get_rcc_index(const console_tbl *ct)
{
  return usart_rcc_index [ct->ulCtrlPort2];
}

static const uint8_t usart_pclk_index [] = { 1, 0, 0, 0, 0, 1 };

static const uint32_t usart_pclk_by_index [] = {
  STM32F4_PCLK1,
  STM32F4_PCLK2
};

static uint32_t usart_get_pclk(const console_tbl *ct)
{
  return usart_pclk_by_index [usart_pclk_index [ct->ulCtrlPort2]];
}

static uint32_t usart_get_baud(const console_tbl *ct)
{
  return ct->ulClock;
}

/*
 * a = 8 * (2 - CR1[OVER8])
 *
 * usartdiv = div_mantissa + div_fraction / a
 *
 * baud = pclk / (a * usartdiv)
 *
 * usartdiv = pclk / (a * baud)
 *
 * Calculation in integer arithmetic:
 *
 * 1. div_mantissa = pclk / (a * baud)
 *
 * 2. div_fraction = pclk / (baud - a * div_mantissa)
 */
static uint32_t usart_get_bbr(
  volatile stm32f4_usart *usart,
  uint32_t pclk,
  uint32_t baud
)
{
  uint32_t a = 8 * (2 - ((usart->cr1 & STM32F4_USART_CR1_OVER8) != 0));
  uint32_t div_mantissa_low = pclk / (a * baud);
  uint32_t div_fraction_low = pclk / (baud - a * div_mantissa_low);
  uint32_t div_mantissa_high;
  uint32_t div_fraction_high;
  uint32_t high_err;
  uint32_t low_err;
  uint32_t div_mantissa;
  uint32_t div_fraction;

  if (div_fraction_low < a - 1) {
    div_mantissa_high = div_fraction_low;
    div_fraction_high = div_fraction_low + 1;
  } else {
    div_mantissa_high = div_fraction_low + 1;
    div_fraction_high = 0;
  }

  high_err = pclk - baud * (a * div_mantissa_high + div_fraction_high);
  low_err = baud * (a * div_mantissa_low + div_fraction_low) - pclk;

  if (low_err < high_err) {
    div_mantissa = div_mantissa_low;
    div_fraction = div_fraction_low;
  } else {
    div_mantissa = div_mantissa_high;
    div_fraction = div_fraction_high;
  }

  return STM32F4_USART_BBR_DIV_MANTISSA(div_mantissa)
    | STM32F4_USART_BBR_DIV_FRACTION(div_fraction);
}

static void usart_initialize(int minor)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  volatile stm32f4_usart *usart = usart_get_regs(ct);
  uint32_t pclk = usart_get_pclk(ct);
  uint32_t baud = usart_get_baud(ct);
  stm32f4_rcc_index rcc_index = usart_get_rcc_index(ct);

  stm32f4_rcc_set_clock(rcc_index, true);

  usart->cr1 = 0;
  usart->cr2 = 0;
  usart->cr3 = 0;
  usart->bbr = usart_get_bbr(usart, pclk, baud);
  usart->cr1 = STM32F4_USART_CR1_UE // UART enable
#ifdef BSP_CONSOLE_USE_INTERRUPTS
    | STM32F4_USART_CR1_RXNEIE // RX interrupt
#endif
    | STM32F4_USART_CR1_TE  // TX enable
    | STM32F4_USART_CR1_RE; // RX enable
}

static int usart_first_open(int major, int minor, void *arg)
{
  (void) major;

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  const console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, ct->ulClock);

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  sc = rtems_interrupt_handler_install(ct->ulIntVector,
      ct->sDeviceName,
      RTEMS_INTERRUPT_UNIQUE,
      stm32f4_usart_interrupt,
      tty);
#endif

  return sc;
}

static int usart_last_close(int major, int minor, void *arg)
{
  (void) major;

  rtems_status_code sc = RTEMS_SUCCESSFUL;
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  const console_tbl *ct = Console_Port_Tbl [minor];

  sc = rtems_interrupt_handler_remove(ct->ulIntVector, stm32f4_usart_interrupt, tty);
#else
  (void) minor;
  (void) arg;
#endif
  return sc;
}

#ifndef BSP_CONSOLE_USE_INTERRUPTS
static int usart_read_polled(int minor)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  volatile stm32f4_usart *usart = usart_get_regs(ct);

  if ((usart->sr & STM32F4_USART_SR_RXNE) != 0) {
    return STM32F4_USART_DR_GET(usart->dr);
  } else {
    return -1;
  }
}
#endif

static void usart_write_polled(int minor, char c)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  volatile stm32f4_usart *usart = usart_get_regs(ct);

  while ((usart->sr & STM32F4_USART_SR_TXE) == 0) {
    /* Wait */
  }

  usart->dr = STM32F4_USART_DR(c);
}

static ssize_t usart_write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    usart_write_polled(minor, s [i]);
  }

  return n;
}

/**
 * Configure settings from a termios call to tcsetattr()
 */
static int usart_set_attributes(int minor, const struct termios *term)
{
  console_tbl *ct = Console_Port_Tbl[minor];
  volatile stm32f4_usart *usart = usart_get_regs(ct);
  uint32_t pclk = usart_get_pclk(ct);
  uint32_t baud = term->c_ispeed;

  ct->ulClock = baud;
  usart->bbr = usart_get_bbr(usart, pclk, baud);
  return 0;
}

const console_fns stm32f4_usart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = usart_first_open,
  .deviceLastClose = usart_last_close,
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  .deviceRead = NULL,
#else
  .deviceRead = usart_read_polled,
#endif
  .deviceWrite = usart_write_support_polled,
  .deviceInitialize = usart_initialize,
  .deviceWritePolled = usart_write_polled,
  .deviceSetAttributes = usart_set_attributes,
  .deviceOutputUsesInterrupts = false
};
