/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <dev/serial/sc16is752.h>

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>

#include <dev/spi/spi.h>

#include "sc16is752-regs.h"

static void msg_init(
  struct spi_ioc_transfer *msg,
  uint8_t cs,
  uint32_t speed,
  size_t len,
  uint8_t *rx_buf,
  const uint8_t *tx_buf
)
{
  msg->rx_buf = rx_buf;
  msg->tx_buf = tx_buf;
  msg->len = len;
  msg->speed_hz = speed;
  msg->delay_usecs = 1;
  msg->bits_per_word = 8;
  msg->cs_change = 0;
  msg->rx_nbits = 0;
  msg->tx_nbits = 0;
  msg->mode = 0;
  msg->cs = cs;
}

static int sc16is752_spi_write_reg(
  sc16is752_context *base,
  uint8_t addr,
  const uint8_t *data,
  size_t len
)
{
  sc16is752_spi_context *ctx = (sc16is752_spi_context *) base;
  spi_ioc_transfer msg[2];
  uint8_t unused[SC16IS752_FIFO_DEPTH];
  uint8_t tx_cmd;

  _Assert(len < RTEMS_ARRAY_SIZE(unused));

  msg_init(&msg[0], ctx->cs, ctx->speed_hz, 1, &unused[0], &tx_cmd);
  msg_init(&msg[1], ctx->cs, ctx->speed_hz, len, &unused[0], &data[0]);

  tx_cmd = addr << 3;
  msg[1].cs_change = 1;

  return ioctl(ctx->fd, SPI_IOC_MESSAGE(2), &msg[0]);
}

static int sc16is752_spi_read_reg(
  sc16is752_context *base,
  uint8_t addr,
  uint8_t *data,
  size_t len
)
{
  sc16is752_spi_context *ctx = (sc16is752_spi_context *) base;
  spi_ioc_transfer msg[2];
  uint8_t unused;
  uint8_t tx_cmd;
  uint8_t tx_buf[SC16IS752_FIFO_DEPTH];

  _Assert(len < RTEMS_ARRAY_SIZE(tx_buf));

  msg_init(&msg[0], ctx->cs, ctx->speed_hz, 1, &unused, &tx_cmd);
  msg_init(&msg[1], ctx->cs, ctx->speed_hz, len, &data[0], &tx_buf[0]);

  tx_cmd = 0x80 | (addr << 3);
  msg[1].cs_change = 1;

  return ioctl(ctx->fd, SPI_IOC_MESSAGE(2), &msg[0]);
}

static int sc16is752_spi_read_2_reg(
  sc16is752_context *base,
  uint8_t addr_0,
  uint8_t addr_1,
  uint8_t data[2]
)
{
  sc16is752_spi_context *ctx = (sc16is752_spi_context *) base;
  spi_ioc_transfer msg[4];
  uint8_t unused;
  uint8_t tx_cmd_0;
  uint8_t tx_cmd_1;

  msg_init(&msg[0], ctx->cs, ctx->speed_hz, 1, &unused, &tx_cmd_0);
  msg_init(&msg[1], ctx->cs, ctx->speed_hz, 1, &data[0], &unused);
  msg_init(&msg[2], ctx->cs, ctx->speed_hz, 1, &unused, &tx_cmd_1);
  msg_init(&msg[3], ctx->cs, ctx->speed_hz, 1, &data[1], &unused);

  tx_cmd_0 = 0x80 | (addr_0 << 3);
  tx_cmd_1 = 0x80 | (addr_1 << 3);
  msg[1].cs_change = 1;
  msg[3].cs_change = 1;

  return ioctl(ctx->fd, SPI_IOC_MESSAGE(4), &msg[0]);
}

static bool sc16is752_spi_first_open(sc16is752_context *base)
{
  sc16is752_spi_context *ctx = (sc16is752_spi_context *) base;
  ctx->fd = open(ctx->spi_path, O_RDWR);
  return ctx->fd >= 0;
}

static void sc16is752_spi_last_close(sc16is752_context *base)
{
  sc16is752_spi_context *ctx = (sc16is752_spi_context *) base;
  close(ctx->fd);
}

rtems_status_code sc16is752_spi_create(
  sc16is752_spi_context *ctx,
  const char *device_path
)
{
  ctx->base.write_reg = sc16is752_spi_write_reg;
  ctx->base.read_reg = sc16is752_spi_read_reg;
  ctx->base.read_2_reg = sc16is752_spi_read_2_reg;
  ctx->base.first_open = sc16is752_spi_first_open;
  ctx->base.last_close = sc16is752_spi_last_close;

  return rtems_termios_device_install(
    device_path,
    &sc16is752_termios_handler,
    NULL,
    &ctx->base.base
  );
}
