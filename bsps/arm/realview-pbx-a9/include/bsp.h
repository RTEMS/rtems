/**
 * @file
 *
 * @ingroup RTEMSBSPsARMRealviewPBXA9
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H
#define LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H

/**
 * @defgroup RTEMSBSPsARMRealviewPBXA9 Realview PBX-A9
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Realview PBX-A9 Board Support Package.
 *
 */

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_HAS_FRAME_BUFFER 1

#ifndef ASM

#include <rtems.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_A9MPCORE_SCU_BASE 0x1f000000

#define BSP_ARM_GIC_CPUIF_BASE 0x1f000100

#define BSP_ARM_A9MPCORE_GT_BASE 0x1f000200

#define BSP_ARM_A9MPCORE_PT_BASE 0x1f000600

#define BSP_ARM_GIC_DIST_BASE 0x1f001000

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/* @} */

#endif /* LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H */


