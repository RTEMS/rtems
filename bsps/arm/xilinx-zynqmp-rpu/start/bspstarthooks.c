/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMZynqMPRPU
 *
 * @brief This source file contains the implementation of bsp_start_hook_0().
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
 * Copyright (C) 2023 Reflex Aerospace GmbH
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

#include <bsp/start.h>

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION
#define ARMV7_PMSA_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp/memory.h>
#include <libcpu/arm-cp15.h>

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
  /* Do nothing */
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  uint32_t index = 0;

  for (size_t i = 0; i < zynqmp_mpu_region_count; ++i) {
    const ARMV7_PMSA_Region *region = &zynqmp_mpu_regions[i];
    index = _ARMV7_PMSA_Add_regions(
      index,
      region->begin,
      region->size,
      region->attributes
    );
  }

  arm_cp15_instruction_cache_invalidate();
  arm_cp15_data_cache_all_invalidate();
  _ARM_Data_synchronization_barrier();
  _ARM_Instruction_synchronization_barrier();

  uint32_t control = arm_cp15_get_control();
  control &= ~ARM_CP15_CTRL_A;
  control &= ~ARM_CP15_CTRL_V;
  control |= ARM_CP15_CTRL_M;
  control |= ARM_CP15_CTRL_C;
  control |= ARM_CP15_CTRL_I;
  control |= ARM_CP15_CTRL_Z;
  arm_cp15_set_control(control);
  _ARM_Data_synchronization_barrier();
  _ARM_Instruction_synchronization_barrier();

  bsp_start_clear_bss();
}
