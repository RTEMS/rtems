/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Xen
 *
 * @brief This source file contains the base BSP definitions.
 */

/*
 * Copyright (C) 2025 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBBSP_AARCH64_XEN_BSP_H
#define LIBBSP_AARCH64_XEN_BSP_H

/**
 * @addtogroup RTEMSBSPsARM
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_START_IMAGE_HEADER_LOAD_OFFSET BSP_XEN_LOAD_OFFSET

#ifndef ASM

#include <bsp/default-initial-extension.h>
#include <bsp/start.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_GIC_CPUIF_BASE 0x03002000
#define BSP_ARM_GIC_CPUIF_LENGTH 0x2000

#define BSP_ARM_GIC_DIST_BASE 0x03001000
#define BSP_ARM_GIC_DIST_LENGTH 0x1000

#define BSP_XEN_VPL011_BASE 0x22000000
#define BSP_XEN_VPL011_LENGTH 0x1000

BSP_START_TEXT_SECTION void xen_setup_mmu_and_cache( void );

/**
 * @brief Xen-specific set up of the MMU for non-primary
 * cores.
 *
 * Provide in the application to override the defaults in the BSP.
 */
BSP_START_TEXT_SECTION void xen_setup_secondary_cpu_mmu_and_cache( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_AARCH64_XEN_BSP_H */
