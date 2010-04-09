/**
 * @file
 *
 * @ingroup lpc_dma
 *
 * @brief DMA API.
 */

/*
 * Copyright (c) 2010
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

#ifndef LIBBSP_ARM_SHARED_LPC_DMA_H
#define LIBBSP_ARM_SHARED_LPC_DMA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup lpc_dma DMA Support
 *
 * @ingroup lpc
 *
 * @brief DMA support.
 *
 * @{
 */

/**
 * @brief DMA descriptor item.
 */
typedef struct {
  uint32_t src;
  uint32_t dest;
  uint32_t lli;
  uint32_t ctrl;
} lpc_dma_descriptor;

/**
 * @brief DMA channel block.
 */
typedef struct {
  lpc_dma_descriptor desc;
  uint32_t cfg;
  uint32_t reserved [3];
} lpc_dma_channel;

/**
 * @brief DMA control block.
 */
typedef struct {
  uint32_t int_stat;
  uint32_t int_tc_stat;
  uint32_t int_tc_clear;
  uint32_t int_err_stat;
  uint32_t int_err_clear;
  uint32_t raw_tc_stat;
  uint32_t raw_err_stat;
  uint32_t enabled_channels;
  uint32_t soft_burst_req;
  uint32_t soft_single_req;
  uint32_t soft_last_burst_req;
  uint32_t soft_last_single_req;
  uint32_t cfg;
  uint32_t sync;
  uint32_t reserved [50];
  lpc_dma_channel channels [];
} lpc_dma;

/**
 * @name DMA Configuration Register Defines
 *
 * @{
 */

#define LPC_DMA_CFG_EN (1U << 0)
#define LPC_DMA_CFG_M_0 (1U << 1)
#define LPC_DMA_CFG_M_1 (1U << 2)

/** @} */

/**
 * @name DMA Channel Control Register Defines
 *
 * @{
 */

#define LPC_DMA_CH_CTRL_TSZ_MASK 0xfffU
#define LPC_DMA_CH_CTRL_TSZ_MAX 0xfffU

#define LPC_DMA_CH_CTRL_SB_MASK (0x7U << 12)
#define LPC_DMA_CH_CTRL_SB_1 (0x0U << 12)
#define LPC_DMA_CH_CTRL_SB_4 (0x1U << 12)
#define LPC_DMA_CH_CTRL_SB_8 (0x2U << 12)
#define LPC_DMA_CH_CTRL_SB_16 (0x3U << 12)
#define LPC_DMA_CH_CTRL_SB_32 (0x4U << 12)
#define LPC_DMA_CH_CTRL_SB_64 (0x5U << 12)
#define LPC_DMA_CH_CTRL_SB_128 (0x6U << 12)
#define LPC_DMA_CH_CTRL_SB_256 (0x7U << 12)

#define LPC_DMA_CH_CTRL_DB_MASK (0x7U << 15)
#define LPC_DMA_CH_CTRL_DB_1 (0x0U << 15)
#define LPC_DMA_CH_CTRL_DB_4 (0x1U << 15)
#define LPC_DMA_CH_CTRL_DB_8 (0x2U << 15)
#define LPC_DMA_CH_CTRL_DB_16 (0x3U << 15)
#define LPC_DMA_CH_CTRL_DB_32 (0x4U << 15)
#define LPC_DMA_CH_CTRL_DB_64 (0x5U << 15)
#define LPC_DMA_CH_CTRL_DB_128 (0x6U << 15)
#define LPC_DMA_CH_CTRL_DB_256 (0x7U << 15)

#define LPC_DMA_CH_CTRL_SW_MASK (0x7U << 18)
#define LPC_DMA_CH_CTRL_SW_8 (0x0U << 18)
#define LPC_DMA_CH_CTRL_SW_16 (0x1U << 18)
#define LPC_DMA_CH_CTRL_SW_32 (0x2U << 18)

#define LPC_DMA_CH_CTRL_DW_MASK (0x7U << 21)
#define LPC_DMA_CH_CTRL_DW_8 (0x0U << 21)
#define LPC_DMA_CH_CTRL_DW_16 (0x1U << 21)
#define LPC_DMA_CH_CTRL_DW_32 (0x2U << 21)

#define LPC_DMA_CH_CTRL_SM_0 (0U << 24)
#define LPC_DMA_CH_CTRL_SM_1 (1U << 24)

#define LPC_DMA_CH_CTRL_DM_0 (0U << 25)
#define LPC_DMA_CH_CTRL_DM_1 (1U << 25)

