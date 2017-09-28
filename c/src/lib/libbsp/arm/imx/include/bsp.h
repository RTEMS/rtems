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

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_FDT_IS_SUPPORTED

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_GIC_DIST_BASE 0x31001000

#define BSP_ARM_GIC_CPUIF_BASE 0x31002000

#define BSP_ARM_A9MPCORE_GT_BASE 0

#define BSP_ARM_A9MPCORE_SCU_BASE 0

void arm_generic_timer_get_config(uint32_t *frequency, uint32_t *irq);

rtems_vector_number imx_get_irq_of_node(
  const void *fdt,
  int node,
  size_t index
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_IMX_BSP_H */
