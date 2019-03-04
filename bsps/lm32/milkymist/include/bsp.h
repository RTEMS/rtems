/**
 * @file
 *
 * @ingroup RTEMSBSPsLM32Milkymist
 *
 * @brief Global BSP definitions.
 */

/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  COPYRIGHT (c) 2011 Sebastien Bourdeauducq
 */

#ifndef LIBBSP_LM32_MILKYMIST_BSP_H
#define LIBBSP_LM32_MILKYMIST_BSP_H

#include <stdint.h>
#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

/**
 * @defgroup RTEMSBSPsLM32Milkymist Milkymist
 * 
 * @ingroup RTEMSBSPsLM32
 * 
 * @brief Milkymist Board Support Package.
 *
 * @{
 */

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
struct rtems_bsdnet_ifconfig;
extern int rtems_minimac_driver_attach (struct rtems_bsdnet_ifconfig *config,
                                        int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_minimac_driver_attach
#define RTEMS_BSP_NETWORK_DRIVER_NAME "minimac0"
#endif

#ifdef __cplusplus
}
#endif

/** @} */

#endif
