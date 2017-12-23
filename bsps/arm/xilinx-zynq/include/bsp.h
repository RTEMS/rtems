/**
 * @file
 * @ingroup arm_zynq
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_XILINX_ZYNQ_BSP_H
#define LIBBSP_ARM_XILINX_ZYNQ_BSP_H

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>
#include <bsp/start.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup arm_zynq Xilinx-Zynq Support
 * @ingroup bsp_arm
 * @brief Xilinz-Zynq Board Support Package
 * @{
 */

#define BSP_ARM_A9MPCORE_SCU_BASE 0xf8f00000

#define BSP_ARM_GIC_CPUIF_BASE 0xf8f00100

#define BSP_ARM_A9MPCORE_GT_BASE 0xf8f00200

#define BSP_ARM_A9MPCORE_PT_BASE 0xf8f00600

#define BSP_ARM_GIC_DIST_BASE 0xf8f01000

#define BSP_ARM_L2C_310_BASE 0xf8f02000

#define BSP_ARM_L2C_310_ID 0x410000c8

/**
 * @brief Zynq specific set up of the MMU.
 *
 * Provide in the application to override
 * the defaults in the BSP. Note the defaults do not map in the GP0 and GP1
 * AXI ports. You should add the specific regions that map into your
 * PL rather than just open the whole of the GP[01] address space up.
 */
BSP_START_TEXT_SECTION void zynq_setup_mmu_and_cache(void);

uint32_t zynq_clock_cpu_1x(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_BSP_H */
