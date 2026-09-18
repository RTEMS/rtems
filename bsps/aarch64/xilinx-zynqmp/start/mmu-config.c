/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This source file contains the definition of ::aarch64_mmu_config_table
 *   and ::aarch64_mmu_config_table_size.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <bsp.h>
#include <bsp/aarch64-mmu.h>
#include <bsp/start.h>
#include <libcpu/mmu-vmsav8-64.h>

BSP_START_DATA_SECTION const aarch64_mmu_config_entry
aarch64_mmu_config_table[] = {
  AARCH64_MMU_DEFAULT_SECTIONS,
  {
    .begin = 0xf9000000U,
    .end = 0xf9100000U,
    .flags = AARCH64_MMU_DEVICE
  }, {
    .begin = 0xfd000000U,
    .end = 0xffc00000U,
    .flags = AARCH64_MMU_DEVICE
  /* Map OCM space */
  }, {
    .begin = 0xfffc0000U,
    /*
     * This should be 4GB, but uintptr_t can't hold that for ILP32. This will be
     * rounded up to MMU page alignment during initial setup and will end up
     * being 4GB anyway.
     */
    .end = 0xffffffffU,
    .flags = AARCH64_MMU_DATA_RW
  }, { /* DDRMC_region1_mem, if not used size is 0 and ignored */
    .begin = (uintptr_t) bsp_r1_ram_base,
    .end = (uintptr_t) bsp_r1_ram_end,
    .flags = AARCH64_MMU_DATA_RW_CACHED
  }, {
    .begin = 0xb0000000U,
    .end = 0xb0200000U,
    .flags = AARCH64_MMU_DATA_RW
  }
};

BSP_START_DATA_SECTION const size_t aarch64_mmu_config_table_size =
  RTEMS_ARRAY_SIZE(aarch64_mmu_config_table);
