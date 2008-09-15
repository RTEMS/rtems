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
 *  http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>

#include <bspopts.h>

/*
 * FIXME: One of these would be enough.
 */
#include <gdbsci.h>
#include <rtems/devnull.h>

/* Constants */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 *
 * FIXME: Not applicable with gdb's simulator
 * Kept for sourcecode compatibility with other sh-BSPs
 */
#define rtems_bsp_delay( microseconds ) CPU_delay(microseconds)
#define sh_delay( microseconds ) CPU_delay(microseconds)

/*
 * Defined in the linker script 'linkcmds'
 */

extern void *CPU_Interrupt_stack_low ;
extern void *CPU_Interrupt_stack_high ;

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
