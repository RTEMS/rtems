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

#define Install_tm27_vector( handler ) \
{ \
  rtems_isr_entry ignored ; \
  rtems_interrupt_catch( (handler), 0, &ignored ) ; \
}

#define Cause_tm27_intr()

#define Clear_tm27_intr()

#define Lower_tm27_intr()

/* Constants */
#ifndef MHZ
#error Missing MHZ
#endif

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 *
 *  Since we don't have a real time clock, this is a very rough
 *  approximation, assuming that each cycle of the delay loop takes
 *  approx. 4 machine cycles.
 *
 *  e.g.: MHZ = 20 =>     5e-8 secs per instruction
 *                 => 4 * 5e-8 secs per delay loop
 */

#define delay( microseconds ) \
{ register unsigned int _delay = (microseconds) * (MHZ / 4 ); \
  asm volatile ( \
"0:	add  #-1,%0\n \
	nop\n \
	cmp/pl %0\n \
	bt 0b\
	nop" \
    :: "r" (_delay) );  \
}

/*
 * For backward compatibility only.
 * Do not rely on them being present in future
 */
#define CPU_delay( microseconds ) delay( microseconds )
#define sh_delay( microseconds ) delay( microseconds )

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
 * FIXME: Should this go to libcpu/sh/sh7032 ?
 */
#if 0
/* functions */
sh_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
