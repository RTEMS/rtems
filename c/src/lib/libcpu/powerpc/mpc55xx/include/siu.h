/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief System Integration Unit Access (SIU).
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

#ifndef LIBCPU_POWERPC_MPC55XX_SIU_H
#define LIBCPU_POWERPC_MPC55XX_SIU_H

#include <rtems.h>

#include <mpc55xx/regs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct mpc55xx_siu_pcr_entry {
  uint16_t          pcr_idx; /* first PCR index for this entry      */
  uint16_t          pcr_cnt; /* PCR count using this entry          */
  union SIU_PCR_tag pcr_val; /* value to write to the PCR[idx++val] */
} mpc55xx_siu_pcr_entry_t;

rtems_status_code mpc55xx_siu_pcr_init(volatile struct SIU_tag *siu,
				       const mpc55xx_siu_pcr_entry_t *pcr_entry);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_SIU_H */
