/**
 * @file
 *
 * @ingroup RTEMSBSPsV850GDBSim
 *
 * @brief Global BSP definitions.
 */
/*
 *  This include file contains some definitions specific to the
 *  GDB simulator in gdb.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_V850_GDBV850SIM_BSP_H
#define LIBBSP_V850_GDBV850SIM_BSP_H

/**
 * @defgroup RTEMSBSPsV850GDBSim GDB Simulator
 *
 * @ingroup RTEMSBSPsV850
 *
 * @brief GDB Simulator Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/* support for simulated clock tick */
void *clock_driver_sim_idle_body(uintptr_t);
#define BSP_IDLE_TASK_BODY clock_driver_sim_idle_body

#ifdef __cplusplus
}
#endif

/** @} */

#endif
