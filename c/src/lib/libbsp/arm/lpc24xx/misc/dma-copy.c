/**
 * @file
 *
 * @ingroup lpc24xx_dma
 *
 * @brief Direct memory access (DMA) support.
 */

/*
 * Copyright (c) 2008, 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/lpc24xx.h>
#include <bsp/dma.h>
#include <bsp/irq.h>

static rtems_id lpc24xx_dma_sema_table [GPDMA_CH_NUMBER];

static bool lpc24xx_dma_status_table [GPDMA_CH_NUMBER];

static void lpc24xx_dma_copy_handler(void *arg)
{
  /* Get interrupt status */
  uint32_t tc = GPDMA_INT_TCSTAT;
  uint32_t err = GPDMA_INT_ERR_STAT;

  /* Clear interrupt status */
  GPDMA_INT_TCCLR = tc;
  GPDMA_INT_ERR_CLR = err;

  /* Check channel 0 */
  if ((tc & GPDMA_STATUS_CH_0) != 0) {
    rtems_semaphore_release(lpc24xx_dma_sema_table [0]);
  }
  lpc24xx_dma_status_table [0] = (err & GPDMA_STATUS_CH_0) == 0;

  /* Check channel 1 */
  if ((tc & GPDMA_STATUS_CH_1) != 0) {
    rtems_semaphore_release(lpc24xx_dma_sema_table [1]);
  }
  lpc24xx_dma_status_table [1] = (err & GPDMA_STATUS_CH_1) == 0;
}

rtems_status_code lpc24xx_dma_copy_initialize(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_id id0 = RTEMS_ID_NONE;
  rtems_id id1 = RTEMS_ID_NONE;

  /* Create semaphore for channel 0 */
  sc = rtems_semaphore_create(
    rtems_build_name('D', 'M', 'A', '0'),
    0,
    RTEMS_LOCAL | RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &id0
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  /* Create semaphore for channel 1 */
  sc = rtems_semaphore_create(
    rtems_build_name('D', 'M', 'A', '1'),
    0,
    RTEMS_LOCAL | RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &id1
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_semaphore_delete(id0);

    return sc;
  }

  /* Install DMA interrupt handler */
  sc = rtems_interrupt_handler_install(
    LPC24XX_IRQ_DMA,
    "DMA copy",
    RTEMS_INTERRUPT_UNIQUE,
    lpc24xx_dma_copy_handler,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_semaphore_delete(id0);
    rtems_semaphore_delete(id1);

    return sc;
  }

  /* Initialize global data */
  lpc24xx_dma_sema_table [0] = id0;
  lpc24xx_dma_sema_table [1] = id1;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code lpc24xx_dma_copy_release(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code rsc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_remove(
    LPC24XX_IRQ_DMA,
    lpc24xx_dma_copy_handler,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rsc = sc;
  }

  sc = rtems_semaphore_delete(lpc24xx_dma_sema_table [0]);
  if (sc != RTEMS_SUCCESSFUL) {
    rsc = sc;
  }

  sc = rtems_semaphore_delete(lpc24xx_dma_sema_table [1]);
  if (sc != RTEMS_SUCCESSFUL) {
    rsc = sc;
  }

  return rsc;
}

rtems_status_code lpc24xx_dma_copy(
  unsigned channel,
  void *dest,
  const void *src,
  size_t n,
  size_t width
)
{
  volatile lpc24xx_dma_channel *e = GPDMA_CH_BASE_ADDR(channel);
  uint32_t w = GPDMA_CH_CTRL_W_8;

  switch (width) {
    case 4:
      w = GPDMA_CH_CTRL_W_32;
      break;
    case 2:
      w = GPDMA_CH_CTRL_W_16;
      break;
  }

  n = n >> w;

  if (n > 0U && n < 4096U) {
    e->desc.src = (uint32_t) src;
    e->desc.dest = (uint32_t) dest;
    e->desc.lli = 0;
    e->desc.ctrl = SET_GPDMA_CH_CTRL_TSZ(0, n)
      | SET_GPDMA_CH_CTRL_SBSZ(0, GPDMA_CH_CTRL_BSZ_1)
      | SET_GPDMA_CH_CTRL_DBSZ(0, GPDMA_CH_CTRL_BSZ_1)
      | SET_GPDMA_CH_CTRL_SW(0, w)
      | SET_GPDMA_CH_CTRL_DW(0, w)
      | GPDMA_CH_CTRL_ITC
      | GPDMA_CH_CTRL_SI
      | GPDMA_CH_CTRL_DI;
    e->cfg = SET_GPDMA_CH_CFG_FLOW(0, GPDMA_CH_CFG_FLOW_MEM_TO_MEM_DMA)
      | GPDMA_CH_CFG_IE
      | GPDMA_CH_CFG_ITC
      | GPDMA_CH_CFG_EN;
  } else {
    return RTEMS_INVALID_SIZE;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code lpc24xx_dma_copy_wait(unsigned channel)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_semaphore_obtain(
    lpc24xx_dma_sema_table [channel],
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  return lpc24xx_dma_status_table [channel]
    ? RTEMS_SUCCESSFUL : RTEMS_IO_ERROR;
}
