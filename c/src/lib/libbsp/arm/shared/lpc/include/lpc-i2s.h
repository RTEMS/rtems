/**
 * @file
 *
 * @ingroup lpc_i2s
 *
 * @brief I2S API.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_SHARED_LPC_I2S_H
#define LIBBSP_ARM_SHARED_LPC_I2S_H

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup lpc_i2s I2S Support
 *
 * @ingroup lpc24xx
 * @ingroup lpc32xx
 *
 * @brief I2S support.
 *
 * @{
 */

/**
 * @brief I2S control block.
 */
typedef struct {
  uint32_t dao;
  uint32_t dai;
  uint32_t txfifo;
  uint32_t rxfifo;
  uint32_t state;
  uint32_t dma [2];
  uint32_t irq;
  uint32_t txrate;
  uint32_t rxrate;
} lpc_i2s;

/**
 * @name I2S Digital Audio Input and Output
 *
 * @{
 */

#define I2S_DAIO_WORDWIDTH(val) BSP_FLD32(val, 0, 1)
#define I2S_DAIO_MONO BSP_BIT32(2)
#define I2S_DAIO_STOP BSP_BIT32(3)
#define I2S_DAIO_RESET BSP_BIT32(4)
#define I2S_DAIO_WS_SEL BSP_BIT32(5)
#define I2S_DAIO_WS_HALFPERIOD(val) BSP_FLD32(val, 6, 14)
#define I2S_DAIO_MUTE BSP_BIT32(15)

/** @} */

/**
 * @name I2S Status Feedback
 *
 * @{
 */

#define I2S_STATE_IRQ BSP_BIT32(0)
#define I2S_STATE_DMAREQ_0 BSP_BIT32(1)
#define I2S_STATE_DMAREQ_1 BSP_BIT32(2)
#define I2S_STATE_RX_LEVEL_GET(reg) BSP_FLD32GET(reg, 8, 11)
#define I2S_STATE_TX_LEVEL_GET(reg) BSP_FLD32GET(reg, 16, 19)

/** @} */

/**
 * @name I2S DMA Configuration
 *
 * @{
 */

#define I2S_DMA_RX_ENABLE BSP_BIT32(0)
#define I2S_DMA_TX_ENABLE BSP_BIT32(1)
#define I2S_DMA_RX_DEPTH(val) BSP_FLD32(val, 8, 11)
#define I2S_DMA_TX_DEPTH(val) BSP_FLD32(val, 16, 19)

/** @} */

/**
 * @name I2S Interrupt Request Control
 *
 * @{
 */

#define I2S_IRQ_RX BSP_BIT32(0)
#define I2S_IRQ_TX BSP_BIT32(1)
#define I2S_IRQ_RX_DEPTH(val) BSP_FLD32(val, 8, 11)
#define I2S_IRQ_TX_DEPTH(val) BSP_FLD32(val, 16, 19)

/** @} */

/**
 * @name I2S Transmit and Receive Clock Rate
 *
 * @{
 */

#define LPC24XX_I2S_RATE(val) BSP_FLD32(val, 0, 9)
#define LPC32XX_I2S_RATE_X_DIVIDER(val) BSP_FLD32(val, 0, 7)
#define LPC32XX_I2S_RATE_Y_DIVIDER(val) BSP_FLD32(val, 8, 15)

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_LPC_I2S_H */
