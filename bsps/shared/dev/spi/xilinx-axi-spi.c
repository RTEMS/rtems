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
#include <dev/spi/xilinx-axi-spi.h>
#include <dev/spi/xilinx-axi-spi-regs.h>

#define XILINX_AXI_SPI_CS_NONE 0xFF

typedef struct xilinx_axi_spi_bus xilinx_axi_spi_bus;

struct xilinx_axi_spi_bus {
  spi_bus base;
  volatile xilinx_axi_spi *regs;
  uint32_t fifo_size;
  uint32_t num_cs;
  uint32_t msg_todo;
  const spi_ioc_transfer *msg;
  uint32_t todo;
  uint32_t in_transfer;
  uint8_t *rx_buf;
  const uint8_t *tx_buf;
  rtems_id task_id;
  rtems_vector_number irq;
};


static void xilinx_axi_spi_disable_interrupts(volatile xilinx_axi_spi *regs)
{
  regs->globalirq = 0;
  regs->irqenable = 0;
}

static bool xilinx_axi_spi_rx_fifo_not_empty(volatile xilinx_axi_spi *regs)
{
  return (regs->status & XILINX_AXI_SPI_STATUS_RXEMPTY) == 0;
}

static void xilinx_axi_spi_reset(xilinx_axi_spi_bus *bus)
{
  volatile xilinx_axi_spi *regs = bus->regs;
  uint32_t control;

  /* Initiate soft reset for initial state */
  regs->reset = XILINX_AXI_SPI_RESET;

  /* Configure as master */
  control = regs->control;
  control |= XILINX_AXI_SPI_CONTROL_MSTREN;
  regs->control = control;
}

static void xilinx_axi_spi_done(xilinx_axi_spi_bus *bus)
{
  volatile xilinx_axi_spi *regs = bus->regs;
  uint32_t control = regs->control;
  control &= ~XILINX_AXI_SPI_CONTROL_SPIEN;
  regs->control = control;

  xilinx_axi_spi_disable_interrupts(regs);
  rtems_event_transient_send(bus->task_id);
}

static void xilinx_axi_spi_push(xilinx_axi_spi_bus *bus, volatile xilinx_axi_spi *regs)
{
  while (bus->todo > 0 && bus->in_transfer < bus->fifo_size) {
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
xilinx_axi_spi_set_chipsel(xilinx_axi_spi_bus *bus, uint32_t cs)
{

   volatile xilinx_axi_spi *regs = bus->regs;
   uint32_t cs_bit = XILINX_AXI_SPI_CS_NONE;

  if (cs != SPI_NO_CS && cs < bus->num_cs) {
      cs_bit &= ~(1<<cs);
  }
  bus->base.cs = cs;

  regs->cs = cs_bit;
}

static void xilinx_axi_spi_config(
  xilinx_axi_spi_bus *bus,
  volatile xilinx_axi_spi *regs,
  uint32_t mode,
  uint8_t  cs
)
{
  spi_bus *base = &bus->base;
  uint32_t control = regs->control;

  control &= ~XILINX_AXI_SPI_CONTROL_SPIEN;
  regs->control = control;
  
  if ((mode & SPI_CPHA) != 0) {
    control |= XILINX_AXI_SPI_CONTROL_CPHA;
  } else {
    control &= ~XILINX_AXI_SPI_CONTROL_CPHA;
  }

  if ((mode & SPI_CPOL) != 0) {
    control |= XILINX_AXI_SPI_CONTROL_CPOL;
  } else {
    control &= ~XILINX_AXI_SPI_CONTROL_CPOL;
  }

  if ((mode & SPI_LOOP) != 0) {
    control |= XILINX_AXI_SPI_CONTROL_LOOP;
  } else {
    control &= ~XILINX_AXI_SPI_CONTROL_LOOP;
  }

  regs->control = control;
  xilinx_axi_spi_set_chipsel(bus, cs);

  base->mode = mode;
  base->cs = cs;

}

static void
xilinx_axi_spi_next_msg(xilinx_axi_spi_bus *bus, volatile xilinx_axi_spi *regs)
{
    uint32_t control = regs->control;
    control |= XILINX_AXI_SPI_CONTROL_MST_TRANS_INHIBIT
               | XILINX_AXI_SPI_CONTROL_RX_FIFO_RESET
               | XILINX_AXI_SPI_CONTROL_TX_FIFO_RESET;
    regs->control = control;

  if (bus->msg_todo > 0) {
    const spi_ioc_transfer *msg;
    spi_bus *base = &bus->base;

    msg = bus->msg;

    if (
        msg->mode != base->mode
        || msg->cs != base->cs
    ) {
      xilinx_axi_spi_config(
        bus,
        regs,
        msg->mode,
        msg->cs
      );
    }

    if ((msg->mode & SPI_NO_CS) != 0) {
      xilinx_axi_spi_set_chipsel(bus, XILINX_AXI_SPI_CS_NONE);
    }

    bus->todo = msg->len;
    bus->rx_buf = msg->rx_buf;
    bus->tx_buf = msg->tx_buf;
    xilinx_axi_spi_push(bus, regs);
    
    xilinx_axi_spi_disable_interrupts(regs);
    if (
        bus->todo < bus->fifo_size
        || bus->fifo_size == 1) {
        /* if the msg fits into the FIFO, wait for empty TX buffer */
        regs->irqenable = XILINX_AXI_SPI_IRQ_TXEMPTY;

    } else {
        /* if the msg does not fit, refill tx_buf when the tx FIFO is half empty */
        regs->irqenable = XILINX_AXI_SPI_IRQ_TXHALF;
    }
    regs->globalirq = XILINX_AXI_SPI_GLOBAL_IRQ_ENABLE;
    control = regs->control;
    control |= XILINX_AXI_SPI_CONTROL_SPIEN;
    control &= ~XILINX_AXI_SPI_CONTROL_MST_TRANS_INHIBIT;
    regs->control = control;
  } else {
    xilinx_axi_spi_done(bus);
  }
}

static void xilinx_axi_spi_interrupt(void *arg)
{
  xilinx_axi_spi_bus *bus;
  volatile xilinx_axi_spi *regs;

  bus = arg;
  regs = bus->regs;

  /* Clear interrupt flag. It's safe, since only one IRQ active at a time */
  regs->irqstatus = regs->irqenable;

  while (xilinx_axi_spi_rx_fifo_not_empty(regs)) {
    uint32_t val = regs->rxdata;
    if (bus->rx_buf != NULL) {
        *bus->rx_buf = (uint8_t)val;
        ++bus->rx_buf;
    }
    --bus->in_transfer;
  }

  if (bus->todo > 0) {
    xilinx_axi_spi_push(bus, regs);
  } else if (bus->in_transfer > 0) {
    /* Wait until all bytes have been transfered */
    regs->irqenable = XILINX_AXI_SPI_IRQ_TXEMPTY;
  } else {
    --bus->msg_todo;
    ++bus->msg;
    xilinx_axi_spi_next_msg(bus, regs);
  }
}

static int xilinx_axi_spi_check_messages(
  xilinx_axi_spi_bus *bus,
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
        (msg->cs > bus->num_cs)) {
      return -EINVAL;
    }

    ++msg;
    --size;
  }

  return 0;
}

