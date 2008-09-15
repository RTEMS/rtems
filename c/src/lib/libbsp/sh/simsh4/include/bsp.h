/*
 *  This include file contains all board IO definitions.
 *
 *  sh4 simulator bsp
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *          Alexandra Kossovsky <sasha@oktet.ru>
 *
 *  Based on work:
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998-2001.
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
 *
 *  $Id$
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <ramdisk.h>

#include <bspopts.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 1

/* !!! Should be defined in accordance to simulator considerations.*/
#define CLOCK_VECTOR 32

/* Constants */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( microseconds ) CPU_delay(microseconds)
#define sh_delay( microseconds ) CPU_delay( microseconds )

/*
 * Defined in the linker script 'linkcmds'
 */
extern void *CPU_Interrupt_stack_low ;
extern void *CPU_Interrupt_stack_high ;

/*
 *  Device Driver Table Entries
 */

#undef CONSOLE_DRIVER_TABLE_ENTRY
#define CONSOLE_DRIVER_TABLE_ENTRY \
  { console_initialize, console_open, console_close, \
      console_read, console_write, console_control }

#define RAMDISK_DRIVER_TABLE_ENTRY \
  { ramdisk_initialize, ramdisk_open, ramdisk_close, \
        ramdisk_read, ramdisk_write, ramdisk_control }

/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 * Trap interface with simulator
 */
int __trap34();

#ifdef __cplusplus
}
#endif

#endif
