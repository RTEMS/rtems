/*
 *  This include file contains all board IO definitions.
 *
 *  generic sh4 BSP
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
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
 *  http://www.OARcorp.com/rtems/license.html.
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

#ifndef __gensh4_h
#define __gensh4_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <clockdrv.h>
#include <console.h>
#include "rtems/score/sh7750_regs.h"

/*
 *  confdefs.h overrides for this BSP:
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (4 * 1024)

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

#define MUST_WAIT_FOR_INTERRUPT 1

#ifndef SH7750_EVT_WDT_ITI
#   error "..."
#endif

#define Install_tm27_vector( handler ) \
{ \
    rtems_isr_entry old_handler; \
    rtems_status_code status; \
    status = rtems_interrupt_catch( (handler), \
            SH7750_EVT_TO_NUM(SH7750_EVT_WDT_ITI), &old_handler); \
    if (status != RTEMS_SUCCESSFUL) \
        printf("Status of rtems_interrupt_catch = %d", status); \
}

#define Cause_tm27_intr() \
{ \
    *(volatile rtems_unsigned16 *)SH7750_IPRB |= 0xf000; \
    *(volatile rtems_unsigned16 *)SH7750_WTCSR = SH7750_WTCSR_KEY; \
    *(volatile rtems_unsigned16 *)SH7750_WTCNT = SH7750_WTCNT_KEY | 0xfe; \
    *(volatile rtems_unsigned16 *)SH7750_WTCSR = \
                            SH7750_WTCSR_KEY | SH7750_WTCSR_TME; \
}

#define Clear_tm27_intr() \
{ \
    *(volatile rtems_unsigned16 *)SH7750_WTCSR = SH7750_WTCSR_KEY; \
}

#define Lower_tm27_intr() \
{ \
    sh_set_interrupt_level((SH7750_IPRB & 0xf000) << SH4_SR_IMASK_S); \
}

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

extern unsigned32       HeapStart ;
extern unsigned32       HeapEnd ;
extern unsigned32       WorkSpaceStart ;
extern unsigned32       WorkSpaceEnd ;

extern void *CPU_Interrupt_stack_low ;
extern void *CPU_Interrupt_stack_high ;

/*
 * This variable is nesessary for console driver.
 */
extern rtems_unsigned32 SH4_CPU_HZ_Frequency;

/*
 * Defined in start.S
 */
extern unsigned32 boot_mode;
#define SH4_BOOT_MODE_FLASH 0
#define SH4_BOOT_MODE_IPL   1
  
/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

extern void bsp_cleanup( void );


/*
 *  Device Driver Table Entries
 */

/*
 * We redefine CONSOLE_DRIVER_TABLE_ENTRY to redirect /dev/console
 */
#undef CONSOLE_DRIVER_TABLE_ENTRY
#define CONSOLE_DRIVER_TABLE_ENTRY \
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
