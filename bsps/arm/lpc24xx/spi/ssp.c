/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XXSSP
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2008, 2019 embedded brains GmbH
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

#include <bsp/ssp.h>
#include <bsp.h>
#include <bsp/io.h>
#include <bsp/irq.h>
#include <bsp/lpc24xx.h>

#include <rtems/score/assert.h>

#include <dev/spi/spi.h>

typedef struct {
  spi_bus base;
  volatile lpc24xx_ssp *regs;
  size_t tx_todo;
  const uint8_t *tx_buf;
  size_t tx_inc;
  size_t rx_todo;
  uint8_t *rx_buf;
  size_t rx_inc;
  const spi_ioc_transfer *msg;
  uint32_t msg_todo;
  int msg_error;
  rtems_binary_semaphore sem;
  lpc24xx_module module;
  rtems_vector_number irq;
} lpc24xx_ssp_bus;

typedef struct {
  volatile lpc24xx_ssp *regs;
  lpc24xx_module module;
  rtems_vector_number irq;
} lpc24xx_ssp_config;

static uint8_t lpc24xx_ssp_trash;

static const uint8_t lpc24xx_ssp_idle = 0xff;

static void lpc24xx_ssp_done(lpc24xx_ssp_bus *bus, int error)
{
  bus->msg_error = error;
  rtems_binary_semaphore_post(&bus->sem);
}

static int lpc24xx_ssp_do_setup(
  lpc24xx_ssp_bus *bus,
  uint32_t speed_hz,
  uint32_t mode
)
{
  volatile lpc24xx_ssp *regs;
  uint32_t clk;
  uint32_t scr_plus_one;
  uint32_t cr0;

  if (speed_hz > bus->base.max_speed_hz || speed_hz == 0) {
    return -EINVAL;
  }

  if ((mode & ~(SPI_CPOL | SPI_CPHA)) != 0) {
    return -EINVAL;
  }

  regs = bus->regs;
  clk = bus->base.max_speed_hz;
  scr_plus_one = (clk + speed_hz - 1) / speed_hz;

  if (scr_plus_one > 256) {
    uint32_t pre;

    pre = (scr_plus_one + 255) / 256;

    if (pre <= 127) {
      scr_plus_one = (clk / pre + speed_hz - 1) / speed_hz;
    } else {
      pre = 127;
      scr_plus_one = 256;
    }

    regs->cpsr = 2 * pre;
  }

  cr0 = SET_SSP_CR0_DSS(0, 0x7) | SET_SSP_CR0_SCR(0, scr_plus_one - 1);

  if ((mode & SPI_CPOL) != 0) {
    cr0 |= SSP_CR0_CPOL;
  }

  if ((mode & SPI_CPHA) != 0) {
    cr0 |= SSP_CR0_CPHA;
  }

  regs->cr0 = cr0;

  bus->base.speed_hz = speed_hz;
  bus->base.mode = mode;
  return 0;
}

static bool lpc24xx_ssp_msg_setup(
  lpc24xx_ssp_bus *bus,
  const spi_ioc_transfer *msg
)
{
  if (msg->cs_change == 0 || msg->bits_per_word != 8) {
    lpc24xx_ssp_done(bus, -EINVAL);
    return false;
  }

  if (msg->speed_hz != bus->base.speed_hz || msg->mode != bus->base.mode) {
    int error;

    error = lpc24xx_ssp_do_setup(bus, msg->speed_hz, msg->mode);
    if (error != 0) {
      lpc24xx_ssp_done(bus, error);
      return false;
    }
  }

  bus->tx_todo = msg->len;
  bus->rx_todo = msg->len;

  if (msg->tx_buf != NULL) {
    bus->tx_buf = msg->tx_buf;
    bus->tx_inc = 1;
  } else {
    bus->tx_buf = &lpc24xx_ssp_idle;
    bus->tx_inc = 0;
  }

  if (msg->rx_buf != NULL) {
    bus->rx_buf = msg->rx_buf;
    bus->rx_inc = 1;
  } else {
    bus->rx_buf = &lpc24xx_ssp_trash;
    bus->rx_inc = 0;
  }

  return true;
}

