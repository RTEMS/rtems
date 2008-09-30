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

#ifndef LIBBSP_ARM_LPC24XX_DMA_H
#define LIBBSP_ARM_LPC24XX_DMA_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void lpc24xx_dma_initialize( void);

bool lpc24xx_dma_channel_obtain( unsigned channel);

void lpc24xx_dma_channel_release( unsigned channel);

void lpc24xx_dma_channel_disable( unsigned channel, bool force);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_DMA_H */
