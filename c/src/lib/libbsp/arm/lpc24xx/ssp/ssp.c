/**
 * @file
 *
 * @ingroup lpc24xx_libi2c
 *
 * @brief LibI2C bus driver for the Synchronous Serial Port (SSP).
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <stdbool.h>

#include <bsp/ssp.h>
#include <bsp/lpc24xx.h>
#include <bsp/irq.h>
#include <bsp/system-clocks.h>
#include <bsp/dma.h>
#include <bsp/io.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#define LPC24XX_SSP_NUMBER 2

#define LPC24XX_SSP_FIFO_SIZE 8

#define LPC24XX_SSP_BAUD_RATE 2000000

typedef enum {
  LPC24XX_SSP_DMA_INVALID = 0,
  LPC24XX_SSP_DMA_AVAILABLE = 1,
  LPC24XX_SSP_DMA_NOT_INITIALIZED = 2,
  LPC24XX_SSP_DMA_INITIALIZATION = 3,
  LPC24XX_SSP_DMA_TRANSFER_FLAG = 0x80000000U,
  LPC24XX_SSP_DMA_WAIT = 1 | LPC24XX_SSP_DMA_TRANSFER_FLAG,
  LPC24XX_SSP_DMA_WAIT_FOR_CHANNEL_0 = 2 | LPC24XX_SSP_DMA_TRANSFER_FLAG,
  LPC24XX_SSP_DMA_WAIT_FOR_CHANNEL_1 = 3 | LPC24XX_SSP_DMA_TRANSFER_FLAG,
  LPC24XX_SSP_DMA_ERROR = 4 | LPC24XX_SSP_DMA_TRANSFER_FLAG,
  LPC24XX_SSP_DMA_DONE = 5 | LPC24XX_SSP_DMA_TRANSFER_FLAG
} lpc24xx_ssp_dma_status;

typedef struct {
  rtems_libi2c_bus_t bus;
  volatile lpc24xx_ssp *regs;
  unsigned clock;
  uint32_t idle_char;
} lpc24xx_ssp_bus_entry;

typedef struct {
  lpc24xx_ssp_dma_status status;
  lpc24xx_ssp_bus_entry *bus;
  rtems_libi2c_read_write_done_t done;
  int n;
  void *arg;
} lpc24xx_ssp_dma_entry;

static lpc24xx_ssp_dma_entry lpc24xx_ssp_dma_data = {
  .status = LPC24XX_SSP_DMA_NOT_INITIALIZED,
  .bus = NULL,
  .done = NULL,
  .n = 0,
  .arg = NULL
};

static uint32_t lpc24xx_ssp_trash = 0;

static inline bool lpc24xx_ssp_is_busy(const lpc24xx_ssp_bus_entry *bus)
{
  return lpc24xx_ssp_dma_data.bus == bus
    && lpc24xx_ssp_dma_data.status != LPC24XX_SSP_DMA_AVAILABLE;
}

static void lpc24xx_ssp_handler(void *arg)
{
  lpc24xx_ssp_bus_entry *e = (lpc24xx_ssp_bus_entry *) arg;
  volatile lpc24xx_ssp *regs = e->regs;
  uint32_t mis = regs->mis;
  uint32_t icr = 0;

  if ((mis & SSP_MIS_RORRIS) != 0) {
    /* TODO */
    printk("%s: Receiver overrun!\n", __func__);
    icr |= SSP_ICR_RORRIS;
  }

  regs->icr = icr;
}

