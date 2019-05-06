/**
 * @file
 *
 * @ingroup RTEMSBSPsMIPSCSB350
 *
 * @brief Global BSP definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_MIPS_CSB350_BSP_H
#define LIBBSP_MIPS_CSB350_BSP_H

/**
 * @defgroup RTEMSBSPsMIPSCSB350 CSB350
 *
 * @ingroup RTEMSBSPsMIPS
 *
 * @brief CSB350 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <libcpu/au1x00.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION
#define BSP_SHARED_HANDLER_SUPPORT      1

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;

int rtems_au1x00_emac_attach(struct rtems_bsdnet_ifconfig *config,
                             int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_au1x00_emac_attach

#ifdef __cplusplus
}
#endif

/** @} */

#endif
