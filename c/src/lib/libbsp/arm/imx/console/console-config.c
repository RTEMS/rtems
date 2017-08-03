/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/sysinit.h>
#include <rtems/termiostypes.h>

#include <bsp.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>

#include <arm/freescale/imx/imx_uartreg.h>

#include <libfdt.h>

typedef struct {
  rtems_termios_device_context base;
  volatile imx_uart *regs;
#ifdef CONSOLE_USE_INTERRUPTS
  rtems_vector_number irq;
  bool transmitting;
#endif
} imx_uart_context;

static imx_uart_context imx_uart_instances[7];

static imx_uart_context *imx_uart_console;

static volatile imx_uart *imx_uart_get_regs(rtems_termios_device_context *base)
{
  imx_uart_context *ctx;

  ctx = (imx_uart_context *) base;
  return ctx->regs;
}

static void imx_uart_write_polled(rtems_termios_device_context *base, char c)
{
  volatile imx_uart *regs;

  regs = imx_uart_get_regs(base);

  while ((regs->usr1 & IMX_UART_USR1_TRDY) == 0) {
    /* Wait */
  }

  regs->utxd = IMX_UART_UTXD_TX_DATA(c);
}

static void imx_output_char(char c)
{
  imx_uart_context *ctx;

  ctx = imx_uart_console;

  if (c == '\n') {
    imx_uart_write_polled(&ctx->base, '\r');
  }

  imx_uart_write_polled(&ctx->base, c);
}

static void imx_uart_init_context(
  imx_uart_context *ctx,
  const char *fdt,
  const char *serial
)
{
  int node;
  int len;
  const uint32_t *val;

  rtems_termios_device_context_initialize(&ctx->base, "UART");

  node = fdt_path_offset(fdt, serial);

  val = fdt_getprop(fdt, node, "reg", &len);
  if (val != NULL && len >= 4) {
    ctx->regs = (imx_uart *) fdt32_to_cpu(val[0]);
  }

#ifdef CONSOLE_USE_INTERRUPTS
  val = fdt_getprop(fdt, node, "interrupts", &len);
  if (val != NULL && len >= 8) {
    ctx->irq = bsp_fdt_map_intr(fdt32_to_cpu(val[1]));
  }
#endif
}

static void imx_uart_probe(void)
{
  const void *fdt;
  int node;
  int offset;
  const char *console;
  size_t i;

  fdt = bsp_fdt_get();
  node = fdt_path_offset(fdt, "/chosen");

  console = fdt_getprop(fdt, node, "stdout-path", NULL);
  if (console == NULL) {
    console = "";
  }

  node = fdt_path_offset(fdt, "/aliases");
  offset = fdt_first_property_offset(fdt, node);
  i = 0;

  while (offset >= 0 && i < RTEMS_ARRAY_SIZE(imx_uart_instances)) {
    const struct fdt_property *prop;

    prop = fdt_get_property_by_offset(fdt, offset, NULL);

    if (prop != NULL) {
      const char *name;

      name = fdt_string(fdt, fdt32_to_cpu(prop->nameoff));
      if (strstr(name, "serial") != NULL) {
        imx_uart_context *ctx;
        const char *serial;

        ctx = &imx_uart_instances[i];
        serial = prop->data;

        if (strcmp(serial, console) == 0) {
          imx_uart_console = ctx;
        }

        imx_uart_init_context(ctx, fdt, serial);
        ++i;
      }
    }

    offset = fdt_next_property_offset(fdt, offset);
  }

  BSP_output_char = imx_output_char;
}

static void imx_output_char_init(char c)
{
  imx_uart_probe();
  imx_output_char(c);
}

BSP_output_char_function_type BSP_output_char = imx_output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;

#ifdef CONSOLE_USE_INTERRUPTS
static void imx_uart_interrupt(void *arg)
{
  rtems_termios_tty *tty;
  imx_uart_context *ctx;
  volatile imx_uart *regs;
  uint32_t usr2;

  tty = arg;
  ctx = rtems_termios_get_device_context(tty);
  regs = ctx->regs;
  usr2 = regs->usr2;

  while ((usr2 & IMX_UART_USR2_RDR) != 0) {
    char c;

    c = IMX_UART_URXD_RX_DATA_GET(regs->urxd);
    rtems_termios_enqueue_raw_characters(tty, &c, 1);
    usr2 = regs->usr2;
  }

  if (ctx->transmitting && (regs->usr1 & IMX_UART_USR1_TRDY) != 0) {
    rtems_termios_dequeue_characters(tty, 1);
  }
}
#endif

