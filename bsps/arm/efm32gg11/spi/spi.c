/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 SPI Driver
 */

/*
 * Copyright (C) 2025 Allan N. Hessenflow
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
#include <bsp/processor.h>
#include <bsp/efm32gg11_clock.h>
#include <bsp/efm32gg11_gpio.h>
#include <bsp/efm32gg11_dma.h>
#include <bsp/spi.h>
#include <unistd.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <dev/spi/spi.h>


#define PASTE2(a,b)          a ## b
#define PASTE3(a,b,c)        a ## b ## c
#define PASTE4(a,b,c,d)      a ## b ## c ## d

#define REGS(i)              PASTE2(USART, i)
#define TX_DMA_REQ(i)        PASTE3(DMAREQ_USART, i, _TXBL)
#define RX_DMA_REQ(i)        PASTE3(DMAREQ_USART, i, _RXDATAV)
#define CLK_PORT(i, l)       PASTE3(AF_USART, i, _CLK_PORT)(l)
#define CLK_PIN(i, l)        PASTE3(AF_USART, i, _CLK_PIN)(l)
#define TX_PORT(i, l)        PASTE3(AF_USART, i, _TX_PORT)(l)
#define TX_PIN(i, l)         PASTE3(AF_USART, i, _TX_PIN)(l)
#define RX_PORT(i, l)        PASTE3(AF_USART, i, _RX_PORT)(l)
#define RX_PIN(i, l)         PASTE3(AF_USART, i, _RX_PIN)(l)
#define HFPERCLKEN0_MASK(i)  PASTE2(CMU_HFPERCLKEN0_USART, i)


struct spi_cs_pin_s {
  uint8_t port;
  uint8_t pin;
  uint8_t high_true;
  uint8_t cs_setup_bits;
};

#if defined(EFM32GG11_SPI_0_INDEX) && EFM32GG11_SPI_0_INDEX >= 0
static struct spi_cs_pin_s spi_0_cs_pins[] = EFM32GG11_SPI_0_CS_PINS;
#endif
#if defined(EFM32GG11_SPI_1_INDEX) && EFM32GG11_SPI_1_INDEX >= 0
static struct spi_cs_pin_s spi_1_cs_pins[] = EFM32GG11_SPI_1_CS_PINS;
#endif
#if defined(EFM32GG11_SPI_2_INDEX) && EFM32GG11_SPI_2_INDEX >= 0
static struct spi_cs_pin_s spi_2_cs_pins[] = EFM32GG11_SPI_2_CS_PINS;
#endif
#if defined(EFM32GG11_SPI_3_INDEX) && EFM32GG11_SPI_3_INDEX >= 0
static struct spi_cs_pin_s spi_3_cs_pins[] = EFM32GG11_SPI_3_CS_PINS;
#endif
#if defined(EFM32GG11_SPI_4_INDEX) && EFM32GG11_SPI_4_INDEX >= 0
static struct spi_cs_pin_s spi_4_cs_pins[] = EFM32GG11_SPI_4_CS_PINS;
#endif
#if defined(EFM32GG11_SPI_5_INDEX) && EFM32GG11_SPI_5_INDEX >= 0
static struct spi_cs_pin_s spi_5_cs_pins[] = EFM32GG11_SPI_5_CS_PINS;
#endif

static struct spi_context_s {
  spi_bus bus; /* must be first */
  const char *name;
  const uint8_t *tx_ptr;
  uint8_t *rx_ptr;
  uint32_t xfer_count;
  rtems_binary_semaphore sem;
  USART_TypeDef *regs;
  int tx_dma_channel;
  int rx_dma_channel;
  uint32_t tx_dma_req;
  uint32_t rx_dma_req;
  struct spi_cs_pin_s *cs_pins;
  int cs_pin_count;
  int clk_pin_loc;
  int clk_port;
  int clk_pin;
  int tx_pin_loc;
  int tx_port;
  int tx_pin;
  int rx_pin_loc;
  int rx_port;
  int rx_pin;
  uint32_t hfperclken0_mask;
  uint32_t routeloc0;
  uint32_t routepen;
  uint32_t tx_dummy;
  uint32_t rx_dummy;
} spi_instances[] = {
#if defined(EFM32GG11_SPI_0_INDEX) && EFM32GG11_SPI_0_INDEX >= 0
  {
    .name = "/dev/spi-0",
    .sem = RTEMS_BINARY_SEMAPHORE_INITIALIZER("spi0"),
    .regs = REGS(EFM32GG11_SPI_0_INDEX),
    .tx_dma_channel = EFM32GG11_SPI_0_TX_DMA_CHANNEL,
    .rx_dma_channel = EFM32GG11_SPI_0_RX_DMA_CHANNEL,
    .tx_dma_req = TX_DMA_REQ(EFM32GG11_SPI_0_INDEX),
    .rx_dma_req = RX_DMA_REQ(EFM32GG11_SPI_0_INDEX),
    .cs_pins = spi_0_cs_pins,
    .cs_pin_count = sizeof(spi_0_cs_pins) / sizeof(spi_0_cs_pins[0]),
    .clk_pin_loc = EFM32GG11_SPI_0_CLK_LOC,
    .clk_port = CLK_PORT(EFM32GG11_SPI_0_INDEX, EFM32GG11_SPI_0_CLK_LOC),
    .clk_pin = CLK_PIN(EFM32GG11_SPI_0_INDEX, EFM32GG11_SPI_0_CLK_LOC),
    .tx_pin_loc = EFM32GG11_SPI_0_TX_LOC,
    .tx_port = TX_PORT(EFM32GG11_SPI_0_INDEX, EFM32GG11_SPI_0_TX_LOC),
    .tx_pin = TX_PIN(EFM32GG11_SPI_0_INDEX, EFM32GG11_SPI_0_TX_LOC),
    .rx_pin_loc = EFM32GG11_SPI_0_RX_LOC,
    .rx_port = RX_PORT(EFM32GG11_SPI_0_INDEX, EFM32GG11_SPI_0_RX_LOC),
    .rx_pin = RX_PIN(EFM32GG11_SPI_0_INDEX, EFM32GG11_SPI_0_RX_LOC),
    .hfperclken0_mask = HFPERCLKEN0_MASK(EFM32GG11_SPI_0_INDEX)
  },
#endif
};


