/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief DMA support.
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

#include <rtems/endian.h>

#include <bsp/lpc24xx.h>
#include <bsp/dma.h>

/**
 * @brief Table that indicates if a channel is currently occupied.
 */
static bool lpc24xx_dma_channel_occupation [GPDMA_CH_NUMBER];

/**
 * @brief Initializes the general purpose DMA.
 */
void lpc24xx_dma_initialize( void)
{
  rtems_interrupt_level level;

  /* Enable power */
  rtems_interrupt_disable( level);
  PCONP = SET_FLAG( PCONP, PCONP_PCGPDMA);
  rtems_interrupt_enable( level);

  /* Disable module */
  GPDMA_CONFIG = 0;

  /* Enable module */
  #if BYTE_ORDER == LITTLE_ENDIAN
    GPDMA_CONFIG = GPDMA_CONFIG_EN;
  #else
    GPDMA_CONFIG = GPDMA_CONFIG_EN | GPDMA_CONFIG_MODE;
  #endif

  /* Reset registers */
  GPDMA_SOFT_SREQ = 0;
  GPDMA_SOFT_BREQ = 0;
  GPDMA_SOFT_LSREQ = 0;
  GPDMA_SOFT_LBREQ = 0;
  GPDMA_SYNC = 0;
}

/**
 * @brief Returns true if the channel @a channel was obtained.
 *
 * If the channel number @a channel is out of range the last valid channel will
 * be used.
 */
bool lpc24xx_dma_channel_obtain( unsigned channel)
{
  rtems_interrupt_level level;
  bool occupation = true;

  if (channel > GPDMA_CH_NUMBER) {
    channel = GPDMA_CH_NUMBER - 1;
  }

  rtems_interrupt_disable( level);
  occupation = lpc24xx_dma_channel_occupation [channel];
  lpc24xx_dma_channel_occupation [channel] = true;
  rtems_interrupt_enable( level);

  return !occupation;
}

/**
 * @brief Releases the channel @a channel.  You must have obtained this channel
 * with lpc24xx_dma_channel_obtain() previously.
 *
 * If the channel number @a channel is out of range the last valid channel will
 * be used.
 */
void lpc24xx_dma_channel_release( unsigned channel)
{
  if (channel > GPDMA_CH_NUMBER) {
    channel = GPDMA_CH_NUMBER - 1;
  }

  lpc24xx_dma_channel_occupation [channel] = false;
}

/**
 * @brief Disables the channel @a channel.
 *
 * If @a force is false the channel will be halted and disabled when the
 * channel is inactive.  If the channel number @a channel is out of range the
 * last valid channel will be used.
 */
void lpc24xx_dma_channel_disable( unsigned channel, bool force)
{
  volatile lpc24xx_dma_channel *ch = GPDMA_CH_BASE_ADDR( channel);
  uint32_t cfg = ch->cfg;

  if (!force) {
    /* Halt */
    ch->cfg = SET_FLAG( cfg, GPDMA_CH_CFG_HALT);

    /* Wait for inactive */
    do {
      cfg = ch->cfg;
    } while (IS_FLAG_SET( cfg, GPDMA_CH_CFG_ACTIVE));
  }

  /* Disable */
  ch->cfg = CLEAR_FLAG( cfg, GPDMA_CH_CFG_EN);
}
