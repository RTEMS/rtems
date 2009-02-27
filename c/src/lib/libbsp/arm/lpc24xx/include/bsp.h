/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Global BSP variables and functions.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_BSP_H
#define LIBBSP_ARM_LPC24XX_BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

/* Network driver configuration */

struct rtems_bsdnet_ifconfig;

int lpc24xx_eth_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	lpc24xx_eth_attach_detach

#define RTEMS_BSP_NETWORK_DRIVER_NAME "eth0"

#endif /* ASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_BSP_H */
