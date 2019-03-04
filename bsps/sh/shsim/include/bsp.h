/**
 * @file
 *
 * @ingroup RTEMSBSPsSHSim
 *
 * @brief Global BSP definitions.
 */

/*
 *  SH-gdb simulator BSP
 *
 *  This include file contains all board IO definitions.
 */

/*
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 2001, Ralf Corsepius, Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  COPYRIGHT (c) 2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_SH_SHSIM_BSP_H
#define LIBBSP_SH_SHSIM_BSP_H

/**
 * @defgroup RTEMSBSPsSHSim Simulator
 *
 * @ingroup RTEMSBSPsSH
 *
 * @brief Simulator Board Support Package.
 *
 * @{
 */

#ifndef ASM

#include <rtems.h>

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

/*
 * FIXME: One of these would be enough.
 */
#include <rtems/devnull.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

void *clock_driver_sim_idle_body(uintptr_t);
#define BSP_IDLE_TASK_BODY clock_driver_sim_idle_body

/*
 * BSP methods that cross file boundaries.
 */
int _sys_exit (int n);
void bsp_hw_init(void);

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

/** @} */

#endif
