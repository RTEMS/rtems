/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <stm32h7/hal.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/fatal.h>
#include <rtems/console.h>

#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

static stm32h7_uart_context * const stm32h7_uart_instances[] = {
#ifdef STM32H7_CONSOLE_ENABLE_USART1
  &stm32h7_usart1_instance,
#endif
#ifdef STM32H7_CONSOLE_ENABLE_USART2
  &stm32h7_usart2_instance,
#endif
#ifdef STM32H7_CONSOLE_ENABLE_USART3
  &stm32h7_usart3_instance,
#endif
#ifdef STM32H7_CONSOLE_ENABLE_UART4
  &stm32h7_uart4_instance,
#endif
#ifdef STM32H7_CONSOLE_ENABLE_UART5
  &stm32h7_uart5_instance,
#endif
#ifdef STM32H7_CONSOLE_ENABLE_USART6
  &stm32h7_usart6_instance,
#endif
#ifdef STM32H7_CONSOLE_ENABLE_UART7
  &stm32h7_uart7_instance,
#endif
#ifdef STM32H7_CONSOLE_ENABLE_UART8
  &stm32h7_uart8_instance,
#endif
#if defined(STM32H7_CONSOLE_ENABLE_UART9) && defined(UART9)
  &stm32h7_uart9_instance,
#endif
#if defined(STM32H7_CONSOLE_ENABLE_USART10) && defined(USART10)
  &stm32h7_usart10_instance
#endif
};

static bool stm32h7_uart_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  stm32h7_uart_context *ctx;
  uint32_t previous_baud;
  uint32_t previous_stop_bits;
  uint32_t previous_parity;
  uint32_t previous_mode;
  HAL_StatusTypeDef status;

  if ((term->c_cflag & CSIZE) != CS8) {
    return false;
  }

  ctx = stm32h7_uart_get_context(base);

  previous_baud = ctx->uart.Init.BaudRate;
  ctx->uart.Init.BaudRate = rtems_termios_baud_to_number(term->c_ospeed);

  previous_stop_bits = ctx->uart.Init.StopBits;
  if ((term->c_cflag & CSTOPB) != 0) {
    ctx->uart.Init.StopBits = UART_STOPBITS_2;
  } else {
    ctx->uart.Init.StopBits = UART_STOPBITS_1;
  }

  previous_parity = ctx->uart.Init.Parity;
  if ((term->c_cflag & PARENB) != 0) {
    if ((term->c_cflag & PARODD) != 0) {
      ctx->uart.Init.Parity = UART_PARITY_ODD;
    } else {
      ctx->uart.Init.Parity = UART_PARITY_EVEN;
    }
  } else {
    ctx->uart.Init.Parity = UART_PARITY_NONE;
  }

  previous_mode = ctx->uart.Init.Mode;
  if ((term->c_cflag & CREAD) != 0) {
    ctx->uart.Init.Mode = UART_MODE_TX_RX;
  } else {
    ctx->uart.Init.Mode = UART_MODE_RX;
  }

  status = UART_SetConfig(&ctx->uart);
  if (status != HAL_OK) {
    ctx->uart.Init.BaudRate = previous_baud;
    ctx->uart.Init.StopBits = previous_stop_bits;
    ctx->uart.Init.Parity = previous_parity;
    ctx->uart.Init.Mode = previous_mode;
    return false;
  }

  return true;
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static void stm32h7_uart_interrupt(void *arg)
{
  rtems_termios_tty *tty;
  rtems_termios_device_context *base;
  stm32h7_uart_context *ctx;
  USART_TypeDef *regs;
  uint32_t isr;

  tty = arg;
  base = rtems_termios_get_device_context(tty);
  ctx = stm32h7_uart_get_context(base);
  regs = ctx->uart.Instance;
  isr = regs->ISR;

  while ((isr & USART_ISR_RXNE_RXFNE) != 0) {
    char c;

    c = (uint8_t) regs->RDR;
    rtems_termios_enqueue_raw_characters(tty, &c, 1);

    isr = regs->ISR;
  }

  if (ctx->transmitting && (isr & USART_ISR_TXE_TXFNF) != 0) {
    rtems_termios_dequeue_characters(tty, 1);
  }

  regs->ICR = USART_ICR_ORECF;
}
#endif

static bool stm32h7_uart_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  stm32h7_uart_context *ctx;
  UART_HandleTypeDef *uart;
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  rtems_status_code sc;
#endif

  ctx = stm32h7_uart_get_context(base);
  uart = &ctx->uart;

  rtems_termios_set_initial_baud(tty, BSP_CONSOLE_BAUD);

  (void) HAL_UART_Init(uart);
  (void) HAL_UARTEx_SetTxFifoThreshold(uart, UART_TXFIFO_THRESHOLD_1_8);
  (void) HAL_UARTEx_SetRxFifoThreshold(uart, UART_RXFIFO_THRESHOLD_1_8);
  (void) HAL_UARTEx_EnableFifoMode(uart);

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  sc = rtems_interrupt_handler_install(
    ctx->config->irq,
    "UART",
    RTEMS_INTERRUPT_SHARED,
    stm32h7_uart_interrupt,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }

  ctx->uart.Instance->CR1 |= USART_CR1_RXNEIE_RXFNEIE;
#endif

  stm32h7_uart_set_attributes(base, term);

  return true;
}

static void stm32h7_uart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  stm32h7_uart_context *ctx;

  ctx = stm32h7_uart_get_context(base);

  (void) rtems_interrupt_handler_remove(
    ctx->config->irq,
    stm32h7_uart_interrupt,
    tty
  );
#endif
}

static void stm32h7_uart_write(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  stm32h7_uart_context *ctx;
  USART_TypeDef *regs;

  ctx = stm32h7_uart_get_context(base);
  regs = ctx->uart.Instance;

  if (len > 0) {
    ctx->transmitting = true;
    regs->TDR = (uint8_t) buf[0];
    regs->CR1 |= USART_CR1_TXEIE_TXFNFIE;
  } else {
    ctx->transmitting = false;
    regs->CR1 &= ~USART_CR1_TXEIE_TXFNFIE;
  }
#else
  size_t i;

  for (i = 0; i < len; ++i) {
    stm32h7_uart_polled_write(base, buf[i]);
  }
#endif
}

static const rtems_termios_device_handler stm32h7_uart_handler = {
  .first_open = stm32h7_uart_first_open,
  .last_close = stm32h7_uart_last_close,
  .write = stm32h7_uart_write,
  .set_attributes = stm32h7_uart_set_attributes,
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  .mode = TERMIOS_IRQ_DRIVEN
#else
  .poll_read = stm32h7_uart_polled_read,
  .mode = TERMIOS_POLLED
#endif
};

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  size_t i;

  rtems_termios_initialize();

  for (i = 0; i < RTEMS_ARRAY_SIZE(stm32h7_uart_instances); ++i) {
    stm32h7_uart_context *ctx;
    char path[sizeof("/dev/ttySXXX")];

    ctx = stm32h7_uart_instances[i];
    snprintf(path, sizeof(path), "/dev/ttyS%" PRIu8, ctx->config->device_index);

    rtems_termios_device_install(
      path,
      &stm32h7_uart_handler,
      NULL,
      &ctx->device
    );

    if (ctx == &STM32H7_PRINTK_INSTANCE) {
      link(path, CONSOLE_DEVICE_NAME);
    }
  }

  return RTEMS_SUCCESSFUL;
}
