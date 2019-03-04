/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCPSIM
 *
 * @brief Global BSP definitions.
 */

/*  bsp.h
 *
 *  This include file contains all Papyrus board IO definitions.
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
 */

#ifndef LIBBSP_POWERPC_PSIM_BSP_H
#define LIBBSP_POWERPC_PSIM_BSP_H

/**
 * @defgroup RTEMSBSPsPowerPCPSIM PSIM
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief PSIM Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <libcpu/io.h>
#include <bsp/vectors.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

/*
 *  Information placed in the linkcmds file.
 */

extern int   end;        /* last address in the program */
extern int   RAM_END;

extern uint32_t   BSP_mem_size;

#define BSP_Convert_decrementer( _value ) ( (unsigned long long) _value )

/* macros */
#define Processor_Synchronize() \
  __asm__ (" eieio ")

/*
 *  Network configuration
 */
struct rtems_bsdnet_ifconfig;

int rtems_ifsim_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching);

#define RTEMS_BSP_NETWORK_DRIVER_NAME   "ifsim1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_ifsim_attach

/*
 *  Interfaces to required Clock Driver support methods
 */
int BSP_disconnect_clock_handler(void);
int BSP_connect_clock_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#define BSP_HAS_NO_VME

/** @} */

#endif
