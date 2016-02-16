/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_ATSAM_BSP_H
#define LIBBSP_ARM_ATSAM_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (13 << 4)

#define BSP_ARMV7M_SYSTICK_PRIORITY (14 << 4)

uint32_t atsam_systick_frequency(void);

#define BSP_ARMV7M_SYSTICK_FREQUENCY atsam_systick_frequency()

struct rtems_bsdnet_ifconfig;

int if_atsam_attach(struct rtems_bsdnet_ifconfig *config, int attaching);

#define RTEMS_BSP_NETWORK_DRIVER_NAME "atsam0"

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH if_atsam_attach

/**
 * @brief Interface driver configuration.
 */
typedef struct {
  /**
   * @brief Maximum retries for MDIO communication.
   */
  uint32_t mdio_retries;

  /**
   * @brief Address of PHY.
   *
   * Use -1 to search for a PHY.
   */
  int phy_addr;
} if_atsam_config;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_BSP_H */