static void xfer_start(struct spi_context_s *ctx)
{
  uint32_t count;
  int bytes_per_transfer;
  uint32_t dma_ctrl_base;

  count = ctx->xfer_count;
  if (count > (_LDMA_CH_CTRL_XFERCNT_MASK >> _LDMA_CH_CTRL_XFERCNT_SHIFT) + 1)
    count = (_LDMA_CH_CTRL_XFERCNT_MASK >> _LDMA_CH_CTRL_XFERCNT_SHIFT) + 1;
  ctx->xfer_count -= count;

  if (ctx->bus.bits_per_word > 8) {
    bytes_per_transfer = 2;
    dma_ctrl_base = LDMA_CH_CTRL_SIZE_HALFWORD;
    LDMA->CH[ctx->rx_dma_channel].SRC = (uint32_t) &ctx->regs->RXDOUBLE;
    LDMA->CH[ctx->tx_dma_channel].DST = (uint32_t) &ctx->regs->TXDOUBLE;
  } else {
    bytes_per_transfer = 1;
    dma_ctrl_base = LDMA_CH_CTRL_SIZE_BYTE;
    LDMA->CH[ctx->rx_dma_channel].SRC = (uint32_t) &ctx->regs->RXDATA;
    LDMA->CH[ctx->tx_dma_channel].DST = (uint32_t) &ctx->regs->TXDATA;
  }

  if (ctx->rx_ptr) {
    LDMA->CH[ctx->rx_dma_channel].DST = (uint32_t) ctx->rx_ptr;
    ctx->rx_ptr += bytes_per_transfer * count;
    LDMA->CH[ctx->rx_dma_channel].CTRL = LDMA_CH_CTRL_DSTINC_ONE |
                                         dma_ctrl_base |
                                         LDMA_CH_CTRL_SRCINC_NONE |
                                         LDMA_CH_CTRL_REQMODE_BLOCK |
                                         LDMA_CH_CTRL_DONEIFSEN |
                                         LDMA_CH_CTRL_BLOCKSIZE_UNIT1 |
                                         ((count - 1) <<
                                          _LDMA_CH_CTRL_XFERCNT_SHIFT);
  } else {
    LDMA->CH[ctx->rx_dma_channel].DST = (uint32_t) &ctx->rx_dummy;
    LDMA->CH[ctx->rx_dma_channel].CTRL = LDMA_CH_CTRL_DSTINC_NONE |
                                         dma_ctrl_base |
                                         LDMA_CH_CTRL_SRCINC_NONE |
                                         LDMA_CH_CTRL_REQMODE_BLOCK |
                                         LDMA_CH_CTRL_DONEIFSEN |
                                         LDMA_CH_CTRL_BLOCKSIZE_UNIT1 |
                                         ((count - 1) <<
                                          _LDMA_CH_CTRL_XFERCNT_SHIFT);
  }

  if (ctx->tx_ptr) {
    LDMA->CH[ctx->tx_dma_channel].SRC = (uint32_t) ctx->tx_ptr;
    ctx->tx_ptr += bytes_per_transfer * count;
    LDMA->CH[ctx->tx_dma_channel].CTRL = LDMA_CH_CTRL_DSTINC_NONE |
                                         dma_ctrl_base |
                                         LDMA_CH_CTRL_SRCINC_ONE |
                                         LDMA_CH_CTRL_REQMODE_BLOCK |
                                         LDMA_CH_CTRL_BLOCKSIZE_UNIT1 |
                                         ((count - 1) <<
                                          _LDMA_CH_CTRL_XFERCNT_SHIFT);
  } else {
    LDMA->CH[ctx->tx_dma_channel].SRC = (uint32_t) &ctx->tx_dummy;
    LDMA->CH[ctx->tx_dma_channel].CTRL = LDMA_CH_CTRL_DSTINC_NONE |
                                         dma_ctrl_base |
                                         LDMA_CH_CTRL_SRCINC_NONE |
                                         LDMA_CH_CTRL_REQMODE_BLOCK |
                                         LDMA_CH_CTRL_BLOCKSIZE_UNIT1 |
                                         ((count - 1) <<
                                          _LDMA_CH_CTRL_XFERCNT_SHIFT);
  }
  /* bit set */
  ((LDMA_TypeDef *) (LDMA_BASE + 0x06000000))->CHEN =
    1 << (ctx->rx_dma_channel + _LDMA_CHEN_CHEN_SHIFT) |
    1 << (ctx->tx_dma_channel + _LDMA_CHEN_CHEN_SHIFT);
}

