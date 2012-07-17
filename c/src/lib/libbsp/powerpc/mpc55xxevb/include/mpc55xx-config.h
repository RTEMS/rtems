/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Low-level configuration.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_POWERPC_MPC55XXEVB_MPC55XX_CONFIG_H
#define LIBBSP_POWERPC_MPC55XXEVB_MPC55XX_CONFIG_H

#include <stddef.h>

#include <libcpu/powerpc-utility.h>

#include <bsp/start.h>

#include <mpc55xx/regs.h>
#include <mpc55xx/regs-mmu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uint32_t index : 10;
  uint32_t count : 10;
  uint32_t output : 1;
  union SIU_PCR_tag pcr;
} mpc55xx_siu_pcr_config;

extern const mpc55xx_siu_pcr_config mpc55xx_start_config_siu_pcr [];

extern const size_t mpc55xx_start_config_siu_pcr_count [];

extern const struct MMU_tag mpc55xx_start_config_mmu_early [];

extern const size_t mpc55xx_start_config_mmu_early_count [];

extern const struct MMU_tag mpc55xx_start_config_mmu [];

extern const size_t mpc55xx_start_config_mmu_count [];

#ifdef MPC55XX_HAS_FMPLL
  typedef struct {
    union FMPLL_SYNCR_tag syncr_tmp;
    union FMPLL_SYNCR_tag syncr_final;
  } mpc55xx_clock_config;
#endif

#ifdef MPC55XX_HAS_FMPLL_ENHANCED
  typedef struct {
    union FMPLL_ESYNCR2_tag esyncr2_tmp;
    union FMPLL_ESYNCR2_tag esyncr2_final;
    union FMPLL_ESYNCR1_tag esyncr1_final;
  } mpc55xx_clock_config;
#endif

#ifdef MPC55XX_HAS_MODE_CONTROL
  typedef struct {
    struct {
      PLLD_CR_32B_tag cr;
      PLLD_MR_32B_tag mr;
    } fmpll [2];
    CGM_OC_EN_32B_tag oc_en;
    CGM_OCDS_SC_32B_tag ocds_sc;
    CGM_SC_DC0_3_32B_tag sc_dc0_3;
    CGM_AUXCLK_tag auxclk [5];
  } mpc55xx_clock_config;
#endif

extern const mpc55xx_clock_config mpc55xx_start_config_clock [];

#ifdef MPC55XX_HAS_EBI
  extern const struct EBI_CS_tag mpc55xx_start_config_ebi_cs [];

  extern const size_t mpc55xx_start_config_ebi_cs_count [];

  extern const struct EBI_CAL_CS_tag mpc55xx_start_config_ebi_cal_cs [];

  extern const size_t mpc55xx_start_config_ebi_cal_cs_count [];
#endif

void mpc55xx_start_early(void);

void mpc55xx_start_flash(void);

void mpc55xx_start_cache(void);

void mpc55xx_start_clock(void);

void mpc55xx_start_watchdog(void);

void mpc55xx_start_mmu_apply_config(const struct MMU_tag *config, size_t count);

uint32_t mpc55xx_get_system_clock(void);

LINKER_SYMBOL(bsp_ram_start)
LINKER_SYMBOL(bsp_ram_end)
LINKER_SYMBOL(bsp_ram_size)

LINKER_SYMBOL(bsp_ram_1_start)
LINKER_SYMBOL(bsp_ram_1_end)
LINKER_SYMBOL(bsp_ram_1_size)

LINKER_SYMBOL(bsp_rom_start)
LINKER_SYMBOL(bsp_rom_end)
LINKER_SYMBOL(bsp_rom_size)

#ifdef MPC55XX_BOOTFLAGS
  extern uint32_t mpc55xx_bootflag_0 [];
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_MPC55XXEVB_MPC55XX_CONFIG_H */
