/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMZynqMPRPU
 *
 * @brief This source file contains the implementation of
 * zynqmp_setup_mpu_and_cache().
 */

/*
 * Copyright (C) 2023 Reflex Aerospace GmbH
 *
 * Written by Philip Kirkpatrick <p.kirkpatrick@reflexaerospace.com>
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

#include <bsp/memory.h>
#include <bsp/start.h>

BSP_START_DATA_SECTION const ARMV7_PMSA_Region
zynqmp_mpu_regions[] = {
  {
    .begin = (uintptr_t)zynqmp_memory_atcm_begin,
    .size = (uintptr_t)zynqmp_memory_atcm_size,
    .attributes = ARMV7_PMSA_READ_WRITE_UNCACHED
  }, {
    .begin = (uintptr_t)zynqmp_memory_btcm_begin,
    .size = (uintptr_t)zynqmp_memory_btcm_size,
    .attributes = ARMV7_PMSA_READ_WRITE_UNCACHED
  }, {
    .begin = (uintptr_t)zynqmp_memory_ddr_begin,
    .size = (uintptr_t)zynqmp_memory_ddr_size,
    .attributes = ARMV7_PMSA_READ_WRITE_CACHED
  }, {
    .begin = (uintptr_t)zynqmp_memory_devpl_begin,
    .size = (uintptr_t)zynqmp_memory_devpl_size,
    .attributes = ARMV7_PMSA_SHAREABLE_DEVICE
  }, {
    .begin = (uintptr_t)zynqmp_memory_devps_begin,
    .size = (uintptr_t)zynqmp_memory_devps_size,
    .attributes = ARMV7_PMSA_NON_SHAREABLE_DEVICE
  }, {
    .begin = (uintptr_t)zynqmp_memory_ocm_begin,
    .size = (uintptr_t)zynqmp_memory_ocm_size,
    .attributes = ARMV7_PMSA_NON_SHAREABLE_DEVICE
  }, {
    .begin = (uintptr_t)zynqmp_memory_nocache_begin,
    .size = (uintptr_t)zynqmp_memory_nocache_size,
    .attributes = ARMV7_PMSA_READ_WRITE_UNCACHED
  }
};

BSP_START_DATA_SECTION const size_t
zynqmp_mpu_region_count = RTEMS_ARRAY_SIZE(zynqmp_mpu_regions);
