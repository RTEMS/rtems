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

#ifndef __BSVC_SIM
#define __BSVC_SIM

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

/*
 *  Define some hardware constants here
 */

/* functions */

rtems_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );

#ifdef __cplusplus
}
#endif

#endif
