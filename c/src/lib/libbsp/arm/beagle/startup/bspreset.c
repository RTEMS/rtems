/*
 * Copyright (c) 2014 Ben Gras <beng@shrike-systems.com>. All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#define AM335X_CM_BASE 0x44E00000
#define AM335X_CM_SIZE 0x1000

#define AM335X_PRM_DEVICE_OFFSET 0xf00
#define AM335X_PRM_RSTCTRL_REG 0x00
#define AM335X_RST_GLOBAL_WARM_SW_BIT 0

#define DM37XX_CM_BASE 0x48307000
#define DM37XX_CM_SIZE 0x1000
#define DM37XX_PRM_RSTCTRL_REG 0x250
#define DM37XX_RST_DPLL3_BIT 2

void bsp_reset(void)
{
#if IS_DM3730
  static uint32_t reset_base = DM37XX_CM_BASE;
  while (true) {
    mmio_set((reset_base + DM37XX_PRM_RSTCTRL_REG),
             (1 << DM37XX_RST_DPLL3_BIT));
  }
#endif

#if IS_AM335X
  static uint32_t reset_base = AM335X_CM_BASE;
  while (true) {
    mmio_set((reset_base + AM335X_PRM_DEVICE_OFFSET +
              AM335X_PRM_RSTCTRL_REG),
             (1 << AM335X_RST_GLOBAL_WARM_SW_BIT));
  }
#endif
}
