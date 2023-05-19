/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMRealviewPBXA9
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H
#define LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H

/**
 * @defgroup RTEMSBSPsARMRealviewPBXA9 Realview PBX-A9
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Realview PBX-A9 Board Support Package.
 *
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_HAS_FRAME_BUFFER 1

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_A9MPCORE_SCU_BASE 0x1f000000

#define BSP_ARM_GIC_CPUIF_BASE 0x1f000100

#define BSP_ARM_A9MPCORE_GT_BASE 0x1f000200

#define BSP_ARM_A9MPCORE_PT_BASE 0x1f000600

#define BSP_ARM_GIC_DIST_BASE 0x1f001000

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/* @} */

#endif /* LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H */


