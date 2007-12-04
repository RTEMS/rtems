/*  bsp.h
 *
 *  This include file contains all C4X Simulator IO definitions.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#ifdef ASM

#else
#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>

/* Constants */

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */

/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 *  Information placed in the linkcmds file.
 */

/* functions */

void bsp_start( void );

void bsp_cleanup( void );

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

void BSP_fatal_return( void );

void bsp_spurious_initialize( void );

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */

extern uint32_t                  bsp_isr_level;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