static void rx_done_int(void *arg)
{
  struct spi_context_s *ctx = (struct spi_context_s *) arg;

  if (ctx->xfer_count)
    xfer_start(ctx);
  else
    rtems_binary_semaphore_post(&ctx->sem);
}

static void cs_control(struct spi_context_s *ctx, unsigned int cs,
                       bool assert)
{
  struct spi_cs_pin_s *pin;
  uint16_t mask;

  if (cs  < ctx->cs_pin_count) {
    pin = &ctx->cs_pins[cs];
    mask = 1 << pin->pin;
    if ((assert && pin->high_true) || (!assert && !pin->high_true))
      efm32gg11_gpio_clear_set(pin->port, 0, mask);
    else
      efm32gg11_gpio_clear_set(pin->port, mask, 0);
  }
}

static uint32_t clock_max(struct spi_context_s *ctx)
{

    return ((ctx->regs == USART2) ?  efm32gg11_clock.hfperbclk :
                                     efm32gg11_clock.hfperclk) / 2;
}

static void clock_set(struct spi_bus *bus)
{
  struct spi_context_s *ctx = (struct spi_context_s *) bus;
  USART_TypeDef *regs = ctx->regs;
  uint32_t r;

  r = (regs == USART2) ? efm32gg11_clock.hfperbclk : efm32gg11_clock.hfperclk;
  /* There seems to be a discrepancy of a factor of 8 between rev 1.2 of the
     EFM32 Giant Gecko 11 Family Reference Manual and reality. */
  r = (16 * r) / bus->speed_hz;
  if (r > 32)
    r -= 32;
  else
    r = 0;

  r <<= _USART_CLKDIV_DIV_SHIFT;
  regs->CLKDIV = r;
}

