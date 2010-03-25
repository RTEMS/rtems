/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief System Integration Unit Access (SIU).
 */

/*
 * Copyright (c) 2010
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <mpc55xx/regs.h>
#include <mpc55xx/mpc55xx.h>
#include <mpc55xx/siu.h>

rtems_status_code mpc55xx_siu_pcr_init(volatile struct SIU_tag *siu,
				       const mpc55xx_siu_pcr_entry_t *pcr_entry)
{
  int idx,cnt;
  /*
   * repeat, until end of list reached (pcr_cnt = 0)
   */
  while ((pcr_entry != NULL) &&
	 (pcr_entry->pcr_cnt > 0)) {
    idx = pcr_entry->pcr_idx;
    for (cnt = pcr_entry->pcr_cnt;cnt > 0;cnt--) {
      siu->PCR[idx++].R = pcr_entry->pcr_val.R;
    }
    pcr_entry++;
  }
  return RTEMS_SUCCESSFUL;
}
