/**
 *  @file
 *
 *  @ingroup h8300_bsp
 *
 *  @brief h8 simulator definitions in gdb
 *
 *  This include file contains some definitions specific to the
 *  h8 simulator in gdb.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_H8300_H8SIM_BSP_H
#define LIBBSP_H8300_H8SIM_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup h8300_bsp Clock Tick Support
 *
 *  @ingroup h8300_h8sim
 *
 *  @brief Clock Tick Support Package
 */
Thread clock_driver_sim_idle_body(uintptr_t);
#define BSP_IDLE_TASK_BODY clock_driver_sim_idle_body

#ifdef __cplusplus
}
#endif

#endif
