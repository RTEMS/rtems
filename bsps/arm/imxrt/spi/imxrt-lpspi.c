/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>

#include <chip.h>
#include <dev/spi/spi.h>
#include <fsl_clock.h>
#include <libfdt.h>
#include <imxrt/lpspi.h>

struct imxrt_lpspi_bus {
  spi_bus base;
  volatile LPSPI_Type *regs;
  rtems_vector_number irq;
  uint32_t src_clock_hz;
  clock_ip_name_t clock_ip;

  uint32_t msg_todo;
  const spi_ioc_transfer *msg;
  rtems_binary_semaphore sem;
  uint32_t tcr;

  size_t remaining_rx_size;
  uint8_t *rx_buf;

  size_t remaining_tx_size;
  const uint8_t *tx_buf;
  uint32_t fifo_size;
};

static const uint32_t word_size = 8;

static unsigned div_round_up(unsigned divident, unsigned divisor)
{
  return (divident + divisor - 1) / divisor;
}

static void imxrt_lpspi_find_clockdivs(
  struct imxrt_lpspi_bus *bus,
  uint32_t max_baud_hz,
  unsigned *sckdiv,
  unsigned *prescale
)
{
  const unsigned max_sckdif = LPSPI_CCR_SCKDIV_MASK >> LPSPI_CCR_SCKDIV_SHIFT;
  const unsigned max_prescale =
      LPSPI_TCR_PRESCALE_MASK >> LPSPI_TCR_PRESCALE_SHIFT;

  unsigned best_baud_hz;
  int best_sckdif;
  int best_prescale;

  int check_baud_hz;
  int check_sckdif;
  int check_prescale;

  /* Start with slowest possible */
  best_sckdif = max_sckdif;
  best_prescale = max_prescale;
  best_baud_hz = div_round_up(bus->src_clock_hz,
      (1 << best_prescale) * (best_sckdif + 2));

  for (check_prescale = 0;
      check_prescale <= max_prescale && best_baud_hz < max_baud_hz;
      ++check_prescale) {

    check_sckdif = div_round_up(bus->src_clock_hz,
        (1 << check_prescale) * max_baud_hz) - 2;

    if (check_sckdif > max_sckdif) {
      check_sckdif = max_sckdif;
    }

    check_baud_hz = div_round_up(bus->src_clock_hz,
        (1 << check_prescale) * (check_sckdif + 2));

    if (check_baud_hz <= max_baud_hz && check_baud_hz > best_baud_hz) {
      best_baud_hz = check_baud_hz;
      best_sckdif = check_sckdif;
      best_prescale = check_prescale;
    }
  }

  *sckdiv = best_sckdif;
  *prescale = best_prescale;
}

static void imxrt_lpspi_config(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs,
  const spi_ioc_transfer *msg
)
{
  uint32_t ccr_orig;
  uint32_t ccr;
  uint32_t tcr;
  unsigned sckdiv;
  unsigned prescale;

  ccr_orig = ccr = regs->CCR;
  tcr = 0;

  imxrt_lpspi_find_clockdivs(bus, msg->speed_hz, &sckdiv, &prescale);

  /* Currently just force half a clock after and before chip select. */
  ccr = LPSPI_CCR_SCKDIV(sckdiv) | LPSPI_CCR_SCKPCS(sckdiv) |
      LPSPI_CCR_PCSSCK(sckdiv) | LPSPI_CCR_DBT(sckdiv);
  tcr |= LPSPI_TCR_PRESCALE(prescale);

  if ((msg->mode & SPI_CPOL) != 0) {
    tcr |= LPSPI_TCR_CPOL_MASK;
  }
  if ((msg->mode & SPI_CPHA) != 0) {
    tcr |= LPSPI_TCR_CPHA_MASK;
  }
  if (msg->mode & SPI_LSB_FIRST) {
    tcr |= LPSPI_TCR_LSBF_MASK;
  }

  tcr |= LPSPI_TCR_PCS(msg->cs);

  if (!msg->cs_change) {
    tcr |= LPSPI_TCR_CONT_MASK;
  }

  tcr |= LPSPI_TCR_FRAMESZ(word_size-1);

  if (ccr_orig != ccr) {
    regs->CR &= ~LPSPI_CR_MEN_MASK;
    regs->CCR = ccr;
    regs->CR |= LPSPI_CR_MEN_MASK;
  }

  /* No CONTC on first write. Otherwise upper 8 bits are not written. */
  regs->TCR = tcr;
  regs->TCR = tcr | LPSPI_TCR_CONTC_MASK | LPSPI_TCR_CONT_MASK;
}

