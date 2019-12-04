/**
 * @file
 *
 * @ingroup RTEMSBSPsARMxen
 *
 * @brief Global BSP definitions.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 DornerWorks
 * Written by Jeff Kubascik <jeff.kubascik@dornerworks.com>
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

#ifndef LIBBSP_ARM_XEN_BSP_H
#define LIBBSP_ARM_XEN_BSP_H

/**
 * @addtogroup RTEMSBSPsARM
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <bsp/default-initial-extension.h>
#include <bsp/start.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_GIC_CPUIF_BASE 0x03002000
#define BSP_ARM_GIC_CPUIF_LENGTH 0x1000

#define BSP_ARM_GIC_DIST_BASE 0x03001000
#define BSP_ARM_GIC_DIST_LENGTH 0x1000

#define BSP_ARM_A9MPCORE_SCU_BASE 0

#define BSP_ARM_A9MPCORE_GT_BASE 0

#define BSP_XEN_VPL011_BASE 0x22000000
#define BSP_XEN_VPL011_LENGTH 0x1000

void arm_generic_timer_get_config(uint32_t *frequency, uint32_t *irq);

BSP_START_TEXT_SECTION void bsp_xen_setup_mmu_and_cache(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_ARM_XEN_BSP_H */
