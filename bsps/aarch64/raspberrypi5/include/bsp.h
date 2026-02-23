/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RaspberryPi5
 *
 * @brief This header file provides core definitions for the
 *   Raspberry Pi 5 BSP.
 */

/*
 * Copyright (C) 2026 Preetam Das <riki10112001@gmail.com>
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


#ifndef LIBBSP_AARCH64_RASPBERRYPI5_BSP_H
#define LIBBSP_AARCH64_RASPBERRYPI5_BSP_H

/**
 * @defgroup RTEMSBSPsAArch64RaspberryPi5 Raspberry Pi 5 - BCM2712
 * 
 * @ingroup RTEMSBSPsAArch64
 *    
 * @brief This group contains the BSP for the Raspberry Pi 5
 * 
 * @{
 */

#include <bspopts.h>

#ifndef ASM

#include <bsp/default-initial-extension.h>
#include <bsp/start.h>

#include <rtems.h>

#include <bsp/raspberrypi5.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Raspberry Pi 5 MMU initialization */
BSP_START_TEXT_SECTION void raspberrypi5_setup_mmu_and_cache(void);

#define BSP_ARM_GIC_CPUIF_BASE (BCM2712_GIC_CPUIF_BASE)
#define BSP_ARM_GIC_DIST_BASE  (BCM2712_GIC_DIST_BASE)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */ /* RTEMSBSPsAArch64RaspberryPi5 */

#endif /* LIBBSP_AARCH64_RASPBERRYPI5_BSP_H */
