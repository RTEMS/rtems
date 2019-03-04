/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCVirtex
 *
 * @brief Global BSP definitions.
 */

/*  bsp.h
 *
 *  This include file contains all GEN405 board IO definitions.
 *
 * derived from helas403/include/bsp.h:
 *  Id: bsp.h,v 1.4 2001/06/18 17:01:48 joel Exp
 *  Author:	Thomas Doerfler <td@imd.m.isar.de>
 *              IMD Ingenieurbuero fuer Microcomputertechnik
 *
 *  COPYRIGHT (c) 1998 by IMD
 *
 *  Changes from IMD are covered by the original distributions terms.
 *  This file has been derived from the papyrus BSP.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/include/bsp.h
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef LIBBSP_POWERPC_VIRTEX_BSP_H
#define LIBBSP_POWERPC_VIRTEX_BSP_H

/**
 * @defgroup RTEMSBSPsPowerPCVirtex Xilinx Virtex
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief Xilinx Virtex Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION

/* Network Defines */
#if 1 /* EB/doe changes */
#define RTEMS_BSP_NETWORK_DRIVER_NAME     "eth0"
#else
#include "xiltemac.h"
#define RTEMS_BSP_NETWORK_DRIVER_NAME     XILTEMAC_DRIVER_PREFIX
#endif
struct rtems_bsdnet_ifconfig;
extern int xilTemac_driver_attach(struct rtems_bsdnet_ifconfig*, int );
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH xilTemac_driver_attach

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif
