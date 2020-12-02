/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2021 Jan Sommer, German Aerospace Center (DLR)
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

#include <rtems/irq-extension.h>
#include <sys/param.h> /* MAX() */

#include <dev/spi/spi.h>
#include <dev/spi/cadence-spi.h>
#include <dev/spi/cadence-spi-regs.h>

#define CADENCE_SPI_FIFO_SIZE 128
#define CADENCE_SPI_MAX_CHIPSELECTS 3
#define CADENCE_SPI_CS_NONE 0xF

typedef struct cadence_spi_bus cadence_spi_bus;

struct cadence_spi_bus {
  spi_bus base;
  volatile cadence_spi *regs;
  uint32_t clk_in;
  uint16_t clk_per_usecs;
  uint32_t msg_todo;
  const spi_ioc_transfer *msg;
  uint32_t todo;
  uint32_t in_transfer;
  uint8_t *rx_buf;
  const uint8_t *tx_buf;
  rtems_id task_id;
  rtems_vector_number irq;
};


static void cadence_spi_disable_interrupts(volatile cadence_spi *regs)
{
  regs->irqdisable = 0xffff;
}

static void cadence_spi_clear_irq_status(volatile cadence_spi *regs)
{
  regs->irqstatus = regs->irqstatus;
}

static bool cadence_spi_rx_fifo_not_empty(volatile cadence_spi *regs)
{
  return (regs->irqstatus & CADENCE_SPI_IXR_RXNEMPTY) != 0;
}

static void cadence_spi_reset(cadence_spi_bus *bus)
{
  volatile cadence_spi *regs = bus->regs;
  uint32_t val;

  cadence_spi_disable_interrupts(regs);

  regs->spienable = 0;

  val = regs->config;
  val &= ~(CADENCE_SPI_CONFIG_MODEFAIL_EN
         | CADENCE_SPI_CONFIG_MANSTRT_EN
         | CADENCE_SPI_CONFIG_MANUAL_CS
         | CADENCE_SPI_CONFIG_PERI_SEL
         | CADENCE_SPI_CONFIG_REF_CLK);

  val |= CADENCE_SPI_CONFIG_CS(CADENCE_SPI_CS_NONE)
    | CADENCE_SPI_CONFIG_MSTREN
    | CADENCE_SPI_CONFIG_MANSTRT;
  regs->config = val;

  /* Initialize here, will be set for every transfer */
  regs->txthreshold = 1;
  /* Set to 1, so we can check when the rx buffer is empty */
  regs->rxthreshold = 1;

  while (cadence_spi_rx_fifo_not_empty(regs)) {
    regs->rxdata;
  }
  cadence_spi_clear_irq_status(regs);
}

static void cadence_spi_done(cadence_spi_bus *bus)
{
  volatile cadence_spi *regs = bus->regs;
  regs->spienable = 0;
  cadence_spi_disable_interrupts(regs);
  cadence_spi_clear_irq_status(regs);
  rtems_event_transient_send(bus->task_id);
}

static void cadence_spi_push(cadence_spi_bus *bus, volatile cadence_spi *regs)
{
  while (bus->todo > 0 && bus->in_transfer < CADENCE_SPI_FIFO_SIZE) {
    uint8_t val = 0;
    if (bus->tx_buf != NULL) {
        val = *bus->tx_buf;
        ++bus->tx_buf;
    }

    --bus->todo;
    regs->txdata = val;
    ++bus->in_transfer;
  }
}

static void
cadence_spi_set_chipsel(cadence_spi_bus *bus, uint32_t cs)
{

   volatile cadence_spi *regs = bus->regs;
   uint32_t cs_bit = CADENCE_SPI_CS_NONE;
   uint32_t config = regs->config;

  if (cs != SPI_NO_CS && cs < CADENCE_SPI_MAX_CHIPSELECTS) {
      cs_bit >>= (CADENCE_SPI_MAX_CHIPSELECTS - cs + 1);
  }
  config = CADENCE_SPI_CONFIG_CS_SET(config, cs_bit);
  bus->base.cs = cs;

  regs->config = config;
}

