/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RaspberryPi5
 *
 * @brief MMU Config Table
 */

/*
 * Copyright (C) 2026 Preetam Das <riki10112001@gmail.com>
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
#include <bsp/start.h>
#include <bsp/aarch64-mmu.h>
#include <bsp/raspberrypi5.h>

BSP_START_DATA_SECTION const aarch64_mmu_config_entry
aarch64_mmu_config_table[] = {
  AARCH64_MMU_DEFAULT_SECTIONS,

  { /* RPI peripheral address */
    .begin = (uintptr_t)RPI_PERIPHERAL_BASE,
    .end = (uintptr_t)RPI_PERIPHERAL_BASE + (uintptr_t)RPI_PERIPHERAL_SIZE,
    .flags = AARCH64_MMU_DEVICE
  },

  { /* RPI firmware-owned addresses including spintables */
    .begin = (uintptr_t)0x0,
    .end = (uintptr_t)0x1000,
    .flags = AARCH64_MMU_DEVICE
  },

};

BSP_START_DATA_SECTION const size_t aarch64_mmu_config_table_size =
  RTEMS_ARRAY_SIZE(aarch64_mmu_config_table);
