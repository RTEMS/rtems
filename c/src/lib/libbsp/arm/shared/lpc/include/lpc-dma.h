/**
 * @file
 *
 * @ingroup lpc_dma
 *
 * @brief DMA support API.
 */

/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_SHARED_LPC_DMA_H
#define LIBBSP_ARM_SHARED_LPC_DMA_H

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup lpc_dma DMA Support
 *
 * @ingroup lpc24xx
 * @ingroup lpc32xx
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
 * @name DMA Configuration Register
 *
 * @{
 */

#define DMA_CFG_E BSP_BIT32(0)
#define DMA_CFG_M_0 BSP_BIT32(1)
#define DMA_CFG_M_1 BSP_BIT32(2)

/** @} */

/**
 * @name DMA Channel Control Register
 *
 * @{
 */

#define DMA_CH_CTRL_TSZ(val) BSP_FLD32(val, 0, 11)
#define DMA_CH_CTRL_TSZ_MAX DMA_CH_CTRL_TSZ(0xfff)

#define DMA_CH_CTRL_SB(val) BSP_FLD32(val, 12, 14)
#define DMA_CH_CTRL_SB_1 DMA_CH_CTRL_SB(0)
#define DMA_CH_CTRL_SB_4 DMA_CH_CTRL_SB(1)
#define DMA_CH_CTRL_SB_8 DMA_CH_CTRL_SB(2)
#define DMA_CH_CTRL_SB_16 DMA_CH_CTRL_SB(3)
#define DMA_CH_CTRL_SB_32 DMA_CH_CTRL_SB(4)
#define DMA_CH_CTRL_SB_64 DMA_CH_CTRL_SB(5)
#define DMA_CH_CTRL_SB_128 DMA_CH_CTRL_SB(6)
#define DMA_CH_CTRL_SB_256 DMA_CH_CTRL_SB(7)

#define DMA_CH_CTRL_DB(val) BSP_FLD32(val, 15, 17)
#define DMA_CH_CTRL_DB_1 DMA_CH_CTRL_DB(0)
#define DMA_CH_CTRL_DB_4 DMA_CH_CTRL_DB(1)
#define DMA_CH_CTRL_DB_8 DMA_CH_CTRL_DB(2)
#define DMA_CH_CTRL_DB_16 DMA_CH_CTRL_DB(3)
#define DMA_CH_CTRL_DB_32 DMA_CH_CTRL_DB(4)
#define DMA_CH_CTRL_DB_64 DMA_CH_CTRL_DB(5)
#define DMA_CH_CTRL_DB_128 DMA_CH_CTRL_DB(6)
#define DMA_CH_CTRL_DB_256 DMA_CH_CTRL_DB(7)

#define DMA_CH_CTRL_SW(val) BSP_FLD32(val, 18, 20)
#define DMA_CH_CTRL_SW_8 DMA_CH_CTRL_SW(0)
#define DMA_CH_CTRL_SW_16 DMA_CH_CTRL_SW(1)
#define DMA_CH_CTRL_SW_32 DMA_CH_CTRL_SW(2)

#define DMA_CH_CTRL_DW(val) BSP_FLD32(val, 21, 23)
#define DMA_CH_CTRL_DW_8 DMA_CH_CTRL_DW(0)
#define DMA_CH_CTRL_DW_16 DMA_CH_CTRL_DW(1)
#define DMA_CH_CTRL_DW_32 DMA_CH_CTRL_DW(2)

#define DMA_CH_CTRL_S BSP_BIT32(24)
#define DMA_CH_CTRL_D BSP_BIT32(25)
#define DMA_CH_CTRL_SI BSP_BIT32(26)
#define DMA_CH_CTRL_DI BSP_BIT32(27)
#define DMA_CH_CTRL_PROT(val) BSP_FLD32(val, 28, 30)
#define DMA_CH_CTRL_I BSP_BIT32(31)

/** @} */

/**
 * @name DMA Channel Configuration Register
 *
 * @{
 */

#define DMA_CH_CFG_E BSP_BIT32(0)
#define DMA_CH_CFG_SPER(val) BSP_FLD32(val, 1, 5)
#define DMA_CH_CFG_DPER(val) BSP_FLD32(val, 6, 10)

#define DMA_CH_CFG_FLOW(val) BSP_FLD32(val, 11, 13)
#define DMA_CH_CFG_FLOW_MEM_TO_MEM_DMA DMA_CH_CFG_FLOW(0)
#define DMA_CH_CFG_FLOW_MEM_TO_PER_DMA DMA_CH_CFG_FLOW(1)
#define DMA_CH_CFG_FLOW_PER_TO_MEM_DMA DMA_CH_CFG_FLOW(2)
#define DMA_CH_CFG_FLOW_PER_TO_PER_DMA DMA_CH_CFG_FLOW(3)
#define DMA_CH_CFG_FLOW_PER_TO_PER_DEST DMA_CH_CFG_FLOW(4)
#define DMA_CH_CFG_FLOW_MEM_TO_PER_PER DMA_CH_CFG_FLOW(5)
#define DMA_CH_CFG_FLOW_PER_TO_MEM_PER DMA_CH_CFG_FLOW(6)
#define DMA_CH_CFG_FLOW_PER_TO_PER_SRC DMA_CH_CFG_FLOW(7)

#define DMA_CH_CFG_IE BSP_BIT32(14)
#define DMA_CH_CFG_ITC BSP_BIT32(15)
#define DMA_CH_CFG_L BSP_BIT32(16)
#define DMA_CH_CFG_A BSP_BIT32(17)
#define DMA_CH_CFG_H BSP_BIT32(18)

/** @} */

/**
 * @name LPC24XX DMA Peripherals
 *
 * @{
 */

#define LPC24XX_DMA_PER_SSP_0_TX 0
#define LPC24XX_DMA_PER_SSP_0_RX 1
#define LPC24XX_DMA_PER_SSP_1_TX 2
#define LPC24XX_DMA_PER_SSP_1_RX 3
#define LPC24XX_DMA_PER_SD_MMC 4
#define LPC24XX_DMA_PER_I2S_CH_0 5
#define LPC24XX_DMA_PER_I2S_CH_1 6

/** @} */

/**
 * @name LPC32XX DMA Peripherals
 *
 * @{
 */

#define LPC32XX_DMA_PER_I2S_0_CH_0 0
#define LPC32XX_DMA_PER_I2S_0_CH_1 13
#define LPC32XX_DMA_PER_I2S_1_CH_0 2
#define LPC32XX_DMA_PER_I2S_1_CH_1 10
#define LPC32XX_DMA_PER_NAND_0 1
#define LPC32XX_DMA_PER_NAND_1 12
#define LPC32XX_DMA_PER_SD_MMC 4
#define LPC32XX_DMA_PER_SSP_0_RX 14
#define LPC32XX_DMA_PER_SSP_0_TX 15
#define LPC32XX_DMA_PER_SSP_1_RX 3
#define LPC32XX_DMA_PER_SSP_1_TX 11
#define LPC32XX_DMA_PER_UART_1_RX 6
#define LPC32XX_DMA_PER_UART_1_TX 5
#define LPC32XX_DMA_PER_UART_2_RX 8
#define LPC32XX_DMA_PER_UART_2_TX 7
#define LPC32XX_DMA_PER_UART_7_RX 10
#define LPC32XX_DMA_PER_UART_7_TX 9

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_LPC_DMA_H */
