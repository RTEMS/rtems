/**
 * @file
 *
 * @ingroup lpc24xx_dma
 *
 * @brief Direct memory access (DMA) support.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/endian.h>

#include <bsp/lpc24xx.h>
#include <bsp/dma.h>
#include <bsp/io.h>

/**
 * @brief Table that indicates if a channel is currently occupied.
 */
static bool lpc24xx_dma_channel_occupation [GPDMA_CH_NUMBER];

void lpc24xx_dma_initialize(void)
{
  /* Enable module power */
  lpc24xx_module_enable(LPC24XX_MODULE_GPDMA, LPC24XX_MODULE_PCLK_DEFAULT);

  /* Disable module */
  GPDMA_CONFIG = 0;

  /* Reset registers */
  GPDMA_SOFT_SREQ = 0;
  GPDMA_SOFT_BREQ = 0;
  GPDMA_SOFT_LSREQ = 0;
  GPDMA_SOFT_LBREQ = 0;
  GPDMA_SYNC = 0;
  GPDMA_CH0_CFG = 0;
  GPDMA_CH1_CFG = 0;

  /* Enable module */
  #if BYTE_ORDER == LITTLE_ENDIAN
    GPDMA_CONFIG = GPDMA_CONFIG_EN;
  #else
    GPDMA_CONFIG = GPDMA_CONFIG_EN | GPDMA_CONFIG_MODE;
  #endif
}

rtems_status_code lpc24xx_dma_channel_obtain(unsigned channel)
{
  if (channel < GPDMA_CH_NUMBER) {
    rtems_interrupt_level level;
    bool occupation = true;

    rtems_interrupt_disable(level);
    occupation = lpc24xx_dma_channel_occupation [channel];
    lpc24xx_dma_channel_occupation [channel] = true;
    rtems_interrupt_enable(level);

    return occupation ? RTEMS_RESOURCE_IN_USE : RTEMS_SUCCESSFUL;
  } else {
    return RTEMS_INVALID_ID;
  }
}

void lpc24xx_dma_channel_release(unsigned channel)
{
  if (channel < GPDMA_CH_NUMBER) {
    lpc24xx_dma_channel_occupation [channel] = false;
  }
}

void lpc24xx_dma_channel_disable(unsigned channel, bool force)
{
  if (channel < GPDMA_CH_NUMBER) {
    volatile lpc24xx_dma_channel *ch = GPDMA_CH_BASE_ADDR(channel);
    uint32_t cfg = ch->cfg;

    if (!force) {
      /* Halt */
      ch->cfg |= GPDMA_CH_CFG_HALT;

      /* Wait for inactive */
      do {
        cfg = ch->cfg;
      } while ((cfg & GPDMA_CH_CFG_ACTIVE) != 0);
    }

    /* Disable */
    ch->cfg &= ~GPDMA_CH_CFG_EN;
  }
}
