/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Raspberrypi4
 *
 * @brief SPI Driver
 */

/*
 * Copyright (C) 2024 Ning Yang
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

#include <bsp/irq.h>
#include <bsp/raspberrypi.h>
#include <bsp/raspberrypi-spi.h>
#include <bsp/rpi-gpio.h>

#include <dev/spi/spi.h>
#include <bspopts.h>

typedef struct {
  spi_bus base;
  volatile raspberrypi_spi *regs;
  const spi_ioc_transfer *msg;
  uint32_t msg_todo;
  uint8_t *rx_buf;
  const uint8_t *tx_buf;
  uint32_t todo;
  uint8_t num_cs;
  uint32_t in_transfer;
  rtems_id task_id;
  rtems_vector_number irq;
}raspberrypi_spi_bus;

static int raspberrypi_spi_check_msg(
  raspberrypi_spi_bus *bus,
  const spi_ioc_transfer *msg,
  uint32_t n
)
{
  while (n > 0) {
    if (msg->bits_per_word != 8) {
      return -EINVAL;
    }

    if ((msg->mode &
      ~(SPI_CPHA | SPI_CPOL | SPI_NO_CS)) != 0) {
      return -EINVAL;
    }

    if (msg->cs >= bus->num_cs) {
      return -EINVAL;
    }

    ++msg;
    --n;
  }

  return 0;
}

/* Calculates a clock divider to be used with the GPU core clock rate
 * to set a SPI clock rate the closest (<=) to a desired frequency. */
static rtems_status_code rpi_spi_calculate_clock_divider(
  uint32_t clock_hz,
  uint16_t *clock_divider
)
{
  uint16_t divider;
  uint32_t clock_rate;

  /* Calculates an initial clock divider. */
  divider = GPU_CORE_CLOCK_RATE / clock_hz;

  /* Because the divider must be a power of two (as per the BCM2835 datasheet),
   * calculate the next greater power of two. */
  --divider;

  divider |= (divider >> 1);
  divider |= (divider >> 2);
  divider |= (divider >> 4);
  divider |= (divider >> 8);

  ++divider;

  clock_rate = GPU_CORE_CLOCK_RATE / divider;

  /* If the resulting clock rate is greater than the desired frequency,
   * try the next greater power of two divider. */
  while (clock_rate > clock_hz) {
    divider = (divider << 1);

    clock_rate = GPU_CORE_CLOCK_RATE / divider;
  }

  *clock_divider = divider;

  return RTEMS_SUCCESSFUL;
}

static int raspberrypi_spi_config(
  raspberrypi_spi_bus *bus,
  volatile raspberrypi_spi *regs,
  uint32_t speed_hz,
  uint32_t mode,
  uint8_t cs
)
{
  spi_bus *base = &bus->base;
  uint32_t spics = regs->spics;
  rtems_status_code sc;
  uint16_t clock_divider;

  /* Calculate the most appropriate clock divider. */
  sc = rpi_spi_calculate_clock_divider(speed_hz, &clock_divider);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }
  /* Set the bus clock divider. */
  regs->spiclk = RPI_SPICLK_CDIV_SET(regs->spiclk, clock_divider);

  if ((mode & SPI_CPHA) != 0) {
    spics |= RPI_SPICS_CPHA;
  } else {
    spics &= ~RPI_SPICS_CPHA;
  }

  if ((mode & SPI_CPOL) != 0) {
    spics |= RPI_SPICS_CPOL;
  } else {
    spics &= ~RPI_SPICS_CPOL;
  }

  if ((mode & SPI_CS_HIGH) != 0) {
    spics |= RPI_SPICS_CSPOL;
  } else {
    spics &= ~RPI_SPICS_CSPOL;
  }

  spics = RPI_SPICS_CS_SET(spics, cs);

  regs->spics = spics;

  base->speed_hz = speed_hz;
  base->mode = mode;
  base->cs = cs;
  return 0;
}

#ifdef BSP_SPI_USE_INTERRUPTS
static void raspberrypi_spi_done(raspberrypi_spi_bus *bus)
{
  volatile raspberrypi_spi *regs;
  regs = bus->regs;
  regs->spics = regs->spics & ~RPI_SPICS_TA;
  rtems_event_transient_send(bus->task_id);
}

