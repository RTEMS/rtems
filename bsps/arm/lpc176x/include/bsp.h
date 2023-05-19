/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (C) 2008, 2013 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_LPC176X_BSP_H
#define LIBBSP_ARM_LPC176X_BSP_H

/**
 * @defgroup RTEMSBSPsARMLPC176X NXP LPC176X
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief NXP LPC176X Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#define LPC176X_PCLK ( LPC176X_CCLK / LPC176X_PCLKDIV )
#define LPC176X_MPU_REGION_COUNT 8u

#define BSP_FEATURE_IRQ_EXTENSION
#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT ( 29u << 3u )
#define BSP_ARMV7M_SYSTICK_PRIORITY ( 30u << 3u )
#define BSP_ARMV7M_SYSTICK_FREQUENCY LPC176X_CCLK

#ifndef ASM

#include <rtems.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct rtems_bsdnet_ifconfig;

/**
 * @brief Optimized idle task.
 *
 * This idle task sets the power mode to idle.  This causes the processor
 * clock to be stopped, while on-chip peripherals remain active.
 * Any enabled interrupt from a peripheral or an external interrupt source
 *  will cause the processor to resume execution.
 *
 * To enable the idle task use the following in the system configuration:
 *
 * @code
 * #include <bsp.h>
 *
 * #define CONFIGURE_INIT
 *
 * #define CONFIGURE_IDLE_TASK_BODY bsp_idle_thread
 *
 * #include <confdefs.h>
 * @endcode
 */
void*bsp_idle_thread( uintptr_t ignored );

#define BSP_CONSOLE_UART_BASE 0x4000C000U

/**
 * @brief Restarts the bsp with "addr" address
 * @param addr Address used to restart the bsp
 */
void bsp_restart( const void *addr );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_ARM_LPC176X_BSP_H */
