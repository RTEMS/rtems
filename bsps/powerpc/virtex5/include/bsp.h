/**
 * @file
 *
 * @ingroupRTEMSBSPsPowerPCVirtex5
 *
 * @brief Global BSP definitions.
 */

/*
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

#ifndef LIBBSP_POWERPC_VIRTEX5_BSP_H
#define LIBBSP_POWERPC_VIRTEX5_BSP_H

/**
 * @defgroup RTEMSBSPsPowerPCVirtex5 Xilinx Virtex-5
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief Xilinx Virtex-5 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - Interrupt stack space is not minimum if defined.
 */
#define BSP_INTERRUPT_STACK_SIZE  (16 * 1024)

#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Bus Frequency
 */
extern unsigned int BSP_bus_frequency;
/*
 * Processor Clock Frequency
 */
extern unsigned int BSP_processor_frequency;
/*
 * Time base divisior (how many tick for 1 second).
 */
extern unsigned int BSP_time_base_divisor;

/*
 * Macro used by shared MPC6xx timer driver
 */
#define BSP_Convert_decrementer( _value ) \
  ((unsigned long long) ((((unsigned long long)BSP_time_base_divisor) * 1000000ULL) /((unsigned long long) BSP_bus_frequency)) * ((unsigned long long) (_value)))

/*
 *  Interfaces to required Clock Driver support methods
 */
int BSP_disconnect_clock_handler(void);
int BSP_connect_clock_handler(void);

/*
 * Prototypes for BSP methods shared across file boundaries
 */
void zero_bss(void);

#endif /* ASM */

void BSP_ask_for_reset(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
