/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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
#include <bsp/imx-gpio.h>

#include <chip.h>
#include <dev/spi/spi.h>
#include <fsl_clock.h>
#include <libfdt.h>
#include <imxrt/lpspi.h>

#if IMXRT_LPSPI_MAX_CS != 0 && IMXRT_LPSPI_MAX_CS < 4
#error IMXRT_LPSPI_MAX_CS hast to be either 0 or at least 4.
#endif

struct imxrt_lpspi_bus {
  spi_bus base;
  volatile LPSPI_Type *regs;
  rtems_vector_number irq;
  uint32_t src_clock_hz;
  clock_ip_name_t clock_ip;

  rtems_binary_semaphore sem;
  bool cs_change_on_last_msg;

  uint32_t rx_msg_todo;
  const spi_ioc_transfer *rx_msg;
  size_t remaining_rx_size;
  uint8_t *rx_buf;

  uint32_t tx_msg_todo;
  const spi_ioc_transfer *tx_msg;
  size_t remaining_tx_size;
  const uint8_t *tx_buf;

  uint32_t fifo_size;

#if IMXRT_LPSPI_MAX_CS != 0
  struct {
    bool is_gpio;
    struct imx_gpio_pin gpio;
    uint32_t active;
  } cs[IMXRT_LPSPI_MAX_CS];
  /*
   * dummy_cs is either <0 if no dummy exists or the index of the cs that is
   * used as dummy.
   */
  int dummy_cs;
#endif
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

#if IMXRT_LPSPI_MAX_CS > 0
  if (bus->cs[msg->cs].is_gpio || (msg->mode & SPI_NO_CS) != 0) {
    tcr |= LPSPI_TCR_PCS(bus->dummy_cs);
  } else {
    tcr |= LPSPI_TCR_PCS(msg->cs);
  }
#else
  tcr |= LPSPI_TCR_PCS(msg->cs);
#endif
  tcr |= LPSPI_TCR_CONT_MASK;
  tcr |= LPSPI_TCR_FRAMESZ(word_size-1);

  if (ccr_orig != ccr) {
    regs->CR &= ~LPSPI_CR_MEN_MASK;
    regs->CCR = ccr;
    regs->CR |= LPSPI_CR_MEN_MASK;
  }

  if (bus->cs_change_on_last_msg) {
    /* No CONTC on first write. Otherwise upper 8 bits are not written. */
    regs->TCR = tcr;
  }
  regs->TCR = tcr | LPSPI_TCR_CONTC_MASK;

