/*
 *  This include file contains all board IO definitions.
 *
 *  SH-gdb simulator BSP
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 2001, Ralf Corsepius, Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *  COPYRIGHT (c) 2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */

#ifndef __bsp_h
#define __bsp_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <clockdrv.h>
#include <console.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 0
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (4 * 1024)

#include <bspopts.h>

/*
 * FIXME: One of these would be enough.
 */
#include <gdbsci.h>
#include <rtems/devnull.h>

/*
 *  Define the time limits for RTEMS Test Suite test durations.
 *  Long test and short test duration limits are provided.  These
 *  values are in seconds and need to be converted to ticks for the
 *  application.
 *
 * FIXME: This should not be here.
 */

#define MAX_LONG_TEST_DURATION       300 /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Stuff for Time Test 27
 *
 * FIXME: This should not be here.
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler )
#define Cause_tm27_intr()
#define Clear_tm27_intr()

/* Constants */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 * 
 * FIXME: Not applicable with gdb's simulator
 * Kept for sourcecode compatibility with other sh-BSPs 
 */
#define delay( microseconds ) CPU_delay(microseconds)
#define sh_delay( microseconds ) CPU_delay(microseconds)

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
 * Redefine CONSOLE_DRIVER_TABLE_ENTRY to redirect /dev/console
 */
#undef CONSOLE_DRIVER_TABLE_ENTRY
#define CONSOLE_DRIVER_TABLE_ENTRY \
  BSP_CONSOLE_DRIVER_TABLE_ENTRY, \
  { console_initialize, console_open, console_close, \
      console_read, console_write, console_control }
 
/*
 * NOTE: Use the standard Clock driver entry
 */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