static bool imx_uart_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  return true;
}

static bool imx_uart_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  imx_uart_context *ctx;
  volatile imx_uart *regs;
#ifdef CONSOLE_USE_INTERRUPTS
  rtems_status_code sc;
  uint32_t ufcr;
#endif

  ctx = (imx_uart_context *) base;
  regs = imx_uart_get_regs(&ctx->base);

  regs->ucr1 = IMX_UART_UCR1_UARTEN;
  regs->ucr2 = IMX_UART_UCR2_IRTS | IMX_UART_UCR2_WS | IMX_UART_UCR2_RXEN
    | IMX_UART_UCR2_TXEN | IMX_UART_UCR2_SRST;

  rtems_termios_set_initial_baud(tty, 115200);
  imx_uart_set_attributes(base, term);

#ifdef CONSOLE_USE_INTERRUPTS
  ufcr = regs->ufcr;
  ufcr = IMX_UART_UFCR_RXTL_SET(ufcr, 16);
  ufcr = IMX_UART_UFCR_TXTL_SET(ufcr, 16);
  regs->ufcr = ufcr;
  regs->ucr1 |= IMX_UART_UCR1_RRDYEN;
  regs->ucr2 |= IMX_UART_UCR2_ATEN;
  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "UART",
    RTEMS_INTERRUPT_SHARED,
    imx_uart_interrupt,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }
#endif

  return true;
}

static void imx_uart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
#ifdef CONSOLE_USE_INTERRUPTS
  imx_uart_context *ctx;

  ctx = (imx_uart_context *) base;
  rtems_interrupt_handler_remove(ctx->irq, imx_uart_interrupt, tty);
#endif
}

static void imx_uart_write(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
#ifdef CONSOLE_USE_INTERRUPTS
  imx_uart_context *ctx;
  volatile imx_uart *regs;
  uint32_t ucr1;

  ctx = (imx_uart_context *) base;
  regs = imx_uart_get_regs(&ctx->base);
  ucr1 = regs->ucr1;

  if (len > 0) {
    ctx->transmitting = true;
    regs->utxd = IMX_UART_UTXD_TX_DATA(buf[0]);
    ucr1 |= IMX_UART_UCR1_TRDYEN;
  } else {
    ctx->transmitting = false;
    ucr1 &= ~IMX_UART_UCR1_TRDYEN;
  }

  regs->ucr1 = ucr1;
#else
  size_t i;

  for (i = 0; i < len; ++i) {
    imx_uart_write_polled(base, buf[i]);
  }
#endif
}

#ifndef CONSOLE_USE_INTERRUPTS
static int imx_uart_read(rtems_termios_device_context *base)
{
  volatile imx_uart *regs;

  regs = imx_uart_get_regs(base);

  if ((regs->usr2 & IMX_UART_USR2_RDR) != 0) {
    return IMX_UART_URXD_RX_DATA_GET(regs->urxd);
  } else {
    return -1;
  }
}
#endif

static const rtems_termios_device_handler imx_uart_handler = {
  .first_open = imx_uart_first_open,
  .last_close = imx_uart_last_close,
  .write = imx_uart_write,
  .set_attributes = imx_uart_set_attributes,
#ifdef CONSOLE_USE_INTERRUPTS
  .mode = TERMIOS_IRQ_DRIVEN
#else
  .poll_read = imx_uart_read,
  .mode = TERMIOS_POLLED
#endif
};

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  char path[] = "/dev/ttyS?";
  size_t i;

  rtems_termios_initialize();

  for (i = 0; i < RTEMS_ARRAY_SIZE(imx_uart_instances); ++i) {
    imx_uart_context *ctx;

    ctx = &imx_uart_instances[i];
    path[sizeof(path) - 2] = (char) ('0' + i);

    rtems_termios_device_install(
      path,
      &imx_uart_handler,
      NULL,
      &ctx->base
    );

    if (ctx == imx_uart_console) {
      link(path, CONSOLE_DEVICE_NAME);
    }
  }

  return RTEMS_SUCCESSFUL;
}

RTEMS_SYSINIT_ITEM(
  imx_uart_probe,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST
);
