/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H
#define LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H

/**
 * @defgroup RTEMSBSPsARMCycV Intel Cyclone V
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Intel Cyclone V Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_A9MPCORE_SCU_BASE 0xFFFEC000

#define BSP_ARM_GIC_ENABLE_FIQ_FOR_GROUP_0

#define BSP_ARM_GIC_CPUIF_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00000100 )

#define BSP_ARM_A9MPCORE_GT_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00000200 )

#define BSP_ARM_GIC_DIST_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00001000 )

#ifndef BSP_ARM_A9MPCORE_PERIPHCLK
extern uint32_t altera_cyclone_v_a9mpcore_periphclk;
#define BSP_ARM_A9MPCORE_PERIPHCLK altera_cyclone_v_a9mpcore_periphclk
#define ALTERA_CYCLONE_V_NEED_A9MPCORE_PERIPHCLK
#endif

#define BSP_ARM_L2C_310_BASE 0xfffef000

#define BSP_ARM_L2C_310_ID 0x410000c9

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/* @} */

#endif /* LIBBSP_ARM_ALTERA_CYCLONE_V_BSP_H */
