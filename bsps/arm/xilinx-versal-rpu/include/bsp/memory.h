/**
 * @file
 *
 * @ingroup RTEMSBSPsARMVersalRPU
 *
 * @brief This header file provides BSP-specific memory interfaces.
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

#ifndef LIBBSP_ARM_XILINX_VERSAL_RPU_BSP_MEMORY_H
#define LIBBSP_ARM_XILINX_VERSAL_RPU_BSP_MEMORY_H

#include <rtems/score/armv7-pmsa.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSBSPsARMVersalRPU
 *
 * @{
 */

extern char versal_memory_atcm_begin[];
extern char versal_memory_atcm_end[];
extern char versal_memory_atcm_size[];

extern char versal_memory_btcm_begin[];
extern char versal_memory_btcm_end[];
extern char versal_memory_btcm_size[];

extern char versal_memory_ddr_begin[];
extern char versal_memory_ddr_end[];
extern char versal_memory_ddr_size[];

extern char versal_memory_nocache_begin[];
extern char versal_memory_nocache_end[];
extern char versal_memory_nocache_size[];

extern char versal_memory_devps_begin[];
extern char versal_memory_devps_end[];
extern char versal_memory_devps_size[];

extern char versal_memory_ocm_begin[];
extern char versal_memory_ocm_end[];
extern char versal_memory_ocm_size[];

/**
 * @brief Versal Adaptive SoC specific set up of the MMU.
 *
 * Provide in the application to override the defaults in the BSP.
 */
void versal_setup_mpu_and_cache(void);

/**
 * @brief This table defines the Versal Adaptive SoC specific MPU
 * regions.
 *
 * The table entry count is provided by ::versal_mpu_region_count.
 *
 * The application may provide this table to override the defaults in the BSP.
 */
extern const ARMV7_PMSA_Region versal_mpu_regions[];

/**
 * @brief This constant contains the entry count of the ::versal_mpu_regions
 *   table.
 */
extern const size_t versal_mpu_region_count;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_VERSAL_RPU_BSP_MEMORY_H */
