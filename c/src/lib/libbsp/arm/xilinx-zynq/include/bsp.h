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

#ifndef LIBBSP_ARM_XILINX_ZYNQ_BSP_H
#define LIBBSP_ARM_XILINX_ZYNQ_BSP_H

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_ARM_A9MPCORE_SCU_BASE 0xf8f00000

#define BSP_ARM_GIC_CPUIF_BASE 0xf8f00100

#define BSP_ARM_A9MPCORE_PT_BASE 0xf8f00600

#define BSP_ARM_GIC_DIST_BASE 0xf8f01000

typedef enum {
  BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_INSTALL,
  BSP_ARM_A9MPCORE_FATAL_CLOCK_IRQ_REMOVE
} zynq_fatal_code;

void zynq_fatal(zynq_fatal_code code) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_BSP_H */