static inline bool imxrt_lpspi_rx_fifo_not_empty(
  volatile LPSPI_Type *regs
)
{
  return ((regs->RSR & LPSPI_RSR_RXEMPTY_MASK) == 0);
}

static inline bool imxrt_lpspi_tx_fifo_not_full(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs
)
{
  /*
   * We might add two things to the FIFO: A TCR and data. Therefore leave one
   * extra space.
   */
  return ((regs->FSR & LPSPI_FSR_TXCOUNT_MASK) >> LPSPI_FSR_TXCOUNT_SHIFT) <
      bus->fifo_size - 2;
}

static void imxrt_lpspi_fill_tx_fifo(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs
)
{
  while(imxrt_lpspi_tx_fifo_not_full(bus, regs)
      && bus->remaining_tx_size > 0) {
    if (bus->remaining_tx_size == 1) {
      regs->TCR &= ~(LPSPI_TCR_CONT_MASK);
    }

    if (bus->tx_buf != NULL) {
      regs->TDR = bus->tx_buf[0];
      ++bus->tx_buf;
    } else {
      regs->TDR = 0;
    }
    --bus->remaining_tx_size;
  }
}

static void imxrt_lpspi_next_msg(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs
)
{
  if (bus->msg_todo > 0) {
    const spi_ioc_transfer *msg;

    msg = bus->msg;

    imxrt_lpspi_config(bus, regs, msg);
    bus->remaining_tx_size = msg->len;
    bus->remaining_rx_size = msg->len;
    bus->rx_buf = msg->rx_buf;
    bus->tx_buf = msg->tx_buf;

    imxrt_lpspi_fill_tx_fifo(bus, regs);
    regs->IER = LPSPI_IER_TDIE_MASK;
  } else {
    regs->IER = 0;
    rtems_binary_semaphore_post(&bus->sem);
  }
}

static void imxrt_lpspi_pull_data_from_rx_fifo(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs
)
{
  while (imxrt_lpspi_rx_fifo_not_empty(regs) && bus->remaining_rx_size > 0) {
    uint32_t data;

    data = regs->RDR;
    if (bus->rx_buf != NULL) {
      *bus->rx_buf = data;
      ++bus->rx_buf;
    }
    --bus->remaining_rx_size;
  }
}

static void imxrt_lpspi_interrupt(void *arg)
{
  struct imxrt_lpspi_bus *bus;
  volatile LPSPI_Type *regs;

  bus = arg;
  regs = bus->regs;

  imxrt_lpspi_pull_data_from_rx_fifo(bus, regs);
  imxrt_lpspi_fill_tx_fifo(bus, regs);

  if (bus->remaining_tx_size == 0) {
    if (bus->remaining_rx_size > 0) {
      regs->IER = LPSPI_IER_RDIE_MASK;
    } else {
      --bus->msg_todo;
      ++bus->msg;
      imxrt_lpspi_next_msg(bus, regs);
    }
  }
}

static inline int imxrt_lpspi_settings_ok(
  struct imxrt_lpspi_bus *bus,
  const spi_ioc_transfer *msg
)
{
  if (msg->cs_change == 0) {
    /*
     * This one most likely would need a bigger workaround if it is necessary.
     * See "i.MX RT1050 Processor Reference Manual Rev. 4" Chapter 47.3.2.2
     * "Receive FIFO and Data Match":
     *
     * "During a continuous transfer, if the transmit FIFO is empty, then the
     * receive data is only written to the receive FIFO after the transmit FIFO
     * is written or after the Transmit Command Register (TCR) is written to end
     * the frame."
     *
     * It might is possible to extend the driver so that it can work with an
     * empty read buffer.
     */
    return -EINVAL;
  }

  /* most of this is currently just not implemented */
  if (msg->cs > 3 ||
      msg->speed_hz > bus->base.max_speed_hz ||
      msg->delay_usecs != 0 ||
      (msg->mode & ~(SPI_CPHA | SPI_CPOL | SPI_LSB_FIRST)) != 0 ||
      msg->bits_per_word != word_size) {
    return -EINVAL;
  }

  return 0;
}

