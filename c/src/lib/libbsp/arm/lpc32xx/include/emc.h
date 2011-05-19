/**
 * @file
 *
 * @ingroup lpc32xx_emc
 *
 * @brief EMC support API.
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

#ifndef LIBBSP_ARM_LPC32XX_EMC_H
#define LIBBSP_ARM_LPC32XX_EMC_H

#include <rtems.h>

#include <bsp/lpc-emc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup lpc_emc
 *
 * @brief EMC Support
 *
 * @{
 */

/**
 * @name SDRAM Clock Control Register (SDRAMCLK_CTRL)
 *
 * @{
 */

#define SDRAMCLK_CLOCKS_DIS BSP_BIT32(0)
#define SDRAMCLK_DDR_MODE BSP_BIT32(1)
#define SDRAMCLK_DDR_DQSIN_DELAY(val) BSP_FLD32(val, 2, 6)
#define SDRAMCLK_RTC_TICK_EN BSP_BIT32(7)
#define SDRAMCLK_SW_DDR_CAL BSP_BIT32(8)
#define SDRAMCLK_CAL_DELAY BSP_BIT32(9)
#define SDRAMCLK_SENSITIVITY_FACTOR(val) BSP_FLD32(val, 10, 12)
#define SDRAMCLK_DCA_STATUS BSP_BIT32(13)
#define SDRAMCLK_COMMAND_DELAY(val) BSP_FLD32(val, 14, 18)
#define SDRAMCLK_SW_DDR_RESET BSP_BIT32(19)
#define SDRAMCLK_PIN_1_FAST BSP_BIT32(20)
#define SDRAMCLK_PIN_2_FAST BSP_BIT32(21)
#define SDRAMCLK_PIN_3_FAST BSP_BIT32(22)

/** @} */

/**
 * @name EMC AHB Control Register (EMCAHBControl)
 *
 * @{
 */

#define EMC_AHB_PORT_BUFF_EN BSP_BIT32(0)

/** @} */

/**
 * @name EMC AHB Timeout Register (EMCAHBTimeOut)
 *
 * @{
 */

#define EMC_AHB_TIMEOUT(val) BSP_FLD32(val, 0, 9)

/** @} */

/**
 * @name SDRAM Mode and Extended Mode Registers
 *
 * @{
 */

#define SDRAM_ADDR_ROW_16MB(val) ((uint32_t) (val) << 10)
#define SDRAM_ADDR_ROW_32MB(val) ((uint32_t) (val) << 11)
#define SDRAM_ADDR_ROW_64MB(val) ((uint32_t) (val) << 11)

#define SDRAM_ADDR_BANK_16MB(ba1, ba0) \
  (((uint32_t) (ba1) << 23) | ((uint32_t) (ba0) << 22))
#define SDRAM_ADDR_BANK_32MB(ba1, ba0) \
  (((uint32_t) (ba1) << 23) | ((uint32_t) (ba0) << 24))
#define SDRAM_ADDR_BANK_64MB(ba1, ba0) \
  (((uint32_t) (ba1) << 25) | ((uint32_t) (ba0) << 24))

#define SDRAM_MODE_16MB(mode) \
  (SDRAM_ADDR_BANK_16MB(0, 0) | SDRAM_ADDR_ROW_16MB(mode))
#define SDRAM_MODE_32MB(mode) \
  (SDRAM_ADDR_BANK_32MB(0, 0) | SDRAM_ADDR_ROW_32MB(mode))
#define SDRAM_MODE_64MB(mode) \
  (SDRAM_ADDR_BANK_64MB(0, 0) | SDRAM_ADDR_ROW_64MB(mode))

#define SDRAM_EXTMODE_16MB(mode) \
  (SDRAM_ADDR_BANK_16MB(1, 0) | SDRAM_ADDR_ROW_16MB(mode))
#define SDRAM_EXTMODE_32MB(mode) \
  (SDRAM_ADDR_BANK_32MB(1, 0) | SDRAM_ADDR_ROW_32MB(mode))
#define SDRAM_EXTMODE_64MB(mode) \
  (SDRAM_ADDR_BANK_64MB(1, 0) | SDRAM_ADDR_ROW_64MB(mode))

#define SDRAM_MODE_BURST_LENGTH(val) BSP_FLD32(val, 0, 2)
#define SDRAM_MODE_BURST_INTERLEAVE BSP_BIT32(3)
#define SDRAM_MODE_CAS(val) BSP_FLD32(val, 4, 6)
#define SDRAM_MODE_TEST_MODE(val) BSP_FLD32(val, 7, 8)
#define SDRAM_MODE_WRITE_BURST_SINGLE_BIT BSP_BIT32(9)

#define SDRAM_EXTMODE_PASR(val) BSP_FLD32(val, 0, 2)
#define SDRAM_EXTMODE_DRIVER_STRENGTH(val) BSP_FLD32(val, 5, 6)

/** @} */

typedef struct {
  uint32_t size;
  uint32_t config;
  uint32_t rascas;
  uint32_t mode;
  uint32_t extmode;
} lpc32xx_emc_dynamic_chip_config;

typedef struct {
  uint32_t sdramclk_ctrl;
  uint32_t nop_time_in_us;
  uint32_t control;
  uint32_t refresh;
  uint32_t readconfig;
  uint32_t trp;
  uint32_t tras;
  uint32_t tsrex;
  uint32_t twr;
  uint32_t trc;
  uint32_t trfc;
  uint32_t txsr;
  uint32_t trrd;
  uint32_t tmrd;
  uint32_t tcdlr;
  lpc32xx_emc_dynamic_chip_config chip [EMC_DYN_CHIP_COUNT];
} lpc32xx_emc_dynamic_config;

void lpc32xx_emc_init(const lpc32xx_emc_dynamic_config *dyn_cfg);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_EMC_H */