static void frame_set(struct spi_bus *bus)
{
  struct spi_context_s *ctx = (struct spi_context_s *) bus;
  USART_TypeDef *regs = ctx->regs;
  uint32_t r;

  r = regs->FRAME & ~_USART_FRAME_DATABITS_MASK;
  switch (bus->bits_per_word) {
  case 4:
    r |= USART_FRAME_DATABITS_FOUR;
    break;
  case 5:
    r |= USART_FRAME_DATABITS_FIVE;
    break;
  case 6:
    r |= USART_FRAME_DATABITS_SIX;
    break;
  case 7:
    r |= USART_FRAME_DATABITS_SEVEN;
    break;
  case 8:
    r |= USART_FRAME_DATABITS_EIGHT;
    break;
  case 9:
    r |= USART_FRAME_DATABITS_NINE;
    break;
  case 10:
    r |= USART_FRAME_DATABITS_TEN;
    break;
  case 11:
    r |= USART_FRAME_DATABITS_ELEVEN;
    break;
  case 12:
    r |= USART_FRAME_DATABITS_TWELVE;
    break;
  case 13:
    r |= USART_FRAME_DATABITS_THIRTEEN;
    break;
  case 14:
    r |= USART_FRAME_DATABITS_FOURTEEN;
    break;
  case 15:
    r |= USART_FRAME_DATABITS_FIFTEEN;
    break;
  case 16:
    r |= USART_FRAME_DATABITS_SIXTEEN;
    break;
  default:
    r |= USART_FRAME_DATABITS_EIGHT;
    break;
  }
  regs->FRAME = r;
}

static void mode_set(struct spi_bus *bus)
{
  struct spi_context_s *ctx = (struct spi_context_s *) bus;
  USART_TypeDef *regs = ctx->regs;
  uint32_t r;

  r = regs->CTRL;
  r &= ~(USART_CTRL_MSBF |
         USART_CTRL_CLKPHA | USART_CTRL_CLKPOL);
  if (!bus->lsb_first)
    r |= USART_CTRL_MSBF;
  if (bus->mode & SPI_CPHA)
    r |= USART_CTRL_CLKPHA;
  if (bus->mode & SPI_CPOL)
    r |= USART_CTRL_CLKPOL;
  regs->CTRL = r;
}

static int spi_setup(spi_bus *bus)
{
  struct spi_context_s *ctx = (struct spi_context_s *) bus;
  int result;

  result = 0;
  if ((bus->cs >= ctx->cs_pin_count && !(bus->mode & SPI_NO_CS)) ||
      bus->bits_per_word < 4 || bus->bits_per_word > 16)
    result = -EINVAL;

  if (result == 0) {
    clock_set(bus);
    frame_set(bus);
    mode_set(bus);
  }

  return result;
}