static int xilinx_axi_spi_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t n
)
{
  xilinx_axi_spi_bus *bus;
  int rv;

  bus = (xilinx_axi_spi_bus *) base;

  rv = xilinx_axi_spi_check_messages(bus, msgs, n);

  if (rv == 0) {
    bus->msg_todo = n;
    bus->msg = &msgs[0];
    bus->task_id = rtems_task_self();

    xilinx_axi_spi_next_msg(bus, bus->regs);
    rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    xilinx_axi_spi_set_chipsel(bus, XILINX_AXI_SPI_CS_NONE);
  }
  return rv;
}

static void xilinx_axi_spi_destroy(spi_bus *base)
{
  xilinx_axi_spi_bus *bus;

  bus = (xilinx_axi_spi_bus *) base;
  rtems_interrupt_handler_remove(bus->irq, xilinx_axi_spi_interrupt, bus);
  spi_bus_destroy_and_free(&bus->base);
}

static int xilinx_axi_spi_setup(spi_bus *base)
{
  xilinx_axi_spi_bus *bus;
  uint32_t mode = base->mode;

  bus = (xilinx_axi_spi_bus *) base;

  if (bus->base.bits_per_word > 8) {
    return -EINVAL;
  }

  /* SPI_CS_HIGH not supported */
  if (mode & SPI_CS_HIGH) {
      return -EINVAL;
  }

  xilinx_axi_spi_config(
    bus,
    bus->regs,
    base->mode,
    base->cs
  );
  return 0;
}

int spi_bus_register_xilinx_axi(
        const char *bus_path,
        uintptr_t register_base,
        uint32_t fifo_size,
        uint32_t num_cs,
        rtems_vector_number irq)
{
  xilinx_axi_spi_bus *bus;
  spi_bus *base;
  int sc;

  if (fifo_size != 0 && fifo_size != 16 && fifo_size != 256) {
      return -1;
  }

  if (num_cs > 32) {
      return -1;
  }

  bus = (xilinx_axi_spi_bus *) spi_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL){
    return -1;
  }

  base = &bus->base;
  bus->regs = (volatile xilinx_axi_spi *) register_base;
  bus->irq = irq;
  bus->num_cs = num_cs;

  /* For operation without FIFO set fifo_size to 1
   * so that comparison operators work
   */
  if (fifo_size == 0) {
    bus->fifo_size = 1;
  } else {
    bus->fifo_size = fifo_size;
  }

  base->cs = SPI_NO_CS;

  xilinx_axi_spi_reset(bus);
  xilinx_axi_spi_config(
    bus,
    bus->regs,
    base->mode,
    base->cs
    );


  sc = rtems_interrupt_handler_install(
    bus->irq,
    "XSPI",
    RTEMS_INTERRUPT_UNIQUE,
    xilinx_axi_spi_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    (*bus->base.destroy)(&bus->base);
    rtems_set_errno_and_return_minus_one(EAGAIN);
  }

  bus->base.transfer = xilinx_axi_spi_transfer;
  bus->base.destroy = xilinx_axi_spi_destroy;
  bus->base.setup = xilinx_axi_spi_setup;

  return spi_bus_register(&bus->base, bus_path);
}
