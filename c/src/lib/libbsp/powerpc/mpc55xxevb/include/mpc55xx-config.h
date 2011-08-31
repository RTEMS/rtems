/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief MPC55XX low-level configuration.
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
 *
 * $Id$
 */

#ifndef LIBBSP_POWERPC_MPC55XXEVB_MPC55XX_CONFIG_H
#define LIBBSP_POWERPC_MPC55XXEVB_MPC55XX_CONFIG_H

#include <stddef.h>

#include <mpc55xx/regs.h>
#include <mpc55xx/regs-mmu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uint16_t index;
  uint16_t count;
  union SIU_PCR_tag pcr;
} mpc55xx_siu_pcr_config_entry;

extern const mpc55xx_siu_pcr_config_entry mpc55xx_siu_pcr_config [];

extern const size_t mpc55xx_siu_pcr_config_count [];

extern const struct MMU_tag mpc55xx_mmu_config [];

extern const size_t mpc55xx_mmu_config_count [];

extern const struct EBI_CS_tag mpc55xx_ebi_cs_config [];

extern const size_t mpc55xx_ebi_cs_config_count [];

extern const struct EBI_CAL_CS_tag mpc55xx_ebi_cal_cs_config [];

extern const size_t mpc55xx_ebi_cal_cs_config_count [];

void mpc55xx_early_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_MPC55XXEVB_MPC55XX_CONFIG_H */
