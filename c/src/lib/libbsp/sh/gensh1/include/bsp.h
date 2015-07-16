/*
 *  generic sh1
 *
 *  This include file contains all board IO definitions.
 */

/*
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
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_SH_GENSH1_BSP_H
#define LIBBSP_SH_GENSH1_BSP_H

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <termios.h> /* for tcflag_t */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EDIT: To activate the sci driver, change the define below */
#if 1
#include <rtems/devnull.h>
#define BSP_CONSOLE_DEVNAME "/dev/null"
#define BSP_CONSOLE_DRIVER_TABLE_ENTRY DEVNULL_DRIVER_TABLE_ENTRY
#else
#include <sh/sci.h>
#define BSP_CONSOLE_DEVNAME "/dev/sci0"
#define BSP_CONSOLE_DRIVER_TABLE_ENTRY DEVSCI_DRIVER_TABLE_ENTRY
#endif

/* Constants */

/*
 * Defined in the linker script 'linkcmds'
 */

extern void *CPU_Interrupt_stack_low;
extern void *CPU_Interrupt_stack_high;

/*
 *  Device Driver Table Entries
 */

/*
 * We redefine CONSOLE_DRIVER_TABLE_ENTRY to redirect /dev/console
 */
#undef CONSOLE_DRIVER_TABLE_ENTRY
#define CONSOLE_DRIVER_TABLE_ENTRY \
  BSP_CONSOLE_DRIVER_TABLE_ENTRY, \
  { console_initialize, console_open, console_close, \
      console_read, console_write, console_control }

/*
 * BSP methods that cross file boundaries.
 */
void bsp_hw_init(void);

extern int _sci_get_brparms(
  tcflag_t      cflag,
  unsigned char *smr,
  unsigned char *brr
);

#ifdef __cplusplus
}
#endif

#endif