static void lpc24xx_ssp_dma_handler(void *arg)
{
  lpc24xx_ssp_dma_entry *e = (lpc24xx_ssp_dma_entry *) arg;
  lpc24xx_ssp_dma_status status = e->status;
  uint32_t tc = 0;
  uint32_t err = 0;
  int rv = 0;

  /* Return if we are not in a transfer status */
  if ((status & LPC24XX_SSP_DMA_TRANSFER_FLAG) == 0) {
    return;
  }

  /* Get interrupt status */
  tc = GPDMA_INT_TCSTAT;
  err = GPDMA_INT_ERR_STAT;

  /* Clear interrupt status */
  GPDMA_INT_TCCLR = tc;
  GPDMA_INT_ERR_CLR = err;

  /* Change status */
  if (err == 0) {
    switch (status) {
      case LPC24XX_SSP_DMA_WAIT:
        if ((tc & (GPDMA_STATUS_CH_0 | GPDMA_STATUS_CH_1)) != 0) {
          status = LPC24XX_SSP_DMA_DONE;
        } else if ((tc & GPDMA_STATUS_CH_0) != 0) {
          status = LPC24XX_SSP_DMA_WAIT_FOR_CHANNEL_1;
        } else if ((tc & GPDMA_STATUS_CH_1) != 0) {
          status = LPC24XX_SSP_DMA_WAIT_FOR_CHANNEL_0;
        }
        break;
      case LPC24XX_SSP_DMA_WAIT_FOR_CHANNEL_0:
        if ((tc & GPDMA_STATUS_CH_1) != 0) {
          status = LPC24XX_SSP_DMA_ERROR;
        } else if ((tc & GPDMA_STATUS_CH_0) != 0) {
          status = LPC24XX_SSP_DMA_DONE;
        }
        break;
      case LPC24XX_SSP_DMA_WAIT_FOR_CHANNEL_1:
        if ((tc & GPDMA_STATUS_CH_0) != 0) {
          status = LPC24XX_SSP_DMA_ERROR;
        } else if ((tc & GPDMA_STATUS_CH_1) != 0) {
          status = LPC24XX_SSP_DMA_DONE;
        }
        break;
      default:
        status = LPC24XX_SSP_DMA_ERROR;
        break;
    }
  } else {
    status = LPC24XX_SSP_DMA_ERROR;
  }

  /* Error cleanup */
  if (status == LPC24XX_SSP_DMA_ERROR) {
    lpc24xx_dma_channel_disable(0, true);
    lpc24xx_dma_channel_disable(1, true);
    status = LPC24XX_SSP_DMA_DONE;
    rv = -RTEMS_IO_ERROR;
  }

  /* Done */
  if (status == LPC24XX_SSP_DMA_DONE) {
    status = LPC24XX_SSP_DMA_AVAILABLE;
    if (e->done != NULL) {
      e->done(rv, e->n, e->arg);
      e->done = NULL;
    }
  }

  /* Set status */
  e->status = status;
}

