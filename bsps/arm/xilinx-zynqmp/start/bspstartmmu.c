/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013 embedded brains GmbH
 *
 * Copyright (C) 2019 DornerWorks
 *
 * Written by Jeff Kubascik <jeff.kubascik@dornerworks.com>
 *        and Josh Whitehead <josh.whitehead@dornerworks.com>
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
zynqmp_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
#if defined(RTEMS_SMP)
  {
    .begin = 0xffff0000U,
    .end = 0xffffffffU,
    .flags = ARMV7_MMU_DEVICE
  },
#endif
  {
    .begin = 0xf9000000U,
    .end = 0xf9100000U,
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = 0xfd000000U,
    .end = 0xffc00000U,
    .flags = ARMV7_MMU_DEVICE
  }
};

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void zynqmp_setup_mmu_and_cache(void) __attribute__ ((weak));

BSP_START_TEXT_SECTION void zynqmp_setup_mmu_and_cache(void)
{
  uint32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_TRE | ARM_CP15_CTRL_A,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &zynqmp_mmu_config_table[0],
    RTEMS_ARRAY_SIZE(zynqmp_mmu_config_table)
  );
}
