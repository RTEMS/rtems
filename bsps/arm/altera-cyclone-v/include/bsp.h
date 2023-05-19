/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (C) 2013, 2018 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H
#define LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H

/**
 * @defgroup RTEMSBSPsARMCycV Intel Cyclone V
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Intel Cyclone V Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_A9MPCORE_SCU_BASE 0xFFFEC000

#define BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0

#define BSP_ARM_GIC_CPUIF_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00000100 )

#define BSP_ARM_A9MPCORE_GT_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00000200 )

#define BSP_ARM_GIC_DIST_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00001000 )

#ifndef BSP_ARM_A9MPCORE_PERIPHCLK
extern uint32_t altera_cyclone_v_a9mpcore_periphclk;
#define BSP_ARM_A9MPCORE_PERIPHCLK altera_cyclone_v_a9mpcore_periphclk
#define ALTERA_CYCLONE_V_NEED_A9MPCORE_PERIPHCLK
#endif

#define BSP_ARM_L2C_310_BASE 0xfffef000

#define BSP_ARM_L2C_310_ID 0x410000c9

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/* @} */

#endif /* LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H */
