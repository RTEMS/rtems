/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This header file provides internal APIs for managing ECC events.
 */

/*
 * Copyright (C) 2024 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_ECC_PRIV_H
#define LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_ECC_PRIV_H

/**
 * @addtogroup RTEMSBSPsAArch64
 *
 * @{
 */

#include <bspopts.h>

#ifndef ASM

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bsp/ecc.h>

/**
 * @brief Initialize ECC reporting support
 *
 * This initializes the base ECC event reporting support for the platform.
 */
void zynqmp_ecc_init( void );

/**
 * @brief Initialize BSP-specific ECC reporting
 *
 * Various BSPs may have different ECC capabilities. This allows those BSPs to
 * initialize those facilities as necessary.
 */
void zynqmp_ecc_init_bsp( void );

/**
 * @brief Configure Cache ECC reporting
 */
rtems_status_code zynqmp_configure_cache_ecc( void );

/**
 * @brief Configure On-Chip Memory (OCM) ECC reporting
 */
rtems_status_code zynqmp_configure_ocm_ecc( void );

/**
 * @brief Configure DDR Memory ECC reporting
 */
rtems_status_code zynqmp_configure_ddr_ecc( void );

/**
 * @brief Invoke the ECC error handler
 *
 * @param event The ECC error event type to be raised
 * @param data The details associated with the raised ECC error
 */
void zynqmp_invoke_ecc_handler( ECC_Event_Type event, void *data );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_ECC_PRIV_H */