static bool raspberrpi_spi_TX_FULL(volatile raspberrypi_spi *regs)
{
  return !(regs->spics & RPI_SPICS_TXD);
}

static void raspberrypi_spi_push(
  raspberrypi_spi_bus *bus, 
  volatile raspberrypi_spi *regs
)
{
  uint8_t val;
  while (bus->todo > 0 && !raspberrpi_spi_TX_FULL(regs)) {
    val = 0;
    if (bus->tx_buf != NULL) {
        val = *bus->tx_buf;
        ++bus->tx_buf;
    }

    --bus->todo;
    regs->spififo = val;
    ++bus->in_transfer;
  }
}

static void raspberrypi_spi_next_msg(raspberrypi_spi_bus *bus)
{
  const spi_ioc_transfer *msg;
  spi_bus *base;
  volatile raspberrypi_spi *regs;
  regs=bus->regs;

  if (bus->msg_todo > 0) {
    base = &bus->base;
    msg = bus->msg;

    if (
      msg->speed_hz != base->speed_hz
      || msg->mode != base->mode
      || msg->cs != base->cs
      ) {
      raspberrypi_spi_config(
        bus,
        regs,
        msg->speed_hz,
        msg->mode,
        msg->cs
      );
    }

    bus->todo = msg->len;
    bus->rx_buf = msg->rx_buf;
    bus->tx_buf = msg->tx_buf;
    raspberrypi_spi_push(bus, regs);
  } else {
    raspberrypi_spi_done(bus);
  }
}

static void raspberrypi_spi_start(raspberrypi_spi_bus *bus)
{
  volatile raspberrypi_spi *regs;
  regs = bus->regs;

  regs->spics = regs->spics | RPI_SPICS_INTR | RPI_SPICS_INTD;
  /* 
   * Set TA = 1. This will immediately trigger a first interrupt with 
   * DONE = 1. 
   */
  regs->spics = regs->spics | RPI_SPICS_TA;
}

static bool raspberrypi_spi_irq(volatile raspberrypi_spi *regs)
{
  /* Check whether the interrupt is generated by this SPI device */
  if(regs->spics & RPI_SPICS_INTD && regs->spics & RPI_SPICS_DONE) {
    return 1;
  }

  if(regs->spics & RPI_SPICS_INTR && regs->spics & RPI_SPICS_RXR) {
    return 1;
  }

  return 0;
}

static void raspberrypi_spi_interrupt(void *arg)
{
  raspberrypi_spi_bus *bus;
  volatile raspberrypi_spi *regs;
  uint32_t val;
  
  bus = arg;
  regs = bus->regs;

  if (raspberrypi_spi_irq(regs)) {
    
    if (bus->todo > 0) {
      raspberrypi_spi_push(bus, regs);
    } else {
      --bus->msg_todo;
      ++bus->msg;
      raspberrypi_spi_next_msg(bus);
    }

    while (regs->spics & RPI_SPICS_RXD && bus->in_transfer > 0) {
      /*  RX FIFO contains at least 1 byte. */
      val = regs->spififo;
      if (bus->rx_buf != NULL) {
          *bus->rx_buf = (uint8_t)val;
          ++bus->rx_buf;
      }
      --bus->in_transfer;
    }
        
  }
}
#else
static void raspberrypi_spi_polling_tx_rx(raspberrypi_spi_bus *bus)
{
  volatile raspberrypi_spi *regs = bus->regs;

  const unsigned char *sbuffer = bus->tx_buf;
  unsigned char *rbuffer;
  unsigned int size;
  unsigned int read_count, write_count;
  unsigned int data;

  while (bus->msg_todo) {
    rbuffer = bus->rx_buf;
    size = bus->todo;

    regs->spics = regs->spics | RPI_SPICS_CLEAR_RX | RPI_SPICS_CLEAR_TX 
                              | RPI_SPICS_TA;

    read_count = 0;
    write_count = 0;

    while (read_count < size || write_count < size) {
      if (write_count < size && regs->spics & RPI_SPICS_TXD) {
        if (sbuffer) {
          regs->spififo = *sbuffer++;
        } else {
          regs->spififo = 0;
        }

        write_count++;
      }

      if (read_count < size && regs->spics & RPI_SPICS_RXD) {
        data = regs->spififo;

        if (rbuffer) {
          *rbuffer++ = data;
        }

        read_count++;
      }
    }

    while (!(regs->spics & RPI_SPICS_DONE)) {
      /*wait*/
    }
    regs->spics = (regs->spics & ~RPI_SPICS_TA);

    bus->msg_todo--;

    bus->msg++;
    bus->rx_buf = bus->msg->rx_buf;
    bus->tx_buf = bus->msg->tx_buf;
    bus->todo = bus->msg->len;
  }
}

