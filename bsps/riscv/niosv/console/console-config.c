/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Kevin Kirspel
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

#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/sysinit.h>
#include <rtems/termiostypes.h>

#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/niosv.h>
#include <bsp.h>
#include <bsp/console-termios.h>
#include <rtems/libio.h>

typedef struct {
  rtems_termios_device_context base;
  rtems_termios_tty *tty;
  rtems_vector_number irq;
  size_t out_total;
  size_t out_remaining;
  size_t out_current;
  const char *out_buf;
} jtag_uart_context;

static void jtag_uart_initialize_interrupts(
  struct rtems_termios_tty *tty,
  jtag_uart_context *ctx, void (*isr)(void *)
);
static bool jtag_uart_console_first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base, struct termios *term,
  rtems_libio_open_close_args_t *args
);
static void jtag_uart_close(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
);
static void jtag_uart_console_write_int(
  rtems_termios_device_context *base,
  const char *buf, size_t len
);
static size_t jtag_uart_write_to_fifo(const char *buf, size_t len);
static void jtag_uart_clear_and_set_control(
  jtag_uart_context *ctx,
  uint32_t clear, uint32_t set
);
static void jtag_uart_console_putchar(char c);

static jtag_uart_context jtag_uart_console_instance;

static const rtems_termios_device_handler jtag_uart_console_handler = {
  .first_open = jtag_uart_console_first_open,
  .last_close = jtag_uart_close,
  .poll_read = NULL,
  .write = jtag_uart_console_write_int,
  .set_attributes = NULL,
  .mode = TERMIOS_IRQ_DRIVEN
};

static void jtag_uart_clear_and_set_control(
  jtag_uart_context *ctx,
  uint32_t clear,
  uint32_t set
)
{
  volatile altera_avalon_jtag_uart_regs *ajur = JTAG_UART_REGS;
  rtems_interrupt_lock_context lock_context;
  uint8_t val;

  rtems_termios_device_lock_acquire(&ctx->base, &lock_context);
  val = ajur->control;
  val &= ~clear;
  val |= set;
  ajur->control = val;
  rtems_termios_device_lock_release(&ctx->base, &lock_context);
}

static size_t jtag_uart_write_to_fifo(const char *buf, size_t len)
{
  volatile altera_avalon_jtag_uart_regs *ajur = JTAG_UART_REGS;
  uint32_t space = (
    ajur->control & ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK
  ) >> ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST;
  size_t out = len > space ? space : len;
  size_t i;

  for (i = 0; i < out; ++i) {
    ajur->data = buf[i];
  }

  return out;
}

static void jtag_uart_isr(void *arg)
{
  rtems_termios_tty *tty = arg;
  jtag_uart_context *ctx = rtems_termios_get_device_context(tty);
  volatile altera_avalon_jtag_uart_regs *ajur = JTAG_UART_REGS;
  int i = 0;
  char buf [JTAG_UART_READ_DEPTH];

  /* Iterate until no more interrupts are pending */
  do {
    if (ajur->control & ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK) {
      /* Fetch received characters */
      for (i = 0; i < JTAG_UART_READ_DEPTH; ++i) {
        uint32_t data = ajur->data;
        if ((data & ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK) == 0) {
          break;
        } else {
          buf[i] = (char) data;
        }
      }

      /* Enqueue fetched characters */
      rtems_termios_enqueue_raw_characters(tty, buf, i);
    }

    /* Do transmit */
    if (ctx->out_total > 0 &&
      ajur->control & ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK
    ) {
      size_t current = ctx->out_current;

      ctx->out_buf += current;
      ctx->out_remaining -= current;

      if(ctx->out_remaining > 0) {
        ctx->out_current =
          jtag_uart_write_to_fifo(ctx->out_buf, ctx->out_remaining);
      } else {
        rtems_termios_dequeue_characters(tty, ctx->out_total);
      }
    }
  } while((ajur->control & (ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK |
    ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK)) != 0);
}

static void jtag_uart_console_putchar(char c)
{
  volatile altera_avalon_jtag_uart_regs *ajur = JTAG_UART_REGS;

  /*
   *  Wait for the transmitter to be ready.
   *  Check for flow control requests and process.
   *  Then output the character.
   */
  while ((ajur->control & ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK) == 0);

  ajur->data = c;
}

static void jtag_uart_console_probe(void)
{
  rtems_termios_device_context_initialize(
    &jtag_uart_console_instance.base,
    "JURT"
  );

  jtag_uart_console_instance.irq =
    NIOSV_INTERRUPT_VECTOR_EXTERNAL(JTAG_UART_IRQ);
}

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  rtems_termios_device_context *base;
  char jtag_uart_path[] = "/dev/ttyJtagUart";

  rtems_termios_initialize();

  base = &jtag_uart_console_instance.base;
  rtems_termios_device_install(
    jtag_uart_path,
    &jtag_uart_console_handler,
    NULL,
    base
  );

  link(jtag_uart_path, CONSOLE_DEVICE_NAME);

  return RTEMS_SUCCESSFUL;
}

static void jtag_uart_initialize_interrupts(
  struct rtems_termios_tty *tty,
  jtag_uart_context *ctx,
  void (*isr)(void *)
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "JTAG_UART",
    RTEMS_INTERRUPT_UNIQUE,
    isr,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    printk( "%s: Error: Install interrupt handler\n", __func__);
    rtems_fatal_error_occurred(0xdeadbeef);
  }
  /* Enable JUART interrupts */
  alt_irq_enable(JTAG_UART_IRQ);
}

static void jtag_uart_cleanup_interrupts(
  struct rtems_termios_tty *tty,
  jtag_uart_context *ctx,
  void (*isr)(void *)
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* Disable JUART interrupts */
  alt_irq_disable(JTAG_UART_IRQ);

  sc = rtems_interrupt_handler_remove(
    ctx->irq,
    isr,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    /* FIXME */
    printk("%s: Error: Remove interrupt handler\n", __func__);
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static bool jtag_uart_console_first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) term;
  (void) args;

  jtag_uart_context *ctx = (jtag_uart_context *)base;

  ctx->tty = tty;

  if (tty->handler.mode == TERMIOS_IRQ_DRIVEN) {
    jtag_uart_initialize_interrupts(tty, ctx, jtag_uart_isr);
    jtag_uart_clear_and_set_control(
      ctx,
      ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK,
      ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK
    );
  }

  return true;
}

static void jtag_uart_close(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;

  jtag_uart_context *ctx = (jtag_uart_context *) base;

  /* disable interrupts */
  jtag_uart_clear_and_set_control(
    ctx,
    ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK |
    ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK,
    0
  );

  if (tty->handler.mode == TERMIOS_IRQ_DRIVEN) {
    jtag_uart_cleanup_interrupts(tty, ctx, jtag_uart_isr);
  }
}

static void jtag_uart_console_write_int(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  jtag_uart_context *ctx = (jtag_uart_context *) base;

  ctx->out_total = len;

  if (len > 0) {
    ctx->out_remaining = len;
    ctx->out_buf = buf;
    ctx->out_current = jtag_uart_write_to_fifo(buf, len);

    jtag_uart_clear_and_set_control(
      ctx,
      0,
      ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK
    );
  } else {
    jtag_uart_clear_and_set_control(
      ctx,
      ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK,
      0
    );
  }
}

static void jtag_uart_output_char_init(char c)
{
  BSP_output_char = jtag_uart_console_putchar;
  jtag_uart_console_putchar(c);
}

BSP_output_char_function_type BSP_output_char = jtag_uart_output_char_init;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

RTEMS_SYSINIT_ITEM(
  jtag_uart_console_probe,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