static uint32_t
cadence_spi_baud_divider(cadence_spi_bus *bus, 
                uint32_t speed_hz)
{
  uint32_t div;
  uint32_t clk_in;

  clk_in = bus->clk_in;

  div = clk_in / speed_hz;
  div = fls((int) div);

  if (clk_in > (speed_hz << div)) {
      ++div;
  }

  /* The baud divider needs to be at least 4, i.e. 2^2 */
  div = MAX(2, div);
  
  /* 0b111 is the maximum possible divider value */
  div =  MIN(7, div-1);
  return div;
}

static void cadence_spi_config(
  cadence_spi_bus *bus,
  volatile cadence_spi *regs,
  uint32_t speed_hz,
  uint32_t mode,
  uint16_t delay_usecs,
  uint8_t  cs
)
{
  spi_bus *base = &bus->base;
  uint32_t config = regs->config;
  uint32_t delay;

  regs->spienable = 0;
  
  if ((mode & SPI_CPHA) != 0) {
    config |= CADENCE_SPI_CONFIG_CLK_PH;
  } else {
    config &= ~CADENCE_SPI_CONFIG_CLK_PH;
  }

  if ((mode & SPI_CPOL) != 0) {
    config |= CADENCE_SPI_CONFIG_CLK_POL;
  } else {
    config &= ~CADENCE_SPI_CONFIG_CLK_POL;
  }

  config = CADENCE_SPI_CONFIG_BAUD_DIV_SET(config,
          cadence_spi_baud_divider(bus, speed_hz));

  regs->config = config;
  cadence_spi_set_chipsel(bus, cs);

  delay = regs->delay;
  delay = CADENCE_SPI_DELAY_DBTWN_SET(delay, delay_usecs * bus->clk_per_usecs);
  regs->delay = delay;

  base->speed_hz = speed_hz;
  base->mode = mode;
  base->cs = cs;

}

static void
cadence_spi_next_msg(cadence_spi_bus *bus, volatile cadence_spi *regs)
{
  if (bus->msg_todo > 0) {
    const spi_ioc_transfer *msg;
    spi_bus *base = &bus->base;

    msg = bus->msg;

    if (
      msg->speed_hz != base->speed_hz
        || msg->mode != base->mode
        || msg->cs != base->cs
    ) {
      cadence_spi_config(
        bus,
        regs,
        msg->speed_hz,
        msg->mode,
        msg->delay_usecs,
        msg->cs
      );
    }

    if ((msg->mode & SPI_NO_CS) != 0) {
      cadence_spi_set_chipsel(bus, CADENCE_SPI_CS_NONE);
    }

    bus->todo = msg->len;
    bus->rx_buf = msg->rx_buf;
    bus->tx_buf = msg->tx_buf;
    cadence_spi_push(bus, regs);
    
    cadence_spi_disable_interrupts(regs);
    if (bus->todo < CADENCE_SPI_FIFO_SIZE) {
        /* if the msg fits into the FIFO for empty TX buffer */
        regs->txthreshold = 1;

    } else {
        /* if the msg does not fit refill tx_buf when the threshold is hit */
        regs->txthreshold = CADENCE_SPI_FIFO_SIZE / 2;
    }
    regs->irqenable = CADENCE_SPI_IXR_TXOW;
    regs->spienable = 1;
  } else {
    cadence_spi_done(bus);
  }
}

static void cadence_spi_interrupt(void *arg)
{
  cadence_spi_bus *bus;
  volatile cadence_spi *regs;

  bus = arg;
  regs = bus->regs;

  /* The RXNEMPTY flag is sometimes not cleared fast
   * enough between 2 reads which could lead to
   * reading an extra byte erroneously. Therefore,
   * also check the in_transfer counter
   */
  while (cadence_spi_rx_fifo_not_empty(regs) 
		 && bus->in_transfer > 0) {
    uint32_t val = regs->rxdata;
    if (bus->rx_buf != NULL) {
        *bus->rx_buf = (uint8_t)val;
        ++bus->rx_buf;
    }
    --bus->in_transfer;
  }

  if (bus->todo > 0) {
    cadence_spi_push(bus, regs);
  } else if (bus->in_transfer > 0) {
    /* Wait until all bytes have been transfered */
    regs->txthreshold = 1;
  } else {
    --bus->msg_todo;
    ++bus->msg;
    cadence_spi_next_msg(bus, regs);
  }
}