static int imxrt_lpspi_check_messages(
  struct imxrt_lpspi_bus *bus,
  const spi_ioc_transfer *msg,
  uint32_t size
)
{
  while(size > 0) {
    int rv;
    rv = imxrt_lpspi_settings_ok(bus, msg);
    if (rv != 0) {
      return rv;
    }

    ++msg;
    --size;
  }

  return 0;
}

static int imxrt_lpspi_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t n
)
{
  struct imxrt_lpspi_bus *bus;
  int rv;

  bus = (struct imxrt_lpspi_bus *) base;

  rv = imxrt_lpspi_check_messages(bus, msgs, n);

  if (rv == 0) {
    bus->msg_todo = n;
    bus->msg = &msgs[0];

    imxrt_lpspi_next_msg(bus, bus->regs);
    rtems_binary_semaphore_wait(&bus->sem);
  }

  return rv;
}

static void imxrt_lpspi_sw_reset(volatile LPSPI_Type *regs)
{
  regs->CR = LPSPI_CR_RST_MASK | LPSPI_CR_RRF_MASK | LPSPI_CR_RTF_MASK;
  regs->CR = 0;
}

static void imxrt_lpspi_destroy(spi_bus *base)
{
  struct imxrt_lpspi_bus *bus;
  volatile LPSPI_Type *regs;

  bus = (struct imxrt_lpspi_bus *) base;
  regs = bus->regs;
  imxrt_lpspi_sw_reset(regs);

  CLOCK_DisableClock(bus->clock_ip);

  rtems_interrupt_handler_remove(bus->irq, imxrt_lpspi_interrupt, bus);
  spi_bus_destroy_and_free(&bus->base);
}

static int imxrt_lpspi_hw_init(struct imxrt_lpspi_bus *bus)
{
  rtems_status_code sc;
  volatile LPSPI_Type *regs;

  regs = bus->regs;

  CLOCK_EnableClock(bus->clock_ip);

  imxrt_lpspi_sw_reset(regs);

  regs->CFGR1 |= LPSPI_CFGR1_MASTER_MASK;
  regs->FCR = LPSPI_FCR_TXWATER(0) | LPSPI_FCR_RXWATER(0);
  regs->CR |= LPSPI_CR_MEN_MASK;

  bus->fifo_size = 1 << ((regs->PARAM & LPSPI_PARAM_TXFIFO_MASK) >>
      LPSPI_PARAM_TXFIFO_SHIFT);

  sc = rtems_interrupt_handler_install(
    bus->irq,
    "LPSPI",
    RTEMS_INTERRUPT_UNIQUE,
    imxrt_lpspi_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return EAGAIN;
  }

  return 0;
}

static int imxrt_lpspi_setup(spi_bus *base)
{
  struct imxrt_lpspi_bus *bus;
  int rv;
  spi_ioc_transfer msg = {
    .cs_change = base->cs_change,
    .cs = base->cs,
    .bits_per_word = base->bits_per_word,
    .mode = base->mode,
    .speed_hz = base->speed_hz,
    .delay_usecs = base->delay_usecs,
    .rx_buf = NULL,
    .tx_buf = NULL,
  };

  bus = (struct imxrt_lpspi_bus *) base;

  rv = imxrt_lpspi_settings_ok(bus, &msg);

  /*
   * Nothing to do besides checking.
   * Every transfer will later overwrite the settings anyway.
   */

  return rv;
}

