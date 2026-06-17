/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup stm32f4_spi
 * @brief STM32F4 SPI bus driver.
 *
 * Implements the RTEMS SPI bus API (dev/spi/spi.h) for the STM32F4
 * SPI peripheral in interrupt-driven, 8-bit full-duplex master mode.
 *
 * The transfer model follows the STM32F4xx Reference Manual (RM0090)
 * section 28.3.5 (full-duplex transmit/receive procedure for 8-bit
 * frames using TXE and RXNE interrupts).
 *
 * SPI modes 0-3 (all CPOL/CPHA combinations), MSB/LSB-first, and
 * baud-rate selection are supported.  16-bit frames and DMA are not
 * implemented in this initial version.
 */

/*
 * Copyright (C) 2026 Moksh Panicker
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

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/rcc.h>
#include <bsp/stm32f4-spi.h>

#include <dev/spi/spi.h>

#include <errno.h>

/*
 * Internal bus context.  The spi_bus base member must be first so that
 * RTEMS_CONTAINER_OF produces the correct pointer when the framework
 * calls our callbacks with a spi_bus *.
 */
typedef struct {
  spi_bus               base;
  volatile stm32f4_spi *regs;
  rtems_vector_number   irq;
  stm32f4_rcc_index     rcc_index;
  uint32_t              pclk_hz;
  rtems_binary_semaphore sem;
  /* Current transfer state, updated by the ISR */
  const uint8_t        *tx_buf;
  uint8_t              *rx_buf;
  size_t                rx_bytes_remaining;
} stm32f4_spi_bus;

/*
 * Choose the smallest baud-rate prescaler such that fSCK <= speed_hz.
 * The STM32F4 SPI BR field encodes fPCLK / 2^(BR+1), so:
 *   BR=0 -> fPCLK/2, BR=1 -> fPCLK/4, ..., BR=7 -> fPCLK/256.
 * Returns the BR field value [0..7] or -1 if even /256 exceeds speed_hz.
 */
static int stm32f4_spi_find_br(uint32_t pclk_hz, uint32_t speed_hz)
{
  int br;

  if (speed_hz == 0) {
    return -1;
  }

  for (br = 0; br <= 7; ++br) {
    if ((pclk_hz >> (br + 1)) <= speed_hz) {
      return br;
    }
  }

  return -1;
}

/*
 * Interrupt service routine.
 *
 * The STM32F4 SPI has no hardware FIFO — the data register (DR) holds
 * a single byte.  One byte is therefore in flight at a time: TXE fires
 * when DR is empty and we write the next TX byte; RXNE fires when a
 * received byte is available and we read it.  When all bytes have been
 * received we wake the blocked task.  DMA would eliminate the per-byte
 * interrupt overhead but is not implemented in this initial version.
 */
static void stm32f4_spi_isr(void *arg)
{
  stm32f4_spi_bus      *bus  = arg;
  volatile stm32f4_spi *regs = bus->regs;
  uint32_t              sr   = regs->sr;

  if (sr & STM32F4_SPI_SR_TXE) {
    uint8_t byte = 0;
    if (bus->tx_buf != NULL) {
      byte = *bus->tx_buf;
      ++bus->tx_buf;
    }
    regs->dr = byte;
    /*
     * One byte is now in the shift register.  Switch to RXNE: as soon
     * as the received byte is available we read it and immediately put
     * the next TX byte into DR, keeping only one byte in flight.
     */
    regs->cr2 = (regs->cr2 & ~STM32F4_SPI_CR2_TXEIE)
               | STM32F4_SPI_CR2_RXNEIE;
  }

  if (sr & STM32F4_SPI_SR_RXNE) {
    uint8_t byte = (uint8_t) regs->dr;
    if (bus->rx_buf != NULL) {
      *bus->rx_buf = byte;
      ++bus->rx_buf;
    }
    --bus->rx_bytes_remaining;

    if (bus->rx_bytes_remaining == 0) {
      /* All bytes received — disable interrupts and wake the task. */
      regs->cr2 &= ~(STM32F4_SPI_CR2_TXEIE | STM32F4_SPI_CR2_RXNEIE);
      rtems_binary_semaphore_post(&bus->sem);
    } else {
      /*
       * More bytes to go: put the next TX byte into DR immediately.
       * Since only one byte is in flight we can write the next one as
       * soon as we have read the received byte (Christian Mauderer).
       */
      uint8_t next = 0;
      if (bus->tx_buf != NULL) {
        next = *bus->tx_buf;
        ++bus->tx_buf;
      }
      regs->dr = next;
    }
  }
}

