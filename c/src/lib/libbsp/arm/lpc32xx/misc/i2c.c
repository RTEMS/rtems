/**
 * @file
 *
 * @ingroup lpc32xx_i2c
 *
 * @brief I2C support implementation.
 */

/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>

#include <bsp.h>
#include <bsp/i2c.h>

void lpc32xx_i2c_reset(volatile lpc32xx_i2c *i2c)
{
  i2c->ctrl = I2C_CTRL_RESET;
}

rtems_status_code lpc32xx_i2c_init(
  volatile lpc32xx_i2c *i2c,
  unsigned clock_in_hz
)
{
  uint32_t i2cclk = 0;

  if (i2c == &lpc32xx.i2c_1) {
    i2cclk |= I2CCLK_1_EN | I2CCLK_1_HIGH_DRIVE;
  } else if (i2c == &lpc32xx.i2c_2) {
    i2cclk |= I2CCLK_2_EN | I2CCLK_2_HIGH_DRIVE;
  } else {
    return RTEMS_INVALID_ID;
  }

  LPC32XX_I2CCLK_CTRL |= i2cclk;

  lpc32xx_i2c_reset(i2c);

  return lpc32xx_i2c_clock(i2c, clock_in_hz);
}

rtems_status_code lpc32xx_i2c_clock(
  volatile lpc32xx_i2c *i2c,
  unsigned clock_in_hz
)
{
  uint32_t clk_div = lpc32xx_hclk() / clock_in_hz;
  uint32_t clk_lo = 0;
  uint32_t clk_hi = 0;

  switch (clock_in_hz) {
    case 100000:
      clk_lo = clk_div / 2;
      break;
    case 400000:
      clk_lo = (64 * clk_div) / 100;
      break;
    default:
      return RTEMS_INVALID_CLOCK;
  }

  clk_hi = clk_div - clk_lo;

  i2c->clk_lo = clk_lo;
  i2c->clk_hi = clk_hi;

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code wait_for_transaction_done(volatile lpc32xx_i2c *i2c)
{
  uint32_t stat = 0;

  do {
    stat = i2c->stat;
  } while ((stat & I2C_STAT_TDI) == 0);

  if ((stat & I2C_STAT_TFE) != 0) {
    i2c->stat = I2C_STAT_TDI;

    return RTEMS_SUCCESSFUL;
  } else {
    lpc32xx_i2c_reset(i2c);

    return RTEMS_IO_ERROR;
  }
}

static rtems_status_code tx(volatile lpc32xx_i2c *i2c, uint32_t data)
{
  uint32_t stat = 0;

  do {
    stat = i2c->stat;
  } while ((stat & (I2C_STAT_TFE | I2C_STAT_TDI)) == 0);

  if ((stat & I2C_STAT_TDI) == 0) {
    i2c->rx_or_tx = data;

    return RTEMS_SUCCESSFUL;
  } else {
    lpc32xx_i2c_reset(i2c);

    return RTEMS_IO_ERROR;
  }
}

rtems_status_code lpc32xx_i2c_write_start(
  volatile lpc32xx_i2c *i2c,
  unsigned addr
)
{
  return tx(i2c, I2C_TX_ADDR(addr) | I2C_TX_START);
}

rtems_status_code lpc32xx_i2c_read_start(
  volatile lpc32xx_i2c *i2c,
  unsigned addr
)
{
  return tx(i2c, I2C_TX_ADDR(addr) | I2C_TX_START | I2C_TX_READ);
}

rtems_status_code lpc32xx_i2c_write_with_optional_stop(
  volatile lpc32xx_i2c *i2c,
  const uint8_t *out,
  size_t n,
  bool stop
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t i = 0;

  for (i = 0; i < n - 1 && sc == RTEMS_SUCCESSFUL; ++i) {
    sc = tx(i2c, out [i]);
  }

  if (sc == RTEMS_SUCCESSFUL) {
    uint32_t stop_flag = stop ? I2C_TX_STOP : 0;

    sc = tx(i2c, out [n - 1] | stop_flag);
  }

  if (stop && sc == RTEMS_SUCCESSFUL) {
    sc = wait_for_transaction_done(i2c);
  }

  return sc;
}

static bool can_tx_for_rx(volatile lpc32xx_i2c *i2c)
{
  return (i2c->stat & (I2C_STAT_TFF | I2C_STAT_RFF)) == 0;
}

static bool can_rx(volatile lpc32xx_i2c *i2c)
{
  return (i2c->stat & I2C_STAT_RFE) == 0;
}

rtems_status_code lpc32xx_i2c_read_with_optional_stop(
  volatile lpc32xx_i2c *i2c,
  uint8_t *in,
  size_t n,
  bool stop
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t last = n - 1;
  size_t rx = 0;
  size_t tx = 0;

  if (!stop) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  while (rx <= last) {
    while (tx < last && can_tx_for_rx(i2c)) {
      i2c->rx_or_tx = 0;
      ++tx;
    }

    if (tx == last && can_tx_for_rx(i2c)) {
      uint32_t stop_flag = stop ? I2C_TX_STOP : 0;

      i2c->rx_or_tx = stop_flag;
      ++tx;
    }

    while (rx <= last && can_rx(i2c)) {
      in [rx] = (uint8_t) i2c->rx_or_tx;
      ++rx;
    }
  }

  if (stop) {
    sc = wait_for_transaction_done(i2c);
  }

  return sc;
}

rtems_status_code lpc32xx_i2c_write_and_read(
  volatile lpc32xx_i2c *i2c,
  unsigned addr,
  const uint8_t *out,
  size_t out_size,
  uint8_t *in,
  size_t in_size
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (out_size > 0) {
    bool stop = in_size == 0;

    sc = lpc32xx_i2c_write_start(i2c, addr);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }

    sc = lpc32xx_i2c_write_with_optional_stop(i2c, out, out_size, stop);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }
  }

  if (in_size > 0) {
    sc = lpc32xx_i2c_read_start(i2c, addr);
    if (sc != RTEMS_SUCCESSFUL) {
      return sc;
    }

    lpc32xx_i2c_read_with_optional_stop(i2c, in, in_size, true);
  }

  return RTEMS_SUCCESSFUL;
}
