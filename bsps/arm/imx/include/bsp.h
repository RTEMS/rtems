/**
 * @file
 *
 * @ingroup RTEMSBSPsARMimx
 */

/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

void arm_generic_timer_get_config(uint32_t *frequency, uint32_t *irq);

void *imx_get_reg_of_node(const void *fdt, int node);

int imx_iomux_configure_pins(const void *fdt, uint32_t phandle);

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
