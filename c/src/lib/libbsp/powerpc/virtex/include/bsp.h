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
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>

/* miscellaneous stuff assumed to exist */
extern bool bsp_timer_internal_clock;   /* TRUE, when timer runs with CPU clk */

/* Network Defines */
#if 1 /* EB/doe changes */
#define RTEMS_BSP_NETWORK_DRIVER_NAME     "eth0"
#else
#include "xiltemac.h"
#define RTEMS_BSP_NETWORK_DRIVER_NAME     XILTEMAC_DRIVER_PREFIX
#endif
extern xilTemac_driver_attach(struct rtems_bsdnet_ifconfig*, int );
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH xilTemac_driver_attach

#define BSP_PPC403_CLOCK_HOOK_EXCEPTION ASM_BOOKE_DEC_VECTOR

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
