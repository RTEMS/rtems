/**
 * @file
 * @ingroup lm32_milkymist_network RTEMSBSPsLM32SharedMilkymistOne
 * @brief Driver for Minimac ethernet
 */

/*  network.h
 *
 *  RTEMS driver for Minimac ethernet IP-core of Milkymist SoC
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis, France
 */

/**
 * @defgroup lm32_milkymist_network Minimac ethernet driver
 * @ingroup RTEMSBSPsLM32SharedMilkymistOne
 * @brief Driver for Minimac ethernet IP-core of Milkymist SoC
 * @{
 */

#ifndef __MILKYMIST_NETWORKING_H_
#define __MILKYMIST_NETWORKING_H_

int rtems_minimac_driver_attach (struct rtems_bsdnet_ifconfig *, int);

#endif
