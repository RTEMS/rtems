/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMimx
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_IMX_BSP_H
#define LIBBSP_ARM_IMX_BSP_H

/**
 * @defgroup RTEMSBSPsARMimx NXP i.MX
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief NXP i.MX Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_FDT_IS_SUPPORTED

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern uintptr_t imx_gic_dist_base;

#define BSP_ARM_GIC_DIST_BASE imx_gic_dist_base

#define BSP_ARM_GIC_CPUIF_BASE (BSP_ARM_GIC_DIST_BASE + 0x1000)

#define BSP_ARM_A9MPCORE_GT_BASE 0

#define BSP_ARM_A9MPCORE_SCU_BASE 0

void *imx_get_reg_of_node(const void *fdt, int node);

rtems_vector_number imx_get_irq_of_node(
  const void *fdt,
  int node,
  size_t index
);

void imx_uart_console_drain(void);

/**
 * @brief Registers an IMX I2C bus driver.
 *
 * @param[in] bus_path The I2C bus driver device path, e.g. "/dev/i2c-0".
 * @param[in] alias_or_path The FDT alias or path, e.g. "i2c0".
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The errno is set to indicate the error.
 */
int i2c_bus_register_imx(const char *bus_path, const char *alias_or_path);

/**
 * @brief Registers an IMX ECSPI bus driver.
 *
 * @param[in] bus_path The ECSPI bus driver device path, e.g. "/dev/spi-0".
 * @param[in] alias_or_path The FDT alias or path, e.g. "spi0".
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The errno is set to indicate the error.
 */
int spi_bus_register_imx(const char *bus_path, const char *alias_or_path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/* @} */

#endif /* LIBBSP_ARM_IMX_BSP_H */
