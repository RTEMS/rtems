/*-------------------------------------------------------------------------+
| bsp.h v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This include file contains definitions related to the PC386 BSP.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   bsp.h,v 1.5 1995/12/19 20:07:30 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/


#ifndef __BSP_H_
#define __BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define BSP_LIBIO_MAX_FDS 20             /* Number of libio files we want.    */

/*-------------------------------------------------------------------------+
| Memory related constants.
+--------------------------------------------------------------------------*/
#ifdef RTEMS_SMALL_MEMORY                /* We only have low (640K) memory.   */

#define RAM_START 0x00000
#define RAM_END	  0xA0000

#else                                    /* We have at least 2048K of memory. */

#define RAM_START 0x100000
#define RAM_END   0x200000

#endif /* RTEMS_SMALL_MEMORY */

#define HEAP_SIZE 64  /* Size of libc Heap (used for malloc et al) in KBytes. */

/*-------------------------------------------------------------------------+
| Video (console) related constants.
+--------------------------------------------------------------------------*/
#define COLOUR  1  /* Assume colour console */

#if COLOUR

# define        GDC_REG_PORT    0x3D4
# define        GDC_VAL_PORT    0x3D5
# define        TVRAM           ((rtems_unsigned16 *)0xB8000)

#else

# define        GDC_REG_PORT    0x3B4
# define        GDC_VAL_PORT    0x3B5
# define        TVRAM           ((rtems_unsigned16 *)0xB0000)

#endif /* COLOUR */

/* Number of Video Lines & Columns */

#define MAX_COL 80

#ifdef RTEMS_VIDEO_80x50
#define MAX_ROW 50
#else
#define MAX_ROW 25
#endif /* RTEMS_VIDEO_80x50 */

/*-------------------------------------------------------------------------+
| Constants relating to the 8254 (or 8253) programmable interval timers.
+--------------------------------------------------------------------------*/
#define IO_TIMER1      0x40
                  /* Port address of the control port and timer channels */
#define TIMER_CNTR0    (IO_TIMER1 + 0) /* timer 0 counter port           */
#define TIMER_CNTR1    (IO_TIMER1 + 1) /* timer 1 counter port           */
#define TIMER_CNTR2    (IO_TIMER1 + 2) /* timer 2 counter port           */
#define TIMER_MODE     (IO_TIMER1 + 3) /* timer mode port                */
#define TIMER_SEL0     0x00            /* select counter 0               */
#define TIMER_SEL1     0x40            /* select counter 1               */
#define TIMER_SEL2     0x80            /* select counter 2               */
#define TIMER_INTTC    0x00            /* mode 0, intr on terminal cnt   */
#define TIMER_ONESHOT  0x02            /* mode 1, one shot               */
#define TIMER_RATEGEN  0x04            /* mode 2, rate generator         */
#define TIMER_SQWAVE   0x06            /* mode 3, square wave            */
#define TIMER_SWSTROBE 0x08            /* mode 4, s/w triggered strobe   */
#define TIMER_HWSTROBE 0x0a            /* mode 5, h/w triggered strobe   */
#define TIMER_LATCH    0x00            /* latch counter for reading      */
#define TIMER_LSB      0x10            /* r/w counter LSB                */
#define TIMER_MSB      0x20            /* r/w counter MSB                */
#define TIMER_16BIT    0x30            /* r/w counter 16 bits, LSB first */
#define TIMER_BCD      0x01            /* count in BCD                   */

#define	TIMER_TICK     1193182  /* The internal tick rate in ticks per second */

/*-------------------------------------------------------------------------+
| Define the time limits for RTEMS Test Suite test durations. Long test and
| short test duration limits are provided. These values are in seconds and
| need to be converted to ticks for the application.
+--------------------------------------------------------------------------*/
#define MAX_LONG_TEST_DURATION       300  /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3    /* 3 seconds */

/*-------------------------------------------------------------------------+
| Macros
+--------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------+
| Define the interrupt mechanism for Time Test 27.
| NOTE: Use a software interrupt for the i386 family.
+--------------------------------------------------------------------------*/
#define MUST_WAIT_FOR_INTERRUPT 0
#define Install_tm27_vector(handler) \
{ \
  rtems_isr_entry dummy; \
  rtems_interrupt_catch(handler, 0x90, &dummy); \
}
#define Cause_tm27_intr() asm volatile("int $0x90" : :);
#define Clear_tm27_intr()
#define Lower_tm27_intr()

/*-------------------------------------------------------------------------+
| Simple spin delay in microsecond units for device drivers.
| This is very dependent on the clock speed of the target.
+--------------------------------------------------------------------------*/
#define delay(_microseconds) \
{ \
  rtems_unsigned32 _cnt = _microseconds; \
  asm volatile ("0: nop; mov %0,%0; loop 0b" : "=c"(_cnt) : "0"(_cnt)); \
}

/*-------------------------------------------------------------------------+
| Convert microseconds to ticks and ticks to microseconds.
+--------------------------------------------------------------------------*/
#define US_TO_TICK(us) (((us)*105+44)/88)
#define TICK_TO_US(tk) (((tk)*88+52)/105)

/*-------------------------------------------------------------------------+
| External Variables.
+--------------------------------------------------------------------------*/
extern i386_IDT_slot Interrupt_descriptor_table[256];
extern i386_GDT_slot Global_descriptor_table   [8192];
 
extern rtems_configuration_table BSP_Configuration;
                                    /* User provided BSP configuration table. */
extern rtems_unsigned32          rtemsFreeMemStart;
  /* Address of start of free memory - should be used when creating new
     partitions or regions and updated afterwards. */

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
void          _IBMPC_initVideo(void);    /* from 'outch.c'  */
void          _IBMPC_outch    (char);    /* from 'outch.c'  */
rtems_boolean _IBMPC_chrdy    (char *);  /* from 'inch.c'   */
char          _IBMPC_inch     (void);    /* from 'inch.c'   */ 

void printk(char *fmt, ...);             /* from 'printk.c' */

void rtemsReboot(void);                  /* from 'exit.c'   */

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H_ */
/* end of include file */