static void raspberrypi_spi_transfer_msg(
  raspberrypi_spi_bus *bus
)
{
  volatile raspberrypi_spi *regs = bus->regs;
  uint32_t msg_todo = bus->msg_todo;
  const spi_ioc_transfer *msg = bus->msg;

  if (msg_todo > 0) {
    if (
      msg->speed_hz != bus->base.speed_hz
      || msg->mode != bus->base.mode
      || msg->cs != bus->base.cs
      ) {
      raspberrypi_spi_config(
        bus,
        regs,
        msg->speed_hz,
        msg->mode,
        msg->cs
      );
    }

    bus->todo = msg->len;
    bus->rx_buf = msg->rx_buf;
    bus->tx_buf = msg->tx_buf;
    raspberrypi_spi_polling_tx_rx(bus);
  }
}
#endif

static int raspberrypi_spi_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t msg_count
)
{
  int rv = 0;
  raspberrypi_spi_bus *bus;
  bus = (raspberrypi_spi_bus *) base;

  rv = raspberrypi_spi_check_msg(bus, msgs, msg_count);
  if (rv == 0) {
    bus->msg_todo = msg_count;
    bus->msg = msgs;
#ifdef BSP_SPI_USE_INTERRUPTS
    bus->task_id = rtems_task_self();
    
    raspberrypi_spi_start(bus);
    rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
#else
    raspberrypi_spi_transfer_msg(bus);
#endif
  }

  return rv;
}

static void raspberrypi_spi_destroy(spi_bus *base)
{
  raspberrypi_spi_bus *bus;
  bus = (raspberrypi_spi_bus *) base;

#ifdef BSP_SPI_USE_INTERRUPTS
  rtems_interrupt_handler_remove(
    bus->irq,
    raspberrypi_spi_interrupt,
    bus
  );
#endif

  spi_bus_destroy_and_free(&bus->base);
}

static int raspberrypi_spi_setup(spi_bus *base)
{
  raspberrypi_spi_bus *bus;
  uint32_t mode = base->mode;

  bus = (raspberrypi_spi_bus *) base;

  if (mode & SPI_LOOP) {
    return -EINVAL;
  }

  return raspberrypi_spi_config(
    bus,
    bus->regs,
    bus->base.speed_hz,
    bus->base.mode,
    bus->base.cs
  );
}

