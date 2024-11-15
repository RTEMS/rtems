/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This header file provides BSP-specific interfaces.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_H
#define LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_H

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_RESET_SMC

#define BSP_CPU_ON_USES_SMC

#ifndef ASM

#include <bsp/default-initial-extension.h>
#include <bsp/start.h>

#include <rtems.h>

#include <dev/serial/zynq-uart-zynqmp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct rtems_termios_device_context;

/**
 * @defgroup RTEMSBSPsAArch64XilinxZynqMP \
 *   AMD Zynq UltraScale+ MPSoC and RFSoC - Application Processing Unit
 *
 * @ingroup RTEMSBSPsAArch64
 *
 * @brief This group contains the BSP for the Application Processing Unit (APU)
 *   contained in AMD Zynq UltraScale+ MPSoC and RFSoC devices.
 *
 * @{
 */

/*
 * DDRMC mapping
 */
LINKER_SYMBOL(bsp_r0_ram_base)
LINKER_SYMBOL(bsp_r0_ram_end)
LINKER_SYMBOL(bsp_r1_ram_base)
LINKER_SYMBOL(bsp_r1_ram_end)

#define BSP_ARM_GIC_CPUIF_BASE 0xf9020000
#define BSP_ARM_GIC_DIST_BASE 0xf9010000

#define BSP_FDT_IS_SUPPORTED
extern unsigned int zynqmp_dtb_len;
extern unsigned char zynqmp_dtb[];

#define NANDPSU_BASEADDR 0xFF100000

/**
 * @brief Zynq UltraScale+ MPSoC specific set up of the MMU.
 *
 * Provide in the application to override the defaults in the BSP.
 */
BSP_START_TEXT_SECTION void zynqmp_setup_mmu_and_cache(void);

/**
 * @brief Zynq UltraScale+ MPSoC specific set up of the MMU for non-primary
 * cores.
 *
 * Provide in the application to override the defaults in the BSP.
 */
BSP_START_TEXT_SECTION void zynqmp_setup_secondary_cpu_mmu_and_cache( void );

void zynqmp_management_console_termios_init(void);

void zynqmp_debug_console_flush(void);

uint32_t zynqmp_clock_i2c0(void);

uint32_t zynqmp_clock_i2c1(void);

/**
 * @brief Zynq UltraScale+ MPSoC specific set up of a management console.
 *
 * Some systems may have a management interface which needs special
 * initialization. Provide in the application to override the defaults in the
 * BSP. This will only be called if the interface is found in the device tree.
 */
void zynqmp_configure_management_console(
  struct rtems_termios_device_context *base
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_H */
