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
| Modified by Eric Valette the 20/05/98 in order to add definitions used
| to enhance video putchar capabilities.
|
| Copyright (C) 1998  valette@crf.canon.fr
|
| Canon Centre Recherche France.
|
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
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
+--------------------------------------------------------------------------*/

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <libcpu/cpu.h>
#include <rtems/bspIo.h>

#define BSP_HAS_FRAME_BUFFER 1

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;

/* app. may provide a routine (called _very_ early) to tell us
 * which ports to use for printk / console. BSP provides a default
 * implementation (weak alias) which does nothing (use BSP default
 * ports).
 */
extern void
BSP_runtime_console_select(int *pPrintkPort, int *pConsolePort);

extern int rtems_ne_driver_attach(struct rtems_bsdnet_ifconfig *, int);
#define BSP_NE2000_NETWORK_DRIVER_NAME      "ne1"
#define BSP_NE2000_NETWORK_DRIVER_ATTACH    rtems_ne_driver_attach

extern int rtems_wd_driver_attach(struct rtems_bsdnet_ifconfig *, int);
#define BSP_WD8003_NETWORK_DRIVER_NAME      "wd1"
#define BSP_WD8003_NETWORK_DRIVER_ATTACH    rtems_wd_driver_attach

extern int rtems_dec21140_driver_attach(struct rtems_bsdnet_ifconfig *, int);
#define BSP_DEC21140_NETWORK_DRIVER_NAME    "dc1"
#define BSP_DEC21140_NETWORK_DRIVER_ATTACH  rtems_dec21140_driver_attach

#ifndef RTEMS_BSP_NETWORK_DRIVER_NAME
#define RTEMS_BSP_NETWORK_DRIVER_NAME   BSP_DEC21140_NETWORK_DRIVER_NAME
#endif

#ifndef RTEMS_BSP_NETWORK_DRIVER_ATTACH
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH BSP_DEC21140_NETWORK_DRIVER_ATTACH
#endif

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------+
| Video (console) related constants.
+--------------------------------------------------------------------------*/

#include <crt.h>

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
#define TIMER_RD_BACK  0xc0	       /* Read Back Command 		 */
		/* READ BACK command layout in the Command Register 	 */
#define RB_NOT_COUNT	0x40	       /* Don't select counter latch 	 */
#define RB_NOT_STATUS	0x20	       /* Don't select status latch 	 */
#define	RB_COUNT_0	0x02	       /* Counter 0 latch 		 */
#define RB_COUNT_1	0x04	       /* Counter 1 latch 		 */
#define RB_COUNT_2	0x08	       /* Counter 2 latch 		 */
#define RB_OUTPUT	0x80	       /* Output of the counter is 1 	 */

#define	TIMER_TICK     1193182  /* The internal tick rate in ticks per second */

/*-------------------------------------------------------------------------+
| Console Defines
|      WARNING: These Values MUST match the order in 
|               Console_Configuration_Ports
+--------------------------------------------------------------------------*/
#define    BSP_CONSOLE_VGA            0
#define    BSP_CONSOLE_COM1           1
#define    BSP_CONSOLE_COM2           2

/*-------------------------------------------------------------------------+
| Macros
+--------------------------------------------------------------------------*/
/* does anyone need this? if so, report it so we can rename this macro */
#if 0
/*-------------------------------------------------------------------------+
| Simple spin delay in microsecond units for device drivers.
| This is very dependent on the clock speed of the target.
+--------------------------------------------------------------------------*/
#define rtems_bsp_delay(_microseconds) \
{ \
  uint32_t         _cnt = _microseconds; \
  __asm__ volatile ("0: nop; mov %0,%0; loop 0b" : "=c"(_cnt) : "0"(_cnt)); \
}
#endif

/*-------------------------------------------------------------------------+
| Convert microseconds to ticks and ticks to microseconds.
+--------------------------------------------------------------------------*/
#define US_TO_TICK(us) (((us)*105+44)/88)
#define TICK_TO_US(tk) (((tk)*88+52)/105)

/*-------------------------------------------------------------------------+
| External Variables.
+--------------------------------------------------------------------------*/
#define IDT_SIZE 256
#define GDT_SIZE 3

extern interrupt_gate_descriptor Interrupt_descriptor_table[IDT_SIZE];
extern segment_descriptors Global_descriptor_table   [GDT_SIZE];

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
void          _IBMPC_initVideo(void);    /* from 'outch.c'  */
void          _IBMPC_outch    (char);    /* from 'outch.c'  */
char          _IBMPC_inch     (void);    /* from 'inch.c'   */
char          _IBMPC_inch_sleep (void);  /* from 'inch.c'   */

void Wait_X_ms(unsigned int timeToWait); /* from 'timer.c'  */

/* Definitions for BSPConsolePort */
#define BSP_CONSOLE_PORT_CONSOLE (-1)
#define BSP_CONSOLE_PORT_COM1    (BSP_UART_COM1)
#define BSP_CONSOLE_PORT_COM2    (BSP_UART_COM2)

/*
 * Command line.
 */
const char* bsp_cmdline(void);
const char* bsp_cmdline_arg(const char* arg);

/*
 * IDE command line parsing.
 */
void bsp_ide_cmdline_init(void);

/*
 * indicate, that BSP has IDE driver
 */
#define RTEMS_BSP_HAS_IDE_DRIVER

/* GDB stub stuff */
void i386_stub_glue_init(int uart);
void i386_stub_glue_init_breakin(void);
void set_debug_traps(void);
void breakpoint(void);

#define BSP_MAXIMUM_DEVICES 6

#ifdef __cplusplus
}
#endif

#endif /* _BSP_H */
