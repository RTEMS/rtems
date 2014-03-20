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

#ifndef LIBBSP_ARM_ALTERY_CYCLONE_V_BSP_H
#define LIBBSP_ARM_ALTERY_CYCLONE_V_BSP_H

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

#define BSP_ARM_A9MPCORE_SCU_BASE 0xFFFEC000

#define BSP_ARM_GIC_CPUIF_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00000100 )

#define BSP_ARM_A9MPCORE_GT_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00000200 )

#define BSP_ARM_GIC_DIST_BASE ( BSP_ARM_A9MPCORE_SCU_BASE + 0x00001000 )

#define BSP_ARM_L2CC_BASE 0xFFFEF000U

/* Forward declaration */
struct rtems_bsdnet_ifconfig;

/** @brief Network interface attach detach
 *
 * Attaches a network interface tp the network stack.
 * NOTE: Detaching is not supported!
 */
int altera_cyclone_v_network_if_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int                           attaching );

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH altera_cyclone_v_network_if_attach_detach
#define RTEMS_BSP_NETWORK_DRIVER_NAME "eth0"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_ALTERY_CYCLONE_V_BSP_H */
