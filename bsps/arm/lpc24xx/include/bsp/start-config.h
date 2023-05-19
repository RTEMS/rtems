/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief BSP start configuration.
 */

/*
 * Copyright (C) 2011, 2012 embedded brains GmbH & Co. KG
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

#ifdef ARM_MULTILIB_ARCH_V7M

extern BSP_START_DATA_SECTION const ARMV7M_MPU_Region
  lpc24xx_start_config_mpu_region [];

extern BSP_START_DATA_SECTION const size_t
  lpc24xx_start_config_mpu_region_count;

#endif /* ARM_MULTILIB_ARCH_V7M */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_START_CONFIG_H */