static bool lpc24xx_ssp_do_tx_and_rx(
  lpc24xx_ssp_bus *bus,
  volatile lpc24xx_ssp *regs,
  uint32_t sr
)
{
  size_t tx_todo;
  const uint8_t *tx_buf;
  size_t tx_inc;
  size_t rx_todo;
  uint8_t *rx_buf;
  size_t rx_inc;
  uint32_t imsc;

  tx_todo = bus->tx_todo;
  tx_buf = bus->tx_buf;
  tx_inc = bus->tx_inc;
  rx_todo = bus->rx_todo;
  rx_buf = bus->rx_buf;
  rx_inc = bus->rx_inc;

  while (tx_todo > 0 && (sr & SSP_SR_TNF) != 0) {
    regs->dr = *tx_buf;
    --tx_todo;
    tx_buf += tx_inc;

    if (rx_todo > 0 && (sr & SSP_SR_RNE) != 0) {
      *rx_buf = regs->dr;
      --rx_todo;
      rx_buf += rx_inc;
    }

    sr = regs->sr;
  }

  while (rx_todo > 0 && (sr & SSP_SR_RNE) != 0) {
    *rx_buf = regs->dr;
    --rx_todo;
    rx_buf += rx_inc;

    sr = regs->sr;
  }

  bus->tx_todo = tx_todo;
  bus->tx_buf = tx_buf;
  bus->rx_todo = rx_todo;
  bus->rx_buf = rx_buf;

  imsc = 0;

  if (tx_todo > 0) {
    imsc |= SSP_IMSC_TXIM;
  } else if (rx_todo > 0) {
    imsc |= SSP_IMSC_RXIM | SSP_IMSC_RTIM;
    regs->icr = SSP_ICR_RTRIS;
  }

  regs->imsc = imsc;

  return tx_todo == 0 && rx_todo == 0;
}

static void lpc24xx_ssp_start(
  lpc24xx_ssp_bus *bus,
  const spi_ioc_transfer *msg
)
{
  while (true) {
    if (lpc24xx_ssp_msg_setup(bus, msg)) {
      volatile lpc24xx_ssp *regs;
      uint32_t sr;
      bool next_msg;

      regs = bus->regs;
      sr = regs->sr;

      if ((sr & (SSP_SR_RNE | SSP_SR_TFE)) != SSP_SR_TFE) {
        lpc24xx_ssp_done(bus, -EIO);
        break;
      }

      next_msg = lpc24xx_ssp_do_tx_and_rx(bus, regs, sr);
      if (!next_msg) {
        break;
      }

      --bus->msg_todo;

      if (bus->msg_todo == 0) {
        lpc24xx_ssp_done(bus, 0);
        break;
      }

      ++msg;
      bus->msg = msg;
    } else {
      break;
    }
  }
}

static void lpc24xx_ssp_interrupt(void *arg)
{
  lpc24xx_ssp_bus *bus;
  volatile lpc24xx_ssp *regs;

  bus = arg;
  regs = bus->regs;

  while (true) {
    if (lpc24xx_ssp_do_tx_and_rx(bus, regs, regs->sr)) {
      --bus->msg_todo;

      if (bus->msg_todo > 0) {
        ++bus->msg;

        if (!lpc24xx_ssp_msg_setup(bus, bus->msg)) {
          break;
        }
      } else {
        lpc24xx_ssp_done(bus, 0);
        break;
      }
    } else {
      break;
    }
  }
}

static int lpc24xx_ssp_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t msg_count
)
{
  lpc24xx_ssp_bus *bus;

  if (msg_count == 0) {
    return 0;
  }

  bus = (lpc24xx_ssp_bus *) base;
  bus->msg = msgs;
  bus->msg_todo = msg_count;
  lpc24xx_ssp_start(bus, msgs);
  rtems_binary_semaphore_wait(&bus->sem);

  return bus->msg_error;
}