  bus->cs_change_on_last_msg = msg->cs_change;
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

static void imxrt_lpspi_next_tx_msg(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs
)
{
  if (bus->tx_msg_todo > 0) {
    const spi_ioc_transfer *msg;

    msg = bus->tx_msg;

    imxrt_lpspi_config(bus, regs, msg);
    bus->remaining_tx_size = msg->len;
    bus->tx_buf = msg->tx_buf;
  }
}

static void imxrt_lpspi_fill_tx_fifo(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs
)
{
  while(imxrt_lpspi_tx_fifo_not_full(bus, regs)
      && (bus->tx_msg_todo > 0 || bus->remaining_tx_size > 0)) {
    if (bus->remaining_tx_size > 0) {
      if (bus->remaining_tx_size == 1 && bus->tx_msg->cs_change) {
        /*
         * Necessary for getting the last data out of the Rx FIFO. See "i.MX
         * RT1050 Processor Reference Manual Rev. 4" Chapter 47.3.2.2 "Receive
         * FIFO and Data Match":
         *
         * "During a continuous transfer, if the transmit FIFO is empty, then
         * the receive data is only written to the receive FIFO after the
         * transmit FIFO is written or after the Transmit Command Register (TCR)
         * is written to end the frame."
         */
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
    if (bus->remaining_tx_size == 0) {
      --bus->tx_msg_todo;
      ++bus->tx_msg;
      imxrt_lpspi_next_tx_msg(bus, regs);
    }
  }
}

static void imxrt_lpspi_next_rx_msg(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs
)
{
  if (bus->rx_msg_todo > 0) {
    const spi_ioc_transfer *msg;

    msg = bus->rx_msg;

    bus->remaining_rx_size = msg->len;
    bus->rx_buf = msg->rx_buf;
  }
}

static void imxrt_lpspi_pull_data_from_rx_fifo(
  struct imxrt_lpspi_bus *bus,
  volatile LPSPI_Type *regs
)
{
  uint32_t data;
  while (imxrt_lpspi_rx_fifo_not_empty(regs)
      && (bus->rx_msg_todo > 0 || bus->remaining_rx_size > 0)) {
    if (bus->remaining_rx_size > 0) {
      data = regs->RDR;
      if (bus->rx_buf != NULL) {
        *bus->rx_buf = data;
        ++bus->rx_buf;
      }
      --bus->remaining_rx_size;
    }
    if (bus->remaining_rx_size == 0) {
      --bus->rx_msg_todo;
      ++bus->rx_msg;
      imxrt_lpspi_next_rx_msg(bus, regs);
    }
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

  if (bus->tx_msg_todo > 0 || bus->remaining_tx_size > 0) {
    regs->IER = LPSPI_IER_TDIE_MASK;
  } else if (bus->rx_msg_todo > 0 || bus->remaining_rx_size > 0) {
    regs->IER = LPSPI_IER_RDIE_MASK;
  } else {
    regs->IER = 0;
    rtems_binary_semaphore_post(&bus->sem);
  }
}

static inline int imxrt_lpspi_settings_ok(
  struct imxrt_lpspi_bus *bus,
  const spi_ioc_transfer *msg,
  const spi_ioc_transfer *prev_msg
)
{
  /* most of this is currently just not implemented */
  if (msg->speed_hz > bus->base.max_speed_hz ||
      msg->delay_usecs != 0 ||
      (msg->mode & ~(SPI_CPHA | SPI_CPOL | SPI_LSB_FIRST | SPI_NO_CS)) != 0 ||
      msg->bits_per_word != word_size) {
    return -EINVAL;
  }

#if IMXRT_LPSPI_MAX_CS == 0
  if (msg->cs > 3 || (msg->mode & SPI_NO_CS) != 0) {
    return -EINVAL;
  }
#else /* IMXRT_LPSPI_MAX_CS != 0 */
  /*
   * Chip select is a bit tricky. This depends on whether it's a native or a
   * GPIO chip select.
   */
  if (msg->cs > IMXRT_LPSPI_MAX_CS) {
    return -EINVAL;
  }
  if (!bus->cs[msg->cs].is_gpio && msg->cs > 3) {
    return -EINVAL;
  }
  if ((msg->mode & SPI_NO_CS) != 0 && bus->dummy_cs < 0) {
    return -EINVAL;
  }
#endif

  if (prev_msg != NULL && !prev_msg->cs_change) {
    /*
     * A lot of settings have to be the same in this case because the upper 8
     * bit of TCR can't be changed if it is a continuous transfer.
     */
    if (prev_msg->cs != msg->cs ||
        prev_msg->speed_hz != msg->speed_hz ||
        prev_msg->mode != msg->mode) {
      return -EINVAL;
    }
  }

  return 0;
}

static int imxrt_lpspi_check_messages(
  struct imxrt_lpspi_bus *bus,
  const spi_ioc_transfer *msg,
  uint32_t size
)
{
  const spi_ioc_transfer *prev_msg = NULL;

  while(size > 0) {
    int rv;
    rv = imxrt_lpspi_settings_ok(bus, msg, prev_msg);
    if (rv != 0) {
      return rv;
    }

    prev_msg = msg;
    ++msg;
    --size;
  }

  /*
   * Check whether cs_change is set on last message. Can't work without it
   * because the last received data is only put into the FIFO if it is the end
   * of a transfer or if another TX byte is put into the FIFO.
   *
   * In theory, a GPIO CS wouldn't need that limitation. But handling it
   * different for the GPIO CS would add complexity. So keep it as a driver
   * limitation for now.
   */
  if (!prev_msg->cs_change) {
    return -EINVAL;
  }

  return 0;
}

#if IMXRT_LPSPI_MAX_CS > 0
/*
 * Check how many of the messages can be processed in one go. At the moment it
 * is necessary to pause on CS changes when GPIO CS are used.
 */
static int imxrt_lpspi_check_howmany(
  struct imxrt_lpspi_bus *bus,
  const spi_ioc_transfer *msgs,
  uint32_t max
)
{
  int i;

  if (max == 0) {
    return max;
  }

  for (i = 0; i < max - 1; ++i) {
    const spi_ioc_transfer *msg = &msgs[i];
    const spi_ioc_transfer *next_msg = &msgs[i+1];

    bool cs_is_gpio = bus->cs[msg->cs].is_gpio;
    bool no_cs = msg->mode & SPI_NO_CS;
    bool no_cs_next = next_msg->mode & SPI_NO_CS;

    if (cs_is_gpio && msg->cs_change) {
      break;
    }

    if (no_cs != no_cs_next) {
      break;
    }

    if (cs_is_gpio && (msg->cs != next_msg->cs)) {
      break;
    }
  }

  return i+1;
}
#endif

/*
 * Transfer some messages. CS must not change between messages if GPIO CS are
 * used.
 */
static void imxrt_lpspi_transfer_some(
  struct imxrt_lpspi_bus *bus,
  const spi_ioc_transfer *msgs,
  uint32_t n
)
{
#if IMXRT_LPSPI_MAX_CS > 0
  /*
   * Software chip select. Due to the checks in the
   * imxrt_lpspi_check_messages, the CS can't change in the middle of a
   * transfer. So we can just use the one from the first message.
   */
  if ((msgs[0].mode & SPI_NO_CS) == 0 && bus->cs[msgs[0].cs].is_gpio) {
    imx_gpio_set_output(&bus->cs[msgs[0].cs].gpio, bus->cs[msgs[0].cs].active);
  }
#endif

  bus->tx_msg_todo = n;
  bus->tx_msg = &msgs[0];
  bus->rx_msg_todo = n;
  bus->rx_msg = &msgs[0];
  bus->cs_change_on_last_msg = true;

  imxrt_lpspi_next_rx_msg(bus, bus->regs);
  imxrt_lpspi_next_tx_msg(bus, bus->regs);
  /*
   * Enable the transmit FIFO empty interrupt which will cause an interrupt
   * instantly because there is no data in the transmit FIFO. The interrupt
   * will then fill the FIFO. So nothing else to do here.
   */
  bus->regs->IER = LPSPI_IER_TDIE_MASK;
  rtems_binary_semaphore_wait(&bus->sem);

#if IMXRT_LPSPI_MAX_CS > 0
  if ((msgs[0].mode & SPI_NO_CS) == 0 && bus->cs[msgs[0].cs].is_gpio) {
    imx_gpio_set_output(&bus->cs[msgs[0].cs].gpio, ~bus->cs[msgs[0].cs].active);
  }
#endif
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
#if IMXRT_LPSPI_MAX_CS > 0
    while (n > 0) {
      uint32_t howmany;

      howmany = imxrt_lpspi_check_howmany(bus, msgs, n);
      imxrt_lpspi_transfer_some(bus, msgs, howmany);
      n -= howmany;
      msgs += howmany;
    };
#else
    imxrt_lpspi_transfer_some(bus, msgs, n);
#endif
  };

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

  rv = imxrt_lpspi_settings_ok(bus, &msg, NULL);

  /*
   * Nothing to do besides checking.
   * Every transfer will later overwrite the settings anyway.
   */

  return rv;
}

static uint32_t imxrt_lpspi_get_src_freq(clock_ip_name_t clock_ip)
{
  uint32_t freq;
#if IMXRT_IS_MIMXRT10xx
  uint32_t mux;
  uint32_t divider;

  (void) clock_ip; /* Not necessary for i.MXRT1050 */

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
#elif IMXRT_IS_MIMXRT11xx
  /*
   * FIXME: A future version of the mcux_sdk might provide a better method to
   * get the clock instead of this hack.
   */
  clock_root_t clock_root = clock_ip + kCLOCK_Root_Lpspi1 - kCLOCK_Lpspi1;

  freq = CLOCK_GetRootClockFreq(clock_root);
#else
  #error Getting SPI frequency is not implemented for this chip.
#endif

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
#if IMXRT_LPSPI_MAX_CS != 0
      const uint32_t *val;
#endif

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

#if IMXRT_LPSPI_MAX_CS != 0
      bus->dummy_cs = -1;
      val = fdt_getprop(fdt, node, "num-cs", NULL);
      /* If num-cs is not set: Just assume we only have hardware CS pins */
      if (val != NULL) {
        uint32_t num_cs;
        size_t i;
        int len;
        const uint32_t *val_end;

        num_cs = fdt32_to_cpu(val[0]);
        if (num_cs > IMXRT_LPSPI_MAX_CS) {
          bsp_fatal(IMXRT_FATAL_LPSPI_INVALID_FDT);
        }

        val = fdt_getprop(fdt, node, "cs-gpios", &len);
        if (val == NULL) {
          bsp_fatal(IMXRT_FATAL_LPSPI_INVALID_FDT);
        }
        val_end = val + len;

        for (i = 0; i < num_cs; ++i) {
          if (val >= val_end) {
            /* Already reached the end. But still pins to process. */
            bsp_fatal(IMXRT_FATAL_LPSPI_INVALID_FDT);
          }
          if (fdt32_to_cpu(val[0]) == 0) {
            /* phandle == 0; this is a native CS */
            bus->cs[i].is_gpio = false;
            ++val;
          } else {
            /*
             * phandle is something. Assume an imx_gpio. Other GPIO controllers
             * are not supported.
             */
            rtems_status_code sc;

            if (bus->dummy_cs < 0) {
              bus->dummy_cs = i;
            }
            bus->cs[i].is_gpio = true;
            /*
             * According to Linux device tree documentation, the bit 0 of the
             * flag bitfield in the last cell is 0 for an active high and 1 for
             * an active low pin. Usually the defines GPIO_ACTIVE_HIGH and
             * GPIO_ACTIVE_LOW would be used for that. But we don't have them.
             */
            bus->cs[i].active = (~fdt32_to_cpu(val[2])) & 0x1;
            sc = imx_gpio_init_from_fdt_property_pointer(&bus->cs[i].gpio, val,
                IMX_GPIO_MODE_OUTPUT, &val);
            if (sc != RTEMS_SUCCESSFUL || val > val_end) {
              bsp_fatal(IMXRT_FATAL_LPSPI_INVALID_FDT);
            }

            /* Set to idle state */
            imx_gpio_set_output(&bus->cs[i].gpio, ~bus->cs[i].active);
          }
        }

        /*
         * All pins are processed. Check dummy_cs. If it is still <0, no GPIO is
         * used. That's OK. But if it is set, at least one GPIO CS is set and in
         * this case one of the native CS pins has to be reserved for the
         * dummy_cs.
         */
        if (bus->dummy_cs > 3) {
          bsp_fatal(IMXRT_FATAL_LPSPI_INVALID_FDT);
        }
      }
#endif

      bus->clock_ip = imxrt_lpspi_clock_ip(bus->regs);
      bus->src_clock_hz = imxrt_lpspi_get_src_freq(bus->clock_ip);
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
