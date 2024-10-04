/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxVersal
 *
 * @brief This header file provides the core BSP definitions
 */

/*
 * Copyright (C) 2021 Gedare Bloom <gedare@rtems.org>
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

#ifndef LIBBSP_AARCH64_XILINX_VERSAL_BSP_H
#define LIBBSP_AARCH64_XILINX_VERSAL_BSP_H

/**
 * @addtogroup RTEMSBSPsAArch64
 *
 * @{
 */

#include <bspopts.h>

#define BSP_RESET_SMC

#ifndef ASM

#include <bsp/default-initial-extension.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_GIC_CPUIF_BASE 0xf9040000
#define BSP_ARM_GIC_DIST_BASE 0xf9000000
#define BSP_ARM_GIC_REDIST_BASE 0xf9080000

/*
 * DDRMC mapping
 */
LINKER_SYMBOL(bsp_r0_ram_base)
LINKER_SYMBOL(bsp_r0_ram_end)
LINKER_SYMBOL(bsp_r1_ram_base)
LINKER_SYMBOL(bsp_r1_ram_end)

/**
 * @brief Versal specific set up of the MMU.
 *
 * Provide in the application to override the defaults in the BSP.
 */
BSP_START_TEXT_SECTION void versal_setup_mmu_and_cache(void);

void versal_debug_console_flush(void);

uint32_t versal_clock_i2c0(void);

uint32_t versal_clock_i2c1(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_AARCH64_XILINX_VERSAL_BSP_H */
