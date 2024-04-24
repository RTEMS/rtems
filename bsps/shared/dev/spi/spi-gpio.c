/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceSPIGPIO
 *
 * @brief This file provides the implementation of @ref RTEMSDeviceSPIGPIO.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <dev/spi/spi.h>
#include <dev/spi/spi-gpio.h>

struct spi_gpio_bus {
  spi_bus base;
  const struct spi_gpio_params *p;
};

static int spi_gpio_check_msg(
  struct spi_gpio_bus *bus,
  const spi_ioc_transfer *msg
)
{
  /*
   * Check that only implemented modes are requested.
   *
   * Ignore the frequency. This driver doesn't give any guarantees regarding
   * that.
   */
  if ((msg->mode & ~(SPI_CPHA | SPI_CPOL | SPI_NO_CS)) != 0 ||
      (msg->bits_per_word != 8) ||
      (msg->delay_usecs != 0) ||
      (msg->cs >= SPI_GPIO_MAX_CS) ||
      (bus->p->set_cs[msg->cs] == NULL)) {
    return -EINVAL;
  }

  return 0;
}

static int spi_gpio_transfer_msg(
  struct spi_gpio_bus *bus,
  const spi_ioc_transfer *msg
)
{
  uint8_t cs = msg->cs;
  bool clk_idle = ((msg->mode & SPI_CPOL) != 0);
  bool cpha = ((msg->mode & SPI_CPHA) != 0);
  const uint8_t *tx_buf = msg->tx_buf;
  uint8_t *rx_buf = msg->rx_buf;
  size_t len = msg->len;
  int i;
  int rv;

  rv = spi_gpio_check_msg(bus, msg);
  if (rv != 0) {
    return rv;
  }

  /*
   * Make sure that all chip selects but the one in the message are idle.
   * Excluding the one in the message is necessary to avoid deselecting the chip
   * between messages.
   */
  for (i = 0; i < SPI_GPIO_MAX_CS; ++i) {
    if (i != cs && bus->p->set_cs[i] != NULL) {
      bus->p->set_cs[i](bus->p->set_cs_arg[i], bus->p->cs_idle[i]);
    }
  }

  /* Set up clock according to CPOL. */
  bus->p->set_clk(bus->p->set_clk_arg, clk_idle);

  /* Now select the chip (if it isn't selected from the previous message) */
  bus->p->set_cs[cs](bus->p->set_cs_arg[cs], !bus->p->cs_idle[cs]);

  while (len > 0) {
    uint8_t out = 0xff;
    uint8_t in = 0;
    size_t ctr;

    --len;

    if (tx_buf != NULL) {
      out = *tx_buf;
      ++tx_buf;
    }

    /*
     * Read / Write the data.
     *
     * If CPHA=0: Set up data. Read data. Clock to active. Clock to inactive.
     * If CPHA=1: Clock to active. Set up data. Read data. Clock to inactive.
     */
    for (ctr = 0; ctr < sizeof(out) * 8; ++ctr) {
      bool d_out;
      bool d_in;

      if (cpha) {
        bus->p->set_clk(bus->p->set_clk_arg, !clk_idle);
      }

      d_out = ((out & 0x80) != 0);
      out <<= 1;
      bus->p->set_mosi(bus->p->set_mosi_arg, d_out);

      d_in = bus->p->get_miso(bus->p->get_miso_arg);
      in = (in << 1) | (d_in ? 1 : 0);

      if (!cpha) {
        bus->p->set_clk(bus->p->set_clk_arg, !clk_idle);
      }
      bus->p->set_clk(bus->p->set_clk_arg, clk_idle);
    }

    if (rx_buf != NULL) {
      *rx_buf = in;
      ++rx_buf;
    }
  }

  /* Deselect the chip if the message wants that. */
  if (msg->cs_change) {
    bus->p->set_cs[cs](bus->p->set_cs_arg[cs], bus->p->cs_idle[cs]);
  }

  return 0;
}

static int spi_gpio_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t n
)
{
  struct spi_gpio_bus *bus;
  bus = (struct spi_gpio_bus *) base;
  int rv = 0;

  while (n > 0 && rv == 0) {
    rv = spi_gpio_transfer_msg(bus, msgs);
    --n;
    ++msgs;
  };

  return rv;
}

static void spi_gpio_destroy(spi_bus *base)
{
  struct spi_gpio_bus *bus;
  bus = (struct spi_gpio_bus *) base;
  spi_bus_destroy_and_free(&bus->base);
}

static int spi_gpio_setup(spi_bus *base)
{
  struct spi_gpio_bus *bus;
  bus = (struct spi_gpio_bus *) base;
  struct spi_ioc_transfer msg = {
    .speed_hz = base->speed_hz,
    .delay_usecs = base->delay_usecs,
    .bits_per_word = base->bits_per_word,
    .mode = base->mode,
    .cs = base->cs,
  };

  return spi_gpio_check_msg(bus, &msg);
}

rtems_status_code
spi_gpio_init(const char* device, const struct spi_gpio_params *params)
{
  struct spi_gpio_bus *bus;
  int eno;

  if (device == NULL || params == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  bus = (struct spi_gpio_bus*) spi_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL) {
    return RTEMS_UNSATISFIED;
  }

  bus->p = params;

  bus->base.transfer = spi_gpio_transfer;
  bus->base.destroy = spi_gpio_destroy;
  bus->base.setup = spi_gpio_setup;

  eno = spi_bus_register(&bus->base, device);
  if (eno != 0) {
    spi_bus_destroy_and_free(&bus->base);
    return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}