static rtems_status_code raspberrypi_spi_init_gpio(
  raspberrypi_spi_device device
)
{
  switch (device) {
    case raspberrypi_SPI0:
      raspberrypi_gpio_set_function(7, GPIO_AF0);      /* CS1 */
      raspberrypi_gpio_set_pull(7, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(8, GPIO_AF0);      /* CS0 */
      raspberrypi_gpio_set_pull(8, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(9, GPIO_AF0);      /* MISO */
      raspberrypi_gpio_set_function(10, GPIO_AF0);     /* MOSI */
      raspberrypi_gpio_set_function(11, GPIO_AF0);     /* SCLK */
      break;
    case raspberrypi_SPI3:
      raspberrypi_gpio_set_function(24, GPIO_AF5);
      raspberrypi_gpio_set_pull(24, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(0, GPIO_AF3);    
      raspberrypi_gpio_set_pull(0, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(1, GPIO_AF3);      
      raspberrypi_gpio_set_function(2, GPIO_AF3);      
      raspberrypi_gpio_set_function(3, GPIO_AF3);      
      break;
    case raspberrypi_SPI4:
      raspberrypi_gpio_set_function(25, GPIO_AF5);     
      raspberrypi_gpio_set_pull(25, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(4, GPIO_AF3);      
      raspberrypi_gpio_set_pull(4, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(5, GPIO_AF3);      
      raspberrypi_gpio_set_function(6, GPIO_AF3);      
      raspberrypi_gpio_set_function(7, GPIO_AF3);      
      break;
    case raspberrypi_SPI5:
      raspberrypi_gpio_set_function(26, GPIO_AF5);     
      raspberrypi_gpio_set_pull(26, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(12, GPIO_AF3);     
      raspberrypi_gpio_set_pull(12, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(13, GPIO_AF3);     
      raspberrypi_gpio_set_function(14, GPIO_AF3);     
      raspberrypi_gpio_set_function(15, GPIO_AF3);     
      break;
    case raspberrypi_SPI6:
      raspberrypi_gpio_set_function(27, GPIO_AF5);    
      raspberrypi_gpio_set_pull(27, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(18, GPIO_AF3);     
      raspberrypi_gpio_set_pull(18, GPIO_PULL_NONE);
      raspberrypi_gpio_set_function(19, GPIO_AF3);     
      raspberrypi_gpio_set_function(20, GPIO_AF3);     
      raspberrypi_gpio_set_function(21, GPIO_AF3); 
      break;
  default:
    return RTEMS_INVALID_NUMBER;
    break;
  }
  return RTEMS_SUCCESSFUL;
}

rtems_status_code raspberrypi_spi_init(raspberrypi_spi_device device)
{
  raspberrypi_spi_bus *bus;
  int eno;
  volatile raspberrypi_spi *regs;
  const char *bus_path;

  bus = (raspberrypi_spi_bus *) spi_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL) {
    return RTEMS_UNSATISFIED;
  }

  switch (device) {
    case raspberrypi_SPI0:
      regs = (volatile raspberrypi_spi *) BCM2711_SPI0_BASE;
      bus_path = "/dev/spidev0";
      break;
    case raspberrypi_SPI3:
      regs = (volatile raspberrypi_spi *) BCM2711_SPI3_BASE;
      bus_path = "/dev/spidev3";
      break;
    case raspberrypi_SPI4:
      regs = (volatile raspberrypi_spi *) BCM2711_SPI4_BASE;
      bus_path = "/dev/spidev4";
      break;
    case raspberrypi_SPI5:
      regs = (volatile raspberrypi_spi *) BCM2711_SPI5_BASE;
      bus_path = "/dev/spidev5";
      break;
    case raspberrypi_SPI6:
      regs = (volatile raspberrypi_spi *) BCM2711_SPI6_BASE;
      bus_path = "/dev/spidev6";
      break;
    default:
      spi_bus_destroy_and_free(&bus->base);
      return RTEMS_INVALID_NUMBER;
      break;
  }

  bus->regs = regs;
  bus->num_cs = 2;

  bus->base.transfer = raspberrypi_spi_transfer;
  bus->base.destroy = raspberrypi_spi_destroy;
  bus->base.setup = raspberrypi_spi_setup;
  bus->base.bits_per_word = 8;
  bus->base.max_speed_hz = 250000000;
  bus->base.cs = 0;
#ifdef BSP_SPI_USE_INTERRUPTS
  bus->irq = BCM2711_IRQ_SPI;

  eno = rtems_interrupt_handler_install(
    bus->irq,
    "SPI",
    RTEMS_INTERRUPT_SHARED,
    raspberrypi_spi_interrupt,
    bus
  );
  if (eno != RTEMS_SUCCESSFUL) {
    return EAGAIN;
  }
#endif

  eno = spi_bus_register(&bus->base, bus_path);
  if (eno != 0) {
    spi_bus_destroy_and_free(&bus->base);
    return RTEMS_UNSATISFIED;
  }

  eno = raspberrypi_spi_init_gpio(device);
  if (eno != 0) {
    spi_bus_destroy_and_free(&bus->base);
    return RTEMS_INVALID_NUMBER;
  }

  return RTEMS_SUCCESSFUL;
}