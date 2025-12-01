/**
 * @file
 *
 * @ingroup RTEMSBSPsARMZynqMPRPU
 *
 * @brief This header file provides BSP-specific interfaces.
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

#ifndef LIBBSP_ARM_XILINX_ZYNQMP_BSP_H
#define LIBBSP_ARM_XILINX_ZYNQMP_BSP_H

/**
 * @defgroup RTEMSBSPsARMZynqMPRPU \
 *   AMD Zynq UltraScale+ MPSoC and RFSoC - Real-time Processing Unit
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief This group contains the BSP for the Real-time Processing Unit (RPU)
 *   contained in AMD Zynq UltraScale+ MPSoC and RFSoC devices.
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#include <dev/serial/zynq-uart-zynqmp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define BSP_ARM_GIC_CPUIF_BASE 0x00F9001000

#define BSP_ARM_GIC_DIST_BASE 0xF9000000

#if !defined(ZYNQMP_RPU_LOCK_STEP_MODE) && ZYNQMP_RPU_SPLIT_INDEX != 0
#define BSP_ARM_GIC_MULTI_PROCESSOR_SECONDARY
#endif

void zynqmp_debug_console_flush(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_ARM_XILINX_ZYNQMP_BSP_H */
