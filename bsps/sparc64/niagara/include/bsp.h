/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARC64Niagara
 *
 * @brief Global BSP definitions.
 */

/*  bsp.h
 *
 *  This include file contains all SPARC64 simulator definitions.
 *
 *  COPYRIGHT (c) 1989-1998. On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_SPARC64_NIAGARA_BSP_H
#define LIBBSP_SPARC64_NIAGARA_BSP_H

/**
 * @defgroup RTEMSBSPsSPARC64Niagara Niagara
 *
 * @ingroup RTEMSBSPsSPARC64
 *
 * @brief Niagara
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
/*
void *clock_driver_sim_idle_body(uintptr_t);
#define BSP_IDLE_TASK_BODY clock_driver_sim_idle_body
*/

/* this should be defined somewhere */
rtems_isr_entry set_vector(                     /* returns old vector */
    rtems_isr_entry     handler,                /* isr routine        */
    rtems_vector_number vector,                 /* vector number      */
    int                 type                    /* RTEMS or RAW intr  */
);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
