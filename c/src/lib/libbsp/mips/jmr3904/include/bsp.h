/*  bsp.h
 *
 *  This include file contains some definitions specific to the
 *  JMR3904 simulator in gdb.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __JMR3904_h
#define __JMR3904_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>
#include <libcpu/tx3904.h>

/*
 *  Define the time limits for RTEMS Test Suite test durations.
 *  Long test and short test duration limits are provided.  These
 *  values are in seconds and need to be converted to ticks for the
 *  application.
 *
 */

#define MAX_LONG_TEST_DURATION       300 /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Following are for XXX and are board independent
 *
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) 

#define Cause_tm27_intr()  

#define Clear_tm27_intr()  

#define Lower_tm27_intr()

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

/* i960_isr_entry set_vector( rtems_isr_entry, unsigned int, unsigned int ); */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
