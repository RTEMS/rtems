/*  bsp.h
 *
 *  This include file contains some definitions specific to the
 *  BSVC simulator BSP.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_M68K_SIM68000_BSP_H
#define LIBBSP_M68K_SIM68000_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Define some hardware constants here
 */

/* functions */

rtems_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );

/*
 * Prototype for methods in the BSP that cross file boundaries.
 */
void bsp_spurious_initialize(void);
void bsp_spurious_handler_assistant(rtems_vector_number vector);
rtems_isr bsp_spurious_handler(rtems_vector_number vector);

#ifdef __cplusplus
}
#endif

#endif
