/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/arm-cp15-start.h>

const arm_cp15_start_section_config arm_cp15_start_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  { /* Periphery area */
    .begin = 0xFC000000U,
    .end   = 0x00000000U,
    .flags = ARMV7_MMU_DEVICE
  }
};

const size_t arm_cp15_start_mmu_config_table_size =
  RTEMS_ARRAY_SIZE(arm_cp15_start_mmu_config_table);