static rtems_status_code lpc24xx_ssp_init(rtems_libi2c_bus_t *bus)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interrupt_level level;
  lpc24xx_ssp_bus_entry *e = (lpc24xx_ssp_bus_entry *) bus;
  volatile lpc24xx_ssp *regs = e->regs;
  unsigned pclk = lpc24xx_cclk();
  unsigned pre =
    ((pclk + LPC24XX_SSP_BAUD_RATE - 1) / LPC24XX_SSP_BAUD_RATE + 1) & ~1U;
  lpc24xx_module module = LPC24XX_MODULE_SSP_0;
  rtems_vector_number vector = UINT32_MAX;

  if (lpc24xx_ssp_dma_data.status == LPC24XX_SSP_DMA_NOT_INITIALIZED) {
    lpc24xx_ssp_dma_status status = LPC24XX_SSP_DMA_INVALID;

    /* Test and set DMA support status */
    rtems_interrupt_disable(level);
    status = lpc24xx_ssp_dma_data.status;
    if (status == LPC24XX_SSP_DMA_NOT_INITIALIZED) {
      lpc24xx_ssp_dma_data.status = LPC24XX_SSP_DMA_INITIALIZATION;
    }
    rtems_interrupt_enable(level);

    if (status == LPC24XX_SSP_DMA_NOT_INITIALIZED) {
      /* Install DMA interrupt handler */
      sc = rtems_interrupt_handler_install(
        LPC24XX_IRQ_DMA,
        "SSP DMA",
        RTEMS_INTERRUPT_SHARED,
        lpc24xx_ssp_dma_handler,
        &lpc24xx_ssp_dma_data
      );
      RTEMS_CHECK_SC(sc, "install DMA interrupt handler");

      /* Set DMA support status */
      lpc24xx_ssp_dma_data.status = LPC24XX_SSP_DMA_AVAILABLE;
    }
  }

  /* Disable module */
  regs->cr1 = 0;

  switch ((uintptr_t) regs) {
    case SSP0_BASE_ADDR:
      module = LPC24XX_MODULE_SSP_0;
      vector = LPC24XX_IRQ_SPI_SSP_0;
      break;
    case SSP1_BASE_ADDR:
      module = LPC24XX_MODULE_SSP_1;
      vector = LPC24XX_IRQ_SSP_1;
      break;
    default:
      return RTEMS_IO_ERROR;
  }

  /* Set clock select */
  sc = lpc24xx_module_enable(module, LPC24XX_MODULE_PCLK_DEFAULT);
  RTEMS_CHECK_SC(sc, "enable module clock");

  /* Set serial clock rate to save value */
  regs->cr0 = SET_SSP_CR0_SCR(0, 255);

  /* Set clock prescaler */
  if (pre > 254) {
    pre = 254;
  } else if (pre < 2) {
    pre = 2;
  }
  regs->cpsr = pre;

  /* Save clock value */
  e->clock = pclk / pre;

  /* Enable module and loop back mode */
  regs->cr1 = SSP_CR1_LBM | SSP_CR1_SSE;

  /* Install interrupt handler */
  sc = rtems_interrupt_handler_install(
    vector,
    "SSP",
    RTEMS_INTERRUPT_UNIQUE,
    lpc24xx_ssp_handler,
    e
  );
  RTEMS_CHECK_SC(sc, "install interrupt handler");

  /* Enable receiver overrun interrupts */
  e->regs->imsc = SSP_IMSC_RORIM;

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lpc24xx_ssp_send_start(rtems_libi2c_bus_t *bus)
{
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lpc24xx_ssp_send_stop(rtems_libi2c_bus_t *bus)
{
  lpc24xx_ssp_bus_entry *e = (lpc24xx_ssp_bus_entry *) bus;

  /* Release DMA support */
  if (lpc24xx_ssp_dma_data.bus == e) {
    if (lpc24xx_ssp_dma_data.status == LPC24XX_SSP_DMA_AVAILABLE) {
      lpc24xx_dma_channel_release(0);
      lpc24xx_dma_channel_release(1);
      lpc24xx_ssp_dma_data.bus = NULL;
    } else {
      return RTEMS_RESOURCE_IN_USE;
    }
  }

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code lpc24xx_ssp_send_addr(
  rtems_libi2c_bus_t *bus,
  uint32_t addr,
  int rw
)
{
  lpc24xx_ssp_bus_entry *e = (lpc24xx_ssp_bus_entry *) bus;

  if (lpc24xx_ssp_is_busy(e)) {
    return RTEMS_RESOURCE_IN_USE;
  }

  return RTEMS_SUCCESSFUL;
}

static int lpc24xx_ssp_set_transfer_mode(
  rtems_libi2c_bus_t *bus,
  const rtems_libi2c_tfr_mode_t *mode
)
{
  lpc24xx_ssp_bus_entry *e = (lpc24xx_ssp_bus_entry *) bus;
  volatile lpc24xx_ssp *regs = e->regs;
  unsigned clk = e->clock;
  unsigned br = mode->baudrate;
  unsigned scr = (clk + br - 1) / br;

  if (lpc24xx_ssp_is_busy(e)) {
    return -RTEMS_RESOURCE_IN_USE;
  }

  if (mode->bits_per_char != 8) {
    return -RTEMS_INVALID_NUMBER;
  }

  if (mode->lsb_first) {
    return -RTEMS_INVALID_NUMBER;
  }

  if (br == 0) {
    return -RTEMS_INVALID_NUMBER;
  }

  /* Compute new prescaler if necessary */
  if (scr > 256 || scr < 1) {
    unsigned pre = regs->cpsr;
    unsigned pclk = clk * pre;

    while (scr > 256) {
      if (pre > 252) {
        return -RTEMS_INVALID_NUMBER;
      }
      pre += 2;
      clk = pclk / pre;
      scr = (clk + br - 1) / br;
    }

    while (scr < 1) {
      if (pre < 4) {
        return -RTEMS_INVALID_NUMBER;
      }
      pre -= 2;
      clk = pclk / pre;
      scr = (clk + br - 1) / br;
    }

    regs->cpsr = pre;
    e->clock = clk;
  }

  /* Adjust SCR */
  --scr;

  e->idle_char = mode->idle_char;

  while ((regs->sr & SSP_SR_TFE) == 0) {
    /* Wait */
  }

  regs->cr0 = SET_SSP_CR0_DSS(0, 0x7)
    | SET_SSP_CR0_SCR(0, scr)
    | (mode->clock_inv ? SSP_CR0_CPOL : 0)
    | (mode->clock_phs ? SSP_CR0_CPHA : 0);

  return 0;
}

static int lpc24xx_ssp_read_write(
  rtems_libi2c_bus_t *bus,
  unsigned char *in,
  const unsigned char *out,
  int n
)
{
  lpc24xx_ssp_bus_entry *e = (lpc24xx_ssp_bus_entry *) bus;
  volatile lpc24xx_ssp *regs = e->regs;
  int r = 0;
  int w = 0;
  int dr = 1;
  int dw = 1;
  int m = 0;
  uint32_t sr = regs->sr;
  unsigned char trash = 0;
  unsigned char idle_char = (unsigned char) e->idle_char;

  if (lpc24xx_ssp_is_busy(e)) {
    return -RTEMS_RESOURCE_IN_USE;
  }

  if (n < 0) {
    return -RTEMS_INVALID_SIZE;
  }

  /* Disable DMA on SSP */
  regs->dmacr = 0;

  if (in == NULL) {
    dr = 0;
    in = &trash;
  }

  if (out == NULL) {
    dw = 0;
    out = &idle_char;
  }

  /*
   * Assumption: The transmit and receive FIFOs are empty.  If this assumption
   * is not true an input buffer overflow may occur or we may never exit the
   * loop due to data loss.  This is only possible if entities external to this
   * driver operate on the SSP.
   */

  while (w < n) {
    /* FIFO capacity */
    m = w - r;

    /* Write */
    if ((sr & SSP_SR_TNF) != 0 && m < LPC24XX_SSP_FIFO_SIZE) {
      regs->dr = *out;
      ++w;
      out += dw;
    }

    /* Read */
    if ((sr & SSP_SR_RNE) != 0) {
      *in = (unsigned char) regs->dr;
      ++r;
      in += dr;
    }

    /* New status */
    sr = regs->sr;
  }

  /* Read outstanding input */
  while (r < n) {
    /* Wait */
    do {
      sr = regs->sr;
    } while ((sr & SSP_SR_RNE) == 0);

    /* Read */
    *in = (unsigned char) regs->dr;
    ++r;
    in += dr;
  }

  return n;
}

static int lpc24xx_ssp_read_write_async(
  rtems_libi2c_bus_t *bus,
  unsigned char *in,
  const unsigned char *out,
  int n,
  rtems_libi2c_read_write_done_t done,
  void *arg
)
{
  rtems_interrupt_level level;
  lpc24xx_ssp_bus_entry *e = (lpc24xx_ssp_bus_entry *) bus;
  volatile lpc24xx_ssp *ssp = e->regs;
  volatile lpc24xx_dma_channel *receive_channel = GPDMA_CH_BASE_ADDR(0);
  volatile lpc24xx_dma_channel *transmit_channel = GPDMA_CH_BASE_ADDR(1);
  uint32_t di = GPDMA_CH_CTRL_DI;
  uint32_t si = GPDMA_CH_CTRL_SI;

  if (n < 0 || n > (int) GPDMA_CH_CTRL_TSZ_MAX) {
    return -RTEMS_INVALID_SIZE;
  }

  /* Try to reserve DMA support for this bus */
  if (lpc24xx_ssp_dma_data.bus == NULL) {
    rtems_interrupt_disable(level);
    if (lpc24xx_ssp_dma_data.bus == NULL) {
      lpc24xx_ssp_dma_data.bus = e;
    }
    rtems_interrupt_enable(level);

    /* Try to obtain DMA channels */
    if (lpc24xx_ssp_dma_data.bus == e) {
      rtems_status_code cs0 = lpc24xx_dma_channel_obtain(0);
      rtems_status_code cs1 = lpc24xx_dma_channel_obtain(1);

      if (cs0 != RTEMS_SUCCESSFUL || cs1 != RTEMS_SUCCESSFUL) {
        if (cs0 == RTEMS_SUCCESSFUL) {
          lpc24xx_dma_channel_release(0);
        }
        if (cs1 == RTEMS_SUCCESSFUL) {
          lpc24xx_dma_channel_release(1);
        }
        lpc24xx_ssp_dma_data.bus = NULL;
      }
    }
  }

  /* Check if DMA support is available */
  if (lpc24xx_ssp_dma_data.bus != e
    || lpc24xx_ssp_dma_data.status != LPC24XX_SSP_DMA_AVAILABLE) {
    return -RTEMS_RESOURCE_IN_USE;
  }

  /* Set DMA support status and parameter */
  lpc24xx_ssp_dma_data.status = LPC24XX_SSP_DMA_WAIT;
  lpc24xx_ssp_dma_data.done = done;
  lpc24xx_ssp_dma_data.n = n;
  lpc24xx_ssp_dma_data.arg = arg;

  /* Enable DMA on SSP */
  ssp->dmacr = SSP_DMACR_RXDMAE | SSP_DMACR_TXDMAE;

  /* Receive */
  if (in != NULL) {
    receive_channel->desc.dest = (uint32_t) in;
  } else {
    receive_channel->desc.dest = (uint32_t) &lpc24xx_ssp_trash;
    di = 0;
  }
  receive_channel->desc.src = (uint32_t) &ssp->dr;
  receive_channel->desc.lli = 0;
  receive_channel->desc.ctrl = SET_GPDMA_CH_CTRL_TSZ(0, n)
    | SET_GPDMA_CH_CTRL_SBSZ(0, GPDMA_CH_CTRL_BSZ_4)
    | SET_GPDMA_CH_CTRL_DBSZ(0, GPDMA_CH_CTRL_BSZ_4)
    | SET_GPDMA_CH_CTRL_SW(0, GPDMA_CH_CTRL_W_8)
    | SET_GPDMA_CH_CTRL_DW(0, GPDMA_CH_CTRL_W_8)
    | GPDMA_CH_CTRL_ITC
    | di;
  receive_channel->cfg = SET_GPDMA_CH_CFG_SRCPER(0, GPDMA_CH_CFG_PER_SSP1_RX)
    | SET_GPDMA_CH_CFG_FLOW(0, GPDMA_CH_CFG_FLOW_PER_TO_MEM_DMA)
    | GPDMA_CH_CFG_IE
    | GPDMA_CH_CFG_ITC
    | GPDMA_CH_CFG_EN;

  /* Transmit */
  if (out != NULL) {
    transmit_channel->desc.src = (uint32_t) out;
  } else {
    transmit_channel->desc.src = (uint32_t) &e->idle_char;
    si = 0;
  }
  transmit_channel->desc.dest = (uint32_t) &ssp->dr;
  transmit_channel->desc.lli = 0;
  transmit_channel->desc.ctrl = SET_GPDMA_CH_CTRL_TSZ(0, n)
    | SET_GPDMA_CH_CTRL_SBSZ(0, GPDMA_CH_CTRL_BSZ_4)
    | SET_GPDMA_CH_CTRL_DBSZ(0, GPDMA_CH_CTRL_BSZ_4)
    | SET_GPDMA_CH_CTRL_SW(0, GPDMA_CH_CTRL_W_8)
    | SET_GPDMA_CH_CTRL_DW(0, GPDMA_CH_CTRL_W_8)
    | GPDMA_CH_CTRL_ITC
    | si;
  transmit_channel->cfg = SET_GPDMA_CH_CFG_DESTPER(0, GPDMA_CH_CFG_PER_SSP1_TX)
    | SET_GPDMA_CH_CFG_FLOW(0, GPDMA_CH_CFG_FLOW_MEM_TO_PER_DMA)
    | GPDMA_CH_CFG_IE
    | GPDMA_CH_CFG_ITC
    | GPDMA_CH_CFG_EN;

  return 0;
}

static int lpc24xx_ssp_read(rtems_libi2c_bus_t *bus, unsigned char *in, int n)
{
  return lpc24xx_ssp_read_write(bus, in, NULL, n);
}

static int lpc24xx_ssp_write(
  rtems_libi2c_bus_t *bus,
  unsigned char *out,
  int n
)
{
  return lpc24xx_ssp_read_write(bus, NULL, out, n);
}

static int lpc24xx_ssp_ioctl(rtems_libi2c_bus_t *bus, int cmd, void *arg)
{
  int rv = -1;
  const rtems_libi2c_tfr_mode_t *tm = (const rtems_libi2c_tfr_mode_t *) arg;
  rtems_libi2c_read_write_t *rw = (rtems_libi2c_read_write_t *) arg;
  rtems_libi2c_read_write_async_t *rwa =
    (rtems_libi2c_read_write_async_t *) arg;

  switch (cmd) {
    case RTEMS_LIBI2C_IOCTL_READ_WRITE:
      rv = lpc24xx_ssp_read_write(bus, rw->rd_buf, rw->wr_buf, rw->byte_cnt);
      break;
    case RTEMS_LIBI2C_IOCTL_READ_WRITE_ASYNC:
      rv = lpc24xx_ssp_read_write_async(
        bus,
        rwa->rd_buf,
        rwa->wr_buf,
        rwa->byte_cnt,
        rwa->done,
        rwa->arg
      );
      break;
    case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
      rv = lpc24xx_ssp_set_transfer_mode(bus, tm);
      break;
    default:
      rv = -RTEMS_NOT_DEFINED;
      break;
  }

  return rv;
}

static const rtems_libi2c_bus_ops_t lpc24xx_ssp_ops = {
  .init = lpc24xx_ssp_init,
  .send_start = lpc24xx_ssp_send_start,
  .send_stop = lpc24xx_ssp_send_stop,
  .send_addr = lpc24xx_ssp_send_addr,
  .read_bytes = lpc24xx_ssp_read,
  .write_bytes = lpc24xx_ssp_write,
  .ioctl = lpc24xx_ssp_ioctl
};

static lpc24xx_ssp_bus_entry lpc24xx_ssp_bus_table [LPC24XX_SSP_NUMBER] = {
  {
    /* SSP 0 */
    .bus = {
      .ops = &lpc24xx_ssp_ops,
      .size = sizeof(lpc24xx_ssp_bus_entry)
    },
    .regs = (volatile lpc24xx_ssp *) SSP0_BASE_ADDR,
    .clock = 0,
    .idle_char = 0xffffffff
  }, {
    /* SSP 1 */
    .bus = {
      .ops = &lpc24xx_ssp_ops,
      .size = sizeof(lpc24xx_ssp_bus_entry)
    },
    .regs = (volatile lpc24xx_ssp *) SSP1_BASE_ADDR,
    .clock = 0,
    .idle_char = 0xffffffff
  }
};

rtems_libi2c_bus_t * const lpc24xx_ssp_0 =
  (rtems_libi2c_bus_t *) &lpc24xx_ssp_bus_table [0];

rtems_libi2c_bus_t * const lpc24xx_ssp_1 =
  (rtems_libi2c_bus_t *) &lpc24xx_ssp_bus_table [1];
