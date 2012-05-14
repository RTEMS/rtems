/*
 *  This include file contains all board IO definitions.
 *
 *  generic sh2
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
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Minor adaptations for sh2 by:
 *  John M. Mills (jmills@tga.com)
 *  TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *  Norcross, GA 30071 U.S.A.
 *
 *  This modified file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
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

#define BSP_SMALL_MEMORY 1

#if 0
#include <rtems/devnull.h>
#define BSP_CONSOLE_DEVNAME "/dev/null"
#define BSP_CONSOLE_DRIVER_TABLE_ENTRY DEVNULL_DRIVER_TABLE_ENTRY
#else
#include <sh/sci.h>
/* FIXME:
 *   These definitions will be no longer necessary if the old
 *   implementation of SCI driver will be droped
 */
#define BSP_CONSOLE_DEVNAME "/dev/sci0"
#define BSP_CONSOLE_MINOR_NUMBER ((rtems_device_minor_number) 0)
#define BSP_CONSOLE_DRIVER_TABLE_ENTRY DEVSCI_DRIVER_TABLE_ENTRY
#define BSP_CONSOLE_DEVICE_TERMIOS_HANDLERS (sh_sci_get_termios_handlers(TRUE))
#endif

/* Constants */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define rtems_bsp_delay( microseconds ) CPU_delay(microseconds)
#define sh_delay( microseconds ) CPU_delay( microseconds )

/*
 * Defined in the linker script 'linkcmds'
 */

extern void *CPU_Interrupt_stack_low ;
extern void *CPU_Interrupt_stack_high ;

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Clock driver entry
 */

#ifdef __cplusplus
}
#endif

#endif
