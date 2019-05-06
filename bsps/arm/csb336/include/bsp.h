/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCSB336
 *
 * @brief Global BSP definitions.
 */

/*
 *  BSP CSB336 header file
 *
 *  Copyright (c) 2004 Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
*/
#ifndef LIBBSP_ARM_CSB336_BSP_H
#define LIBBSP_ARM_CSB336_BSP_H

/**
 * @defgroup RTEMSBSPsARMCSB336 CSB336
 *
 * @ingroup RTEMSBSPsARM Board Support Package
 *
 * @brief CSB336 support package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <mc9328mxl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION

/* What is the input clock freq in hertz? */
#define BSP_OSC_FREQ  16000000    /* 16 MHz oscillator */
#define BSP_XTAL_FREQ 32768       /* 32.768 KHz crystal */

int get_perclk1_freq(void);

/**
 * @brief Network driver configuration
 */
struct rtems_bsdnet_ifconfig;

/* Change these to match your board */
int rtems_mc9328mxl_enet_attach(struct rtems_bsdnet_ifconfig *config,
                                void *chip);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_mc9328mxl_enet_attach

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _BSP_H */

