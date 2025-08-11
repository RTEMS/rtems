/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup RTEMSBSPsARMSTM32F4
 * @brief Global BSP definitions.
 */

/*
 * Copyright (C) 2012 Sebastian Huber
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

#ifndef LIBBSP_ARM_STM32F4_BSP_H
#define LIBBSP_ARM_STM32F4_BSP_H

/**
 * @defgroup RTEMSBSPsARMSTM32F4 STM32F4
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief STM32F4 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (13 << 4)

#define BSP_ARMV7M_SYSTICK_PRIORITY (14 << 4)

#define BSP_ARMV7M_SYSTICK_FREQUENCY STM32F4_HCLK

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */


#endif /* LIBBSP_ARM_STM32F4_BSP_H */