static int spi_transfer(spi_bus *bus,
                        const spi_ioc_transfer *msgs,
                        uint32_t msg_count)
{
  struct spi_context_s *ctx = (struct spi_context_s *) bus;
  USART_TypeDef *regs = ctx->regs;
  bool cs_asserted;
  uint8_t setup;
  int sc;

  sc = 0;
  cs_asserted = false;
  while (msg_count && sc == 0) {
    bus->cs = msgs->cs;
    bus->mode = msgs->mode;
    bus->speed_hz = msgs->speed_hz;
    bus->bits_per_word = msgs->bits_per_word;
    bus->lsb_first = (msgs->mode & SPI_LSB_FIRST) ? true : false;
    bus->delay_usecs = msgs->delay_usecs;
    sc = spi_setup(bus);

    if (sc == 0) {
      ctx->tx_ptr = msgs->tx_buf;
      ctx->rx_ptr = msgs->rx_buf;
      if (bus->bits_per_word > 8)
        ctx->xfer_count = msgs->len / 2;
      else
        ctx->xfer_count = msgs->len;

      rtems_binary_semaphore_try_wait(&ctx->sem);
      if (!cs_asserted && !(msgs->mode & SPI_NO_CS)) {
        setup = ctx->cs_pins[bus->cs].cs_setup_bits;
        if (setup) {
          regs->TIMECMP0 = USART_TIMECMP0_RESTARTEN_DEFAULT |
                           USART_TIMECMP0_TSTOP_DEFAULT |
                           USART_TIMECMP0_TSTART_DISABLE |
                           ((setup << _USART_TIMECMP0_TCMPVAL_SHIFT) &
                            _USART_TIMECMP0_TCMPVAL_MASK);
          regs->TIMING = USART_TIMING_CSHOLD_ZERO |
                         USART_TIMING_ICS_ZERO |
                         USART_TIMING_CSSETUP_TCMP0 |
                         USART_TIMING_TXDELAY_DISABLE;
        } else {
          regs->TIMING = USART_TIMING_CSHOLD_ZERO |
                         USART_TIMING_ICS_ZERO |
                         USART_TIMING_CSSETUP_ZERO |
                         USART_TIMING_TXDELAY_DISABLE;
        }
        cs_asserted = true;
        cs_control(ctx, bus->cs, true);
      }
      xfer_start(ctx);
      rtems_binary_semaphore_wait(&ctx->sem);
    }
    if ((msgs->cs_change || msg_count == 1 || sc) &&
        !(msgs->mode & SPI_NO_CS)) {
      cs_control(ctx, bus->cs, false);
      cs_asserted = false;
    }
    msgs++;
    msg_count--;
  }

  return sc;
}

static void usart_init(struct spi_context_s *ctx)
{
  USART_TypeDef *regs = ctx->regs;

  regs->IEN = 0;
  regs->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX |
              USART_CMD_RXDIS | USART_CMD_TXDIS;

  regs->CTRL = USART_CTRL_SYNC;
  regs->FRAME = USART_FRAME_STOPBITS_DEFAULT |
                USART_FRAME_PARITY_NONE |
                USART_FRAME_DATABITS_EIGHT;
  regs->TIMING = USART_TIMING_CSHOLD_ZERO |
                 USART_TIMING_ICS_ZERO |
                 USART_TIMING_CSSETUP_ZERO |
                 USART_TIMING_TXDELAY_DISABLE;

  regs->CMD = USART_CMD_MASTEREN | USART_CMD_RXEN | USART_CMD_TXEN;
}

static void pins_init(struct spi_context_s *ctx)
{
  USART_TypeDef *regs = ctx->regs;
  int i;
  uint32_t r;

  for (i = 0; i < ctx->cs_pin_count; i++) {
    cs_control(ctx, i, false);
    efm32gg11_gpio_mode(ctx->cs_pins[i].port, ctx->cs_pins[i].pin,
                        GPIO_P_MODEL_MODE0_PUSHPULL);
  }

  regs->ROUTELOC0 = ((ctx->clk_pin_loc << _USART_ROUTELOC0_CLKLOC_SHIFT) &
                     _USART_ROUTELOC0_CLKLOC_MASK) |
                    ((ctx->tx_pin_loc << _USART_ROUTELOC0_TXLOC_SHIFT) &
                     _USART_ROUTELOC0_TXLOC_MASK) |
                    ((ctx->rx_pin_loc << _USART_ROUTELOC0_RXLOC_SHIFT) &
                     _USART_ROUTELOC0_RXLOC_MASK);

  r = 0;
  if (ctx->clk_pin_loc >= 0) {
    efm32gg11_gpio_clear_set(ctx->clk_port, 1 << ctx->clk_pin, 0);
    efm32gg11_gpio_mode(ctx->clk_port, ctx->clk_pin,
                        GPIO_P_MODEL_MODE0_PUSHPULL);
    r |= USART_ROUTEPEN_CLKPEN;
  }
  if (ctx->tx_pin_loc >= 0) {
    efm32gg11_gpio_clear_set(ctx->tx_port, 1 << ctx->tx_pin, 0);
    efm32gg11_gpio_mode(ctx->tx_port, ctx->tx_pin,
                        GPIO_P_MODEL_MODE0_PUSHPULL);
    r |= USART_ROUTEPEN_TXPEN;
  }
  if (ctx->rx_pin_loc >= 0) {
    efm32gg11_gpio_mode(ctx->rx_port, ctx->rx_pin,
                        GPIO_P_MODEL_MODE0_INPUT);
    r |= USART_ROUTEPEN_RXPEN;
  }
  regs->ROUTEPEN = r;
}

