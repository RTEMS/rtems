/*  bsp.h
 *
 *  This include file contains some definitions specific to the
 *  i960 simulator in gdb.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __I960SIM_h
#define __I960SIM_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

/* Constants */

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */

/*
 * NOTE: Use the standard Clock driver entry
 */

/* functions */

void bsp_cleanup( void );

i960_isr_entry set_vector( rtems_isr_entry, unsigned int, unsigned int );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
