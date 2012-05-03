/**
 *  @file
 *  
 *  This include file contains some definitions specific to the
 *  Cogent CSB350 Board.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <libcpu/au1x00.h>

#define BSP_FEATURE_IRQ_EXTENSION
#define BSP_SHARED_HANDLER_SUPPORT      1

/*
 * Network driver configuration
 */
extern struct rtems_bsdnet_ifconfig *config;

int rtems_au1x00_emac_attach(struct rtems_bsdnet_ifconfig *config,
                             int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_au1x00_emac_attach

#ifdef __cplusplus
}
#endif

#endif
