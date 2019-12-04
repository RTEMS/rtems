/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 DornerWorks
 * Written by Jeff Kubascik <jeff.kubascik@dornerworks.com>
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

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-a9mpcore-start.h>

BSP_START_DATA_SECTION static const arm_cp15_start_section_config
xen_bsp_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  {
    .begin = BSP_ARM_GIC_DIST_BASE,
    .end = BSP_ARM_GIC_DIST_BASE + BSP_ARM_GIC_DIST_LENGTH,
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = BSP_ARM_GIC_CPUIF_BASE,
    .end = BSP_ARM_GIC_CPUIF_BASE + BSP_ARM_GIC_CPUIF_LENGTH,
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = BSP_XEN_VPL011_BASE,
    .end = BSP_XEN_VPL011_BASE + BSP_XEN_VPL011_LENGTH,
    .flags = ARMV7_MMU_DEVICE
  }
};

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void bsp_xen_setup_mmu_and_cache(void) __attribute__ ((weak));

BSP_START_TEXT_SECTION void bsp_xen_setup_mmu_and_cache(void)
{
  uint32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_TRE | ARM_CP15_CTRL_A,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &xen_bsp_mmu_config_table[0],
    RTEMS_ARRAY_SIZE(xen_bsp_mmu_config_table)
  );
}
