/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef ATSAM_CLOCK_CONFIG_H
#define ATSAM_CLOCK_CONFIG_H

#include <rtems.h>
#include <bsp/linker-symbols.h>
#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if ATSAM_CHANGE_CLOCK_FROM_SRAM != 0
/* Note: fast_text is the simplest section to put this code into. Other
 * possibilities would include creating a new section. */
#define ATSAM_START_SRAM_SECTION BSP_FAST_TEXT_SECTION
#else
#define ATSAM_START_SRAM_SECTION
#endif

struct atsam_clock_config {
  /* Initialization value for the PMC_PLLAR. */
  uint32_t pllar_init;
  /* Initialization value for the PMC_MCKR. */
  uint32_t mckr_init;
  /* Resulting frequency in Hz. */
  uint32_t mck_freq;
};

ATSAM_START_SRAM_SECTION
extern const struct atsam_clock_config atsam_clock_config;

#define BOARD_MCK (atsam_clock_config.mck_freq)

typedef struct BOARD_Sdram_Config {
  uint32_t sdramc_tr;
  uint32_t sdramc_cr;
  uint32_t sdramc_mdr;
  uint32_t sdramc_cfr1;
  uint32_t sdramc_lpr;
} atsam_sdram_config;

ATSAM_START_SRAM_SECTION
extern const atsam_sdram_config BOARD_Sdram_Config;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ATSAM_CLOCK_CONFIG_H */
