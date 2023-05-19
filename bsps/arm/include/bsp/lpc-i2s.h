/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup lpc_i2s
 *
 * @brief I2S API.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_SHARED_LPC_I2S_H
#define LIBBSP_ARM_SHARED_LPC_I2S_H

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup lpc_i2s I2S Support
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 * @ingroup RTEMSBSPsARMLPC32XX
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
