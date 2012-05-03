/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2011 Sebastien Bourdeauducq
 */

#ifndef _BSP_H
#define _BSP_H

#include <stdint.h>
#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_HAS_FRAME_BUFFER 1

/*
 * lm32 requires certain aligment of mbuf because unaligned uint32_t
 * accesses are not handled properly.
 */

#define CPU_U32_FIX

#if defined(RTEMS_NETWORKING)
#include <rtems/rtems_bsdnet.h>
struct rtems_bsdnet_ifconfig;
extern int rtems_minimac_driver_attach (struct rtems_bsdnet_ifconfig *config,
                                        int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_minimac_driver_attach
#define RTEMS_BSP_NETWORK_DRIVER_NAME "minimac0"
#endif

#ifdef __cplusplus
}
#endif

#endif