static int stm32f4_spi_setup(spi_bus *base)
{
  stm32f4_spi_bus      *bus  = RTEMS_CONTAINER_OF(base, stm32f4_spi_bus, base);
  volatile stm32f4_spi *regs = bus->regs;
  int                   br;
  uint32_t              cr1;

  if (base->bits_per_word != 8) {
    return -EINVAL;
  }

  br = stm32f4_spi_find_br(bus->pclk_hz, base->speed_hz);
  if (br < 0) {
    return -EINVAL;
  }

  /*
   * Build CR1:
   *   MSTR=1   master mode
   *   SSM=1    software-controlled NSS
   *   SSI=1    internal NSS high (we drive CS via GPIO)
   *   SPE=1    enable
   *   CPOL/CPHA from the requested mode
   *   LSBFIRST from the requested mode
   */
  cr1 = STM32F4_SPI_CR1_MSTR
      | STM32F4_SPI_CR1_SSM
      | STM32F4_SPI_CR1_SSI
      | STM32F4_SPI_CR1_BR(br)
      | STM32F4_SPI_CR1_SPE;

  if (base->mode & SPI_CPOL) {
    cr1 |= STM32F4_SPI_CR1_CPOL;
  }
  if (base->mode & SPI_CPHA) {
    cr1 |= STM32F4_SPI_CR1_CPHA;
  }
  if (base->mode & SPI_LSB_FIRST) {
    cr1 |= STM32F4_SPI_CR1_LSBFIRST;
  }

  regs->cr1 = cr1;

  return 0;
}

static int stm32f4_spi_transfer(
  spi_bus              *base,
  const spi_ioc_transfer *msgs,
  uint32_t              n
)
{
  stm32f4_spi_bus      *bus  = RTEMS_CONTAINER_OF(base, stm32f4_spi_bus, base);
  volatile stm32f4_spi *regs = bus->regs;
  uint32_t              i;

  for (i = 0; i < n; ++i) {
    const spi_ioc_transfer *msg = &msgs[i];

    if (msg->bits_per_word != 8 || msg->len == 0) {
      return -EINVAL;
    }

    bus->tx_buf = msg->tx_buf;
    bus->rx_buf = msg->rx_buf;
    bus->rx_bytes_remaining   = msg->len;

    /*
     * Kick off the transfer by writing the first byte to DR and
     * enabling the RXNE interrupt.  The ISR reads the received byte
     * and writes the next TX byte until all bytes are done.
     */
    uint8_t first = 0;
    if (bus->tx_buf != NULL) {
      first = *bus->tx_buf;
      ++bus->tx_buf;
    }
    regs->dr = first;
    regs->cr2 |= STM32F4_SPI_CR2_RXNEIE;
    rtems_binary_semaphore_wait(&bus->sem);

    /* RM0090 28.3.9: wait until BSY=0 before de-asserting CS */
    while (regs->sr & STM32F4_SPI_SR_BSY) {
      /* busy-wait — only a few clock cycles at most */
    }
  }

  return 0;
}

static void stm32f4_spi_destroy(spi_bus *base)
{
  stm32f4_spi_bus      *bus  = RTEMS_CONTAINER_OF(base, stm32f4_spi_bus, base);
  volatile stm32f4_spi *regs = bus->regs;

  /* Disable the peripheral and its clock */
  regs->cr1 &= ~STM32F4_SPI_CR1_SPE;
  stm32f4_rcc_set_clock(bus->rcc_index, false);

  rtems_interrupt_handler_remove(bus->irq, stm32f4_spi_isr, bus);
  rtems_binary_semaphore_destroy(&bus->sem);
  spi_bus_destroy_and_free(base);
}

int stm32f4_spi_init(
  volatile stm32f4_spi *regs,
  rtems_vector_number   irq,
  stm32f4_rcc_index     rcc,
  uint32_t              pclk,
  const char           *path
)
{
  stm32f4_spi_bus   *bus;
  rtems_status_code  sc;
  int                rv;

  bus = (stm32f4_spi_bus *) spi_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL) {
    return ENOMEM;
  }

  bus->regs      = regs;
  bus->irq       = irq;
  bus->rcc_index = rcc;
  bus->pclk_hz   = pclk;

  rtems_binary_semaphore_init(&bus->sem, "STM32F4 SPI");

  /* Enable the peripheral clock before touching registers */
  stm32f4_rcc_set_clock(rcc, true);

  bus->base.bits_per_word = 8;
  bus->base.mode          = 0;
  bus->base.speed_hz      = pclk / 256; /* conservative default */
  bus->base.max_speed_hz  = pclk / 2;
  bus->base.delay_usecs   = 0;
  bus->base.cs            = 0;
  bus->base.cs_change     = 0;

  rv = stm32f4_spi_setup(&bus->base);
  if (rv != 0) {
    rtems_binary_semaphore_destroy(&bus->sem);
    stm32f4_rcc_set_clock(rcc, false);
    spi_bus_destroy_and_free(&bus->base);
    return -rv;
  }

  sc = rtems_interrupt_handler_install(
    irq,
    "STM32F4 SPI",
    RTEMS_INTERRUPT_UNIQUE,
    stm32f4_spi_isr,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_binary_semaphore_destroy(&bus->sem);
    stm32f4_rcc_set_clock(rcc, false);
    spi_bus_destroy_and_free(&bus->base);
    return EIO;
  }

  bus->base.transfer = stm32f4_spi_transfer;
  bus->base.destroy  = stm32f4_spi_destroy;
  bus->base.setup    = stm32f4_spi_setup;

  rv = spi_bus_register(&bus->base, path);
  if (rv != 0) {
    rtems_interrupt_handler_remove(irq, stm32f4_spi_isr, bus);
    rtems_binary_semaphore_destroy(&bus->sem);
    stm32f4_rcc_set_clock(rcc, false);
    spi_bus_destroy_and_free(&bus->base);
    return -rv;
  }

  return 0;
}
