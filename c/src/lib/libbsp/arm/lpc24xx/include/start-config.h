/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief BSP start configuration.
 */

/*
 * Copyright (c) 2011-2012 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_LPC24XX_START_CONFIG_H
#define LIBBSP_ARM_LPC24XX_START_CONFIG_H

#include <rtems/score/armv7m.h>

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/start.h>
#include <bsp/lpc-emc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Pico seconds @a ps to clock ticks for clock frequency @a f.
 */
#define LPC24XX_PS_TO_CLK(ps, f) \
  (((((uint64_t) (ps)) * ((uint64_t) (f))) + 1000000000000ULL - 1ULL) \
    / 1000000000000ULL)

/**
 * @brief Pico seconds @a ps to EMCCLK clock ticks adjusted by @a m.
 */
#define LPC24XX_PS_TO_EMCCLK(ps, m) \
  (LPC24XX_PS_TO_CLK(ps, LPC24XX_EMCCLK) > (m) ? \
    LPC24XX_PS_TO_CLK(ps, LPC24XX_EMCCLK) - (m) : 0)

typedef struct {
  uint32_t refresh;
  uint32_t readconfig;
  uint32_t trp;
  uint32_t tras;
  uint32_t tsrex;
  uint32_t tapr;
  uint32_t tdal;
  uint32_t twr;
  uint32_t trc;
  uint32_t trfc;
  uint32_t txsr;
  uint32_t trrd;
  uint32_t tmrd;
  uint32_t emcdlyctl;
} lpc24xx_emc_dynamic_config;

typedef struct {
  volatile lpc_emc_dynamic *chip_select;
  uint32_t address;
  uint32_t config;
  uint32_t rascas;
  uint32_t mode;
} lpc24xx_emc_dynamic_chip_config;

typedef struct {
  volatile lpc_emc_static *chip_select;
  struct {
    uint32_t config;
    uint32_t waitwen;
    uint32_t waitoen;
    uint32_t waitrd;
    uint32_t waitpage;
    uint32_t waitwr;
    uint32_t waitrun;
  } config;
} lpc24xx_emc_static_chip_config;

extern BSP_START_DATA_SECTION const lpc24xx_pin_range
  lpc24xx_start_config_pinsel [];

extern BSP_START_DATA_SECTION const lpc24xx_emc_dynamic_config
  lpc24xx_start_config_emc_dynamic [];

extern BSP_START_DATA_SECTION const lpc24xx_emc_dynamic_chip_config
  lpc24xx_start_config_emc_dynamic_chip [];

extern BSP_START_DATA_SECTION const size_t
  lpc24xx_start_config_emc_dynamic_chip_count;

extern BSP_START_DATA_SECTION const lpc24xx_emc_static_chip_config
  lpc24xx_start_config_emc_static_chip [];

extern BSP_START_DATA_SECTION const size_t
  lpc24xx_start_config_emc_static_chip_count;

extern BSP_START_DATA_SECTION const ARMV7M_MPU_Region
  lpc24xx_start_config_mpu_region [];

extern BSP_START_DATA_SECTION const size_t
  lpc24xx_start_config_mpu_region_count;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_START_CONFIG_H */
