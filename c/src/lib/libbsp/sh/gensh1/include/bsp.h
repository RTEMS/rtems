/*
 *  This include file contains all board IO definitions.
 *
 *  generic sh1
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __gensh1_h
#define __gensh1_h

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_CONSOLE_DEVNAME "/dev/null"


#include <rtems.h>
#include <clockdrv.h>
#include <sh/null.h>
#include <console.h>

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
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), 0, 1 )

#define Cause_tm27_intr()

#define Clear_tm27_intr()

#define Lower_tm27_intr()

/* Constants */

#define MHZ 20

/*
 * Defined in the linker script 'linkcmds'
 */

extern unsigned32       HeapStart ;
extern unsigned32       HeapEnd ;
extern unsigned32       WorkSpaceStart ;
extern unsigned32       WorkSpaceEnd ;

extern void *CPU_Interrupt_stack_low ;
extern void *CPU_Interrupt_stack_high ;

  
/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

extern void bsp_cleanup( void );

/*
 *  Device Driver Table Entries
 */

/*
 * We redefine CONSOLE_DRIVER_TABLE_ENTRY to redirect /dev/console
 */
#if defined(CONSOLE_DRIVER_TABLE_ENTRY)
#warning Overwriting CONSOLE_DRIVER_TABLE_ENTRY
#undef CONSOLE_DRIVER_TABLE_ENTRY
#endif

#define CONSOLE_DRIVER_TABLE_ENTRY \
  DEVNULL_DRIVER_TABLE_ENTRY, \
  { console_initialize, console_open, console_close, \
      console_read, console_write, console_control }
 
/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 * How many libio files we want
 */

#define BSP_LIBIO_MAX_FDS       20

/*
 * FIXME: Should this go to libcpu/sh/sh7032 ?
 */
/* functions */
sh_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
