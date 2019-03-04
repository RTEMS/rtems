/**
 * @file
 *
 * @ingroup RTEMSBSPsMoxieMoxieSim
 *
 * @brief Global BSP definitions.
 */

/*
 *  This include file contains some definitions specific to the
 *  moxie simulator in gdb.
 */

/*
 *  COPYRIGHT (c) 1989-1999, 2010, 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_MOXIE_MOXIESIM_BSP_H
#define LIBBSP_MOXIE_MOXIESIM_BSP_H

/**
 * @defgroup RTEMSBSPsMoxieMoxieSim Simulator
 *
 * @ingroup RTEMSBSPsMoxie
 *
 * @brief Simulator Board Support Package.
 *
 * @{
 */

#include <bspopts.h>

#include <rtems.h>
#include <bsp/default-initial-extension.h>

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
