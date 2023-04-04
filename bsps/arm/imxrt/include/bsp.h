/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMimxrt
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_IMXRT_BSP_H
#define LIBBSP_ARM_IMXRT_BSP_H

/**
 * @defgroup RTEMSBSPsARMimxrt NXP i.MXRT
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief NXP i.MXRT Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_FDT_IS_SUPPORTED
extern const unsigned char imxrt_dtb[] __attribute__(( __aligned__(8) ));
extern const size_t imxrt_dtb_size;

void *imx_get_reg_of_node(const void *fdt, int node);

rtems_vector_number imx_get_irq_of_node(
  const void *fdt,
  int node,
  size_t index
);

bool imxrt_fdt_node_is_enabled(const void *fdt, int node);

/*
 * About 50% between `basepri` in arm_interrupt_disable and the maximum for this
 * chip.
 */
#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (13 << 4)
#define BSP_ARMV7M_SYSTICK_PRIORITY (14 << 4)

#define BSP_ARMV7M_SYSTICK_FREQUENCY imxrt_systick_frequency()
uint32_t imxrt_systick_frequency(void);

void imxrt_lpspi_init(void);
void imxrt_lpi2c_init(void);
void imxrt_ffec_init(void);
void BOARD_InitDEBUG_UARTPins(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* @} */

#endif /* LIBBSP_ARM_IMXRT_BSP_H */
