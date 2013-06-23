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
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H
#define LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_HAS_FRAME_BUFFER 1

#ifndef ASM

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_A9MPCORE_SCU_BASE 0x1f000000

#define BSP_ARM_GIC_CPUIF_BASE 0x1f000100

#define BSP_ARM_A9MPCORE_PT_BASE 0x1f000600

#define BSP_ARM_GIC_DIST_BASE 0x1f001000

typedef enum {
  BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_INSTALL,
  BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_REMOVE,
  BSP_ARM_PL111_FATAL_REGISTER_DEV,
  BSP_ARM_PL111_FATAL_SEM_CREATE,
  BSP_ARM_PL111_FATAL_SEM_RELEASE
} rvpbxa9_fatal_code;

void rvpbxa9_fatal(rvpbxa9_fatal_code code) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_REALVIEW_PBX_A9_BSP_H */
