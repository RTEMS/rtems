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

#ifndef LIBBSP_ARM_LPC24XX_DMA_H
#define LIBBSP_ARM_LPC24XX_DMA_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc24xx_dma DMA Support
 *
 * @ingroup lpc24xx
 *
 * @brief Direct memory access (DMA) support.
 *
 * @{
 */

/**
 * @brief Initializes the general purpose DMA.
 */
void lpc24xx_dma_initialize(void);

/**
 * @brief Tries to obtain the DMA channel @a channel.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid channel number.
 * @retval RTEMS_RESOURCE_IN_USE Channel already occupied.
 */
rtems_status_code lpc24xx_dma_channel_obtain(unsigned channel);

/**
 * @brief Releases the DMA channel @a channel.
 *
 * You must have obtained this channel with lpc24xx_dma_channel_obtain()
 * previously.
 *
 * If the channel number @a channel is out of range nothing will happen.
 */
void lpc24xx_dma_channel_release(unsigned channel);

/**
 * @brief Disables the DMA channel @a channel.
 *
 * If @a force is @c false the channel will be halted and disabled when the
 * channel is inactive otherwise it will be disabled immediately.
 *
 * If the channel number @a channel is out of range nothing will happen.
 */
void lpc24xx_dma_channel_disable(unsigned channel, bool force);

rtems_status_code lpc24xx_dma_copy_initialize(void);

rtems_status_code lpc24xx_dma_copy_release(void);

rtems_status_code lpc24xx_dma_copy(
  unsigned channel,
  void *dest,
  const void *src,
  size_t n,
  size_t width
);

rtems_status_code lpc24xx_dma_copy_wait(unsigned channel);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_DMA_H */
