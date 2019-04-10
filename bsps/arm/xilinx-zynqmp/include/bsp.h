/**
 * @file
 * @ingroup RTEMSBSPsARMZynqMP
 * @brief Global BSP definitions.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013, 2014 embedded brains GmbH
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

#ifndef LIBBSP_ARM_XILINX_ZYNQMP_BSP_H
#define LIBBSP_ARM_XILINX_ZYNQMP_BSP_H

/**
 * @defgroup RTEMSBSPsARMZynqMP Xilinx Zynq UltraScale+ MPSoC
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Xilinx Zynq UltraScale+ MPSoC Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>
#include <bsp/start.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_GIC_CPUIF_BASE 0xf9020000

#define BSP_ARM_GIC_DIST_BASE 0xf9010000

#define BSP_ARM_A9MPCORE_SCU_BASE 0

#define BSP_ARM_A9MPCORE_GT_BASE 0

/**
 * @brief Zynq UltraScale+ MPSoC specific set up of the MMU.
 *
 * Provide in the application to override the defaults in the BSP.
 */
BSP_START_TEXT_SECTION void zynqmp_setup_mmu_and_cache(void);

void arm_generic_timer_get_config(uint32_t *frequency, uint32_t *irq);

void zynqmp_debug_console_flush(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_ARM_XILINX_ZYNQMP_BSP_H */