static uint32_t imxrt_lpspi_get_src_freq(void)
{
  uint32_t freq;
  uint32_t mux;
  uint32_t divider;

  mux = CLOCK_GetMux(kCLOCK_LpspiMux);

  switch (mux) {
  case 0: /* PLL3 PFD1 */
    freq = CLOCK_GetFreq(kCLOCK_Usb1PllPfd1Clk);
    break;
  case 1: /* PLL3 PFD0 */
    freq = CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk);
    break;
  case 2: /* PLL2 */
    freq = CLOCK_GetFreq(kCLOCK_SysPllClk);
    break;
  case 3: /* PLL2 PFD2 */
    freq = CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk);
    break;
  default:
    freq = 0;
  }

  divider = CLOCK_GetDiv(kCLOCK_LpspiDiv) + 1;
  freq /= divider;

  return freq;
}

static clock_ip_name_t imxrt_lpspi_clock_ip(volatile LPSPI_Type *regs)
{
  LPSPI_Type *const base_addresses[] = LPSPI_BASE_PTRS;
  static const clock_ip_name_t lpspi_clocks[] = LPSPI_CLOCKS;
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE(base_addresses); ++i) {
    if (base_addresses[i] == regs) {
      return lpspi_clocks[i];
    }
  }

  return kCLOCK_IpInvalid;
}

static int imxrt_lpspi_ioctl(spi_bus *base, ioctl_command_t command, void *arg)
{
  struct imxrt_lpspi_bus *bus;
  bus = (struct imxrt_lpspi_bus *) base;
  int err = 0;

  switch (command) {
    case IMXRT_LPSPI_GET_REGISTERS:
      *(volatile LPSPI_Type**)arg = bus->regs;
      break;
    default:
      err = -EINVAL;
      break;
  }

  return err;
}

void imxrt_lpspi_init(void)
{
  const void *fdt;
  int node;

  fdt = bsp_fdt_get();
  node = -1;

  do {
    node = fdt_node_offset_by_compatible(fdt, node, "nxp,imxrt-lpspi");

    if (node >= 0 && imxrt_fdt_node_is_enabled(fdt, node)) {
      struct imxrt_lpspi_bus *bus;
      int eno;
      const char *bus_path;

      bus = (struct imxrt_lpspi_bus*) spi_bus_alloc_and_init(sizeof(*bus));
      if (bus == NULL) {
        bsp_fatal(IMXRT_FATAL_LPSPI_ALLOC_FAILED);
      }

      rtems_binary_semaphore_init(&bus->sem, "LPSPI");

      bus->regs = imx_get_reg_of_node(fdt, node);
      if (bus->regs == NULL) {
        bsp_fatal(IMXRT_FATAL_LPSPI_INVALID_FDT);
      }

      bus->irq = imx_get_irq_of_node(fdt, node, 0);
      if (bus->irq == BSP_INTERRUPT_VECTOR_INVALID) {
        bsp_fatal(IMXRT_FATAL_LPSPI_INVALID_FDT);
      }

      bus_path = fdt_getprop(fdt, node, "rtems,path", NULL);
      if (bus_path == NULL) {
        bsp_fatal(IMXRT_FATAL_LPSPI_INVALID_FDT);
      }

      bus->clock_ip = imxrt_lpspi_clock_ip(bus->regs);
      bus->src_clock_hz = imxrt_lpspi_get_src_freq();
      /* Absolut maximum is 30MHz according to electrical characteristics */
      bus->base.max_speed_hz = MIN(bus->src_clock_hz / 2, 30000000);
      bus->base.delay_usecs = 0;

      eno = imxrt_lpspi_hw_init(bus);
      if (eno != 0) {
        bsp_fatal(IMXRT_FATAL_LPSPI_HW_INIT_FAILED);
      }

      bus->base.transfer = imxrt_lpspi_transfer;
      bus->base.destroy = imxrt_lpspi_destroy;
      bus->base.setup = imxrt_lpspi_setup;
      bus->base.ioctl = imxrt_lpspi_ioctl;

      eno = spi_bus_register(&bus->base, bus_path);
      if (eno != 0) {
        bsp_fatal(IMXRT_FATAL_LPSPI_REGISTER_FAILED);
      }
    }
  } while (node >= 0);
}
