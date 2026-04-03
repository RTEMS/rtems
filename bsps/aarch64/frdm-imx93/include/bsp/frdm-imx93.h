/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRTEMSBSPsAArch64FRDMIMX93
 *
 * @brief Peripheral definitions.
 */

/*
 * Copyright (C) 2026 Rongjie Lee
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

#ifndef LIBBSP_FRDM_IMX93_BSP_FRDM_IMX93_H
#define LIBBSP_FRDM_IMX93_BSP_FRDM_IMX93_H

#include <bspopts.h>
#include <stdint.h>
#include <bsp/utility.h>

/**
 * @name Register Macros
 *
 * @{
 */

#define IMX93_REG( x ) ( *(volatile uint64_t *) ( x ) )
#define IMX93_BIT( n ) ( 1 << ( n ) )

/** @} */

/**
 * @name Peripheral Base Register Address
 *
 * @{
 */

#define IMX93_PERIPHERAL_1_BASE 0x44000000 /* AIPS Peripheral Bridge 1(NS) */
#define IMX93_PERIPHERAL_1_SIZE 0x800000

#define IMX93_PERIPHERAL_2_BASE 0x42000000 /* AIPS Peripheral Bridge 2(NS) */
#define IMX93_PERIPHERAL_2_SIZE 0x800000

#define IMX93_PERIPHERAL_3_BASE 0x42800000 /* AIPS Peripheral Bridge 3(NS) */
#define IMX93_PERIPHERAL_3_SIZE 0x800000

#define IMX93_PERIPHERAL_4_BASE 0x49000000 /* AIPS Peripheral Bridge 4(NS) */
#define IMX93_PERIPHERAL_4_SIZE 0x800000

#define IMX93_GPIO_BASE 0x43810000 /* GPIO2-4(NS) */
#define IMX93_GPIO_SIZE 0x30000

#define IMX93_GIC_BASE 0x48000000 /* GIC Programming Registers(NS) */
#define IMX93_GIC_SIZE 0x1000000

#define IMX93_MEDIAMIX_BASE 0x4AC10000 /* MEDIAMIX Block Control(NS) */
#define IMX93_MEDIAMIX_SIZE 0x10000

#define IMX93_VIDEO_BASE 0x4AE00000
#define IMX93_VIDEO_SIZE 0x50000

#define IMX93_USB_BASE 0x4C100000 /* USB(NS) */
#define IMX93_USB_SIZE 0x200000

#define IMX93_LPUART1_BASE 0x44380000
#define IMX93_LPUART2_BASE 0x44390000

#define IMX93_LPUART3_BASE 0x42570000
#define IMX93_LPUART4_BASE 0x42580000
#define IMX93_LPUART5_BASE 0x42590000
#define IMX93_LPUART6_BASE 0x425A0000

#define IMX93_LPUART7_BASE 0x42690000
#define IMX93_LPUART8_BASE 0x426A0000

/** @} */

#endif /* LIBBSP_FRDM_IMX93_BSP_FRDM_IMX93_H */