static int cadence_spi_check_messages(
  cadence_spi_bus *bus,
  const spi_ioc_transfer *msg,
  uint32_t size)
{
  while(size > 0) {
    if (msg->bits_per_word != 8) {
      return -EINVAL;
    }
    if ((msg->mode &
        ~(SPI_CPHA | SPI_CPOL | SPI_NO_CS)) != 0) {
      return -EINVAL;
    }
    if ((msg->mode & SPI_NO_CS) == 0 &&
        (msg->cs > CADENCE_SPI_MAX_CHIPSELECTS)) {
      return -EINVAL;
    }

    ++msg;
    --size;
  }

  return 0;
}

static int cadence_spi_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t n
)
{
  cadence_spi_bus *bus;
  int rv;

  bus = (cadence_spi_bus *) base;

  rv = cadence_spi_check_messages(bus, msgs, n);

  if (rv == 0) {
    bus->msg_todo = n;
    bus->msg = &msgs[0];
    bus->task_id = rtems_task_self();

    cadence_spi_next_msg(bus, bus->regs);
    rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    cadence_spi_set_chipsel(bus, CADENCE_SPI_CS_NONE);
  }
  return rv;
}

static void cadence_spi_destroy(spi_bus *base)
{
  cadence_spi_bus *bus;

  bus = (cadence_spi_bus *) base;
  rtems_interrupt_handler_remove(bus->irq, cadence_spi_interrupt, bus);
  spi_bus_destroy_and_free(&bus->base);
}

static int cadence_spi_setup(spi_bus *base)
{
  cadence_spi_bus *bus;
  uint32_t mode = base->mode;

  bus = (cadence_spi_bus *) base;

  /* Baud rate divider is at least 4 and at most 256 */
  if (
    base->speed_hz > base->max_speed_hz
      || base->speed_hz < (bus->clk_in / 256)
      || bus->base.bits_per_word > 8
  ) {
    return -EINVAL;
  }

  /* SPI_CS_HIGH and SPI_LOOP not supported */
  if ((mode & SPI_CS_HIGH) || (mode & SPI_LOOP)) {
      return -EINVAL;
  }

  cadence_spi_config(
    bus,
    bus->regs,
    base->speed_hz,
    base->mode,
    base->delay_usecs,
    base->cs
  );
  return 0;
}

int spi_bus_register_cadence(const char *bus_path,
        uintptr_t register_base,
        uint32_t input_clock,
        rtems_vector_number irq)
{
  cadence_spi_bus *bus;
  spi_bus *base;
  int sc;

  bus = (cadence_spi_bus *) spi_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL){
    return -1;
  }

  base = &bus->base;
  bus->regs = (volatile cadence_spi *) register_base;
  bus->clk_in = input_clock;
  bus->clk_per_usecs = input_clock / 1000000;
  bus->irq = irq;

  /* The minimum clock divider is 4 */
  base->max_speed_hz = (input_clock + 3) / 4;
  base->delay_usecs = 0;
  base->speed_hz = base->max_speed_hz;
  base->cs = SPI_NO_CS;

  cadence_spi_reset(bus);
  cadence_spi_config(
    bus,
    bus->regs,
    base->speed_hz,
    base->mode,
    base->delay_usecs,
    base->cs
    );


  sc = rtems_interrupt_handler_install(
    bus->irq,
    "CASPI",
    RTEMS_INTERRUPT_UNIQUE,
    cadence_spi_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    (*bus->base.destroy)(&bus->base);
    rtems_set_errno_and_return_minus_one(EAGAIN);
  }

  bus->base.transfer = cadence_spi_transfer;
  bus->base.destroy = cadence_spi_destroy;
  bus->base.setup = cadence_spi_setup;

  return spi_bus_register(&bus->base, bus_path);
}