static void dma_init(struct spi_context_s *ctx)
{

  LDMA->CH[ctx->tx_dma_channel].REQSEL = ctx->tx_dma_req;
  LDMA->CH[ctx->tx_dma_channel].CFG = LDMA_CH_CFG_SRCINCSIGN_POSITIVE |
                                      LDMA_CH_CFG_ARBSLOTS_ONE;
  LDMA->CH[ctx->tx_dma_channel].LOOP = LDMA_CH_LOOP_LOOPCNT_DEFAULT;
  LDMA->CH[ctx->tx_dma_channel].LINK = 0;

  LDMA->CH[ctx->rx_dma_channel].REQSEL = ctx->rx_dma_req;
  LDMA->CH[ctx->rx_dma_channel].CFG = LDMA_CH_CFG_DSTINCSIGN_POSITIVE |
                                      LDMA_CH_CFG_ARBSLOTS_ONE;
  LDMA->CH[ctx->rx_dma_channel].LOOP = LDMA_CH_LOOP_LOOPCNT_DEFAULT;
  LDMA->CH[ctx->rx_dma_channel].LINK = 0;

  efm32gg11_dma_register(ctx->rx_dma_channel, rx_done_int, ctx);
  LDMA->IFC = 1 << (ctx->rx_dma_channel + _LDMA_IFC_DONE_SHIFT);
  efm32gg11_dma_int_enable(ctx->rx_dma_channel, true);
}

static void spi_destroy(spi_bus *bus)
{
  struct spi_context_s *ctx = (struct spi_context_s *) bus;
  USART_TypeDef *regs = ctx->regs;
  rtems_interrupt_level level;

  regs->CMD = USART_CMD_RXDIS | USART_CMD_TXDIS;
  regs->IEN = 0;

  efm32gg11_dma_int_enable(ctx->tx_dma_channel, false);
  efm32gg11_dma_int_enable(ctx->rx_dma_channel, false);
  __DSB();
  rtems_binary_semaphore_destroy(&ctx->sem);

  rtems_interrupt_disable(level);
  CMU->HFPERCLKEN0 &= ~ctx->hfperclken0_mask;
  rtems_interrupt_enable(level);
}

int efm32gg11_spi_register()
{
  rtems_status_code sc;
  struct spi_context_s *ctx;
  rtems_interrupt_level level;
  size_t i;

  sc = RTEMS_SUCCESSFUL;
  for (i = 0;
       i < RTEMS_ARRAY_SIZE(spi_instances) && sc == RTEMS_SUCCESSFUL;
       i++) {
    ctx = &spi_instances[i];
    spi_bus_init(&ctx->bus);

    rtems_interrupt_disable(level);
    CMU->HFPERCLKEN0 |= ctx->hfperclken0_mask;
    rtems_interrupt_enable(level);
    usart_init(ctx);
    pins_init(ctx);
    dma_init(ctx);

    ctx->bus.transfer = spi_transfer;
    ctx->bus.destroy = spi_destroy;
    ctx->bus.setup = spi_setup;
    ctx->bus.max_speed_hz = clock_max(ctx);
    ctx->bus.speed_hz = ctx->bus.max_speed_hz;
    ctx->bus.bits_per_word = 8;
    ctx->bus.cs = 0;

    spi_setup(&ctx->bus);

    sc = spi_bus_register(&ctx->bus, ctx->name);
  }

  return sc;
}
