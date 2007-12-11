/*  bsp.h
 *
 *  This include file contains some definitions specific to the RBTX4938.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  bsp.h,v 1.7.6.1 2003/09/04 18:44:49 joel Exp
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <libcpu/tx4938.h>

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Following are for XXX and are board independent
 *
 */
#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( handler ) \
  do { \
  } while(0)

#define Cause_tm27_intr() \
  do { \
  } while(0)

#define Clear_tm27_intr() \
  do { \
  } while(0)

#define Lower_tm27_intr() \
  do { \
  } while(0)


/* Constants */

/* miscellaneous stuff assumed to exist */

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

rtems_isr_entry set_vector(
  rtems_isr_entry, rtems_vector_number, int );

#ifdef __cplusplus
}
#endif

#endif