#define LPC_DMA_CH_CTRL_SI (1U << 26)
#define LPC_DMA_CH_CTRL_DI (1U << 27)
#define LPC_DMA_CH_CTRL_ITC (1U << 31)

/** @} */

/**
 * @name DMA Channel Configuration Register Defines
 *
 * @{
 */

#define LPC_DMA_CH_CFG_EN (1U << 0)

#define LPC_DMA_CH_CFG_SPER_MASK (0xfU << 1)
#define LPC_DMA_CH_CFG_SPER_SHIFT 1
#define LPC_DMA_CH_CFG_SPER_0 (0x0U << 1)
#define LPC_DMA_CH_CFG_SPER_1 (0x1U << 1)
#define LPC_DMA_CH_CFG_SPER_2 (0x2U << 1)
#define LPC_DMA_CH_CFG_SPER_3 (0x3U << 1)
#define LPC_DMA_CH_CFG_SPER_4 (0x4U << 1)
#define LPC_DMA_CH_CFG_SPER_5 (0x5U << 1)
#define LPC_DMA_CH_CFG_SPER_6 (0x6U << 1)
#define LPC_DMA_CH_CFG_SPER_7 (0x7U << 1)
#define LPC_DMA_CH_CFG_SPER_8 (0x8U << 1)
#define LPC_DMA_CH_CFG_SPER_9 (0x9U << 1)
#define LPC_DMA_CH_CFG_SPER_10 (0xaU << 1)
#define LPC_DMA_CH_CFG_SPER_11 (0xbU << 1)
#define LPC_DMA_CH_CFG_SPER_12 (0xcU << 1)
#define LPC_DMA_CH_CFG_SPER_13 (0xdU << 1)
#define LPC_DMA_CH_CFG_SPER_14 (0xeU << 1)
#define LPC_DMA_CH_CFG_SPER_15 (0xfU << 1)

#define LPC_DMA_CH_CFG_DPER_MASK (0xfU << 6)
#define LPC_DMA_CH_CFG_DPER_SHIFT 6
#define LPC_DMA_CH_CFG_DPER_0 (0x0U << 6)
#define LPC_DMA_CH_CFG_DPER_1 (0x1U << 6)
#define LPC_DMA_CH_CFG_DPER_2 (0x2U << 6)
#define LPC_DMA_CH_CFG_DPER_3 (0x3U << 6)
#define LPC_DMA_CH_CFG_DPER_4 (0x4U << 6)
#define LPC_DMA_CH_CFG_DPER_5 (0x5U << 6)
#define LPC_DMA_CH_CFG_DPER_6 (0x6U << 6)
#define LPC_DMA_CH_CFG_DPER_7 (0x7U << 6)
#define LPC_DMA_CH_CFG_DPER_8 (0x8U << 6)
#define LPC_DMA_CH_CFG_DPER_9 (0x9U << 6)
#define LPC_DMA_CH_CFG_DPER_10 (0xaU << 6)
#define LPC_DMA_CH_CFG_DPER_11 (0xbU << 6)
#define LPC_DMA_CH_CFG_DPER_12 (0xcU << 6)
#define LPC_DMA_CH_CFG_DPER_13 (0xdU << 6)
#define LPC_DMA_CH_CFG_DPER_14 (0xeU << 6)
#define LPC_DMA_CH_CFG_DPER_15 (0xfU << 6)

#define LPC_DMA_CH_CFG_FLOW_MASK (0x7U << 11)
#define LPC_DMA_CH_CFG_FLOW_MEM_TO_MEM_DMA (0x0U << 11)
#define LPC_DMA_CH_CFG_FLOW_MEM_TO_PER_DMA (0x1U << 11)
#define LPC_DMA_CH_CFG_FLOW_PER_TO_MEM_DMA (0x2U << 11)
#define LPC_DMA_CH_CFG_FLOW_PER_TO_PER_DMA (0x3U << 11)
#define LPC_DMA_CH_CFG_FLOW_PER_TO_PER_DEST (0x4U << 11)
#define LPC_DMA_CH_CFG_FLOW_MEM_TO_PER_PER (0x5U << 11)
#define LPC_DMA_CH_CFG_FLOW_PER_TO_MEM_PER (0x6U << 11)
#define LPC_DMA_CH_CFG_FLOW_PER_TO_PER_SRC (0x7U << 11)

#define LPC_DMA_CH_CFG_IE (1U << 14)
#define LPC_DMA_CH_CFG_ITC (1U << 15)
#define LPC_DMA_CH_CFG_LOCK (1U << 16)
#define LPC_DMA_CH_CFG_ACTIVE (1U << 17)
#define LPC_DMA_CH_CFG_HALT (1U << 18)

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_LPC_DMA_H */