static void lpc24xx_ssp_destroy(spi_bus *base)
{
  lpc24xx_ssp_bus *bus;
  rtems_status_code sc;

  bus = (lpc24xx_ssp_bus *) base;

  sc = rtems_interrupt_handler_remove(
    bus->irq,
    lpc24xx_ssp_interrupt,
    bus
  );
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  /* Disable SSP module */
  bus->regs->cr1 = 0;

  sc = lpc24xx_module_disable(bus->module);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  rtems_binary_semaphore_destroy(&bus->sem);
  spi_bus_destroy_and_free(&bus->base);
}

static int lpc24xx_ssp_setup(spi_bus *base)
{
  lpc24xx_ssp_bus *bus;

  bus = (lpc24xx_ssp_bus *) base;

  if (bus->base.bits_per_word != 8) {
    return -EINVAL;
  }

  return lpc24xx_ssp_do_setup(bus, bus->base.speed_hz, bus->base.mode);
}

static int lpc24xx_ssp_init(lpc24xx_ssp_bus *bus)
{
  rtems_status_code sc;

  sc = lpc24xx_module_enable(bus->module, LPC24XX_MODULE_PCLK_DEFAULT);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  /* Disable SSP module */
  bus->regs->cr1 = 0;

  sc = rtems_interrupt_handler_install(
    bus->irq,
    "SSP",
    RTEMS_INTERRUPT_UNIQUE,
    lpc24xx_ssp_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return EAGAIN;
  }

  rtems_binary_semaphore_init(&bus->sem, "SSP");

  /* Initialize SSP module */
  bus->regs->dmacr = 0;
  bus->regs->imsc = 0;
  bus->regs->cpsr = 2;
  bus->regs->cr0 = SET_SSP_CR0_DSS(0, 0x7);
  bus->regs->cr1 = SSP_CR1_SSE;

  return 0;
}

static int spi_bus_register_lpc24xx_ssp(
  const char *bus_path,
  const lpc24xx_ssp_config *config
)
{
  lpc24xx_ssp_bus *bus;
  int eno;

  bus = (lpc24xx_ssp_bus *) spi_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL) {
    return -1;
  }

  bus->base.max_speed_hz = LPC24XX_PCLK / 2;
  bus->base.bits_per_word = 8;
  bus->base.speed_hz = bus->base.max_speed_hz;
  bus->regs = config->regs;
  bus->module = config->module;
  bus->irq = config->irq;

  eno = lpc24xx_ssp_init(bus);
  if (eno != 0) {
    (*bus->base.destroy)(&bus->base);
    rtems_set_errno_and_return_minus_one(eno);
  }

  bus->base.transfer = lpc24xx_ssp_transfer;
  bus->base.destroy = lpc24xx_ssp_destroy;
  bus->base.setup = lpc24xx_ssp_setup;

  return spi_bus_register(&bus->base, bus_path);
}

int lpc24xx_register_ssp_0(void)
{
  static const lpc24xx_ssp_config config = {
    .regs = (volatile lpc24xx_ssp *) SSP0_BASE_ADDR,
    .module = LPC24XX_MODULE_SSP_0,
    .irq = LPC24XX_IRQ_SPI_SSP_0
  };

  return spi_bus_register_lpc24xx_ssp(
    LPC24XX_SSP_0_BUS_PATH,
    &config
  );
}

int lpc24xx_register_ssp_1(void)
{
  static const lpc24xx_ssp_config config = {
    .regs = (volatile lpc24xx_ssp *) SSP1_BASE_ADDR,
    .module = LPC24XX_MODULE_SSP_1,
    .irq = LPC24XX_IRQ_SSP_1
  };

  return spi_bus_register_lpc24xx_ssp(
    LPC24XX_SSP_2_BUS_PATH,
    &config
  );
}

#ifdef ARM_MULTILIB_ARCH_V7M
int lpc24xx_register_ssp_2(void)
{
  static const lpc24xx_ssp_config config = {
    .regs = (volatile lpc24xx_ssp *) SSP2_BASE_ADDR,
    .module = LPC24XX_MODULE_SSP_2,
    .irq = LPC24XX_IRQ_SSP_2
  };

  return spi_bus_register_lpc24xx_ssp(
    LPC24XX_SSP_2_BUS_PATH,
    &config
  );
}
#endif
