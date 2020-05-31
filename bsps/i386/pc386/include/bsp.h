/**
 * @file
 *
 * @ingroup RTEMSBSPsI386Pc386
 *
 * @brief Global BSP definitions.
 */

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
| *  http://www.rtems.org/license/LICENSE.
| **************************************************************************
+--------------------------------------------------------------------------*/

#ifndef LIBBSP_I386_PC386_BSP_H
#define LIBBSP_I386_PC386_BSP_H

/**
 * @addtogroup RTEMSBSPsI386
 *
 * @{
 */
#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <bsp/tblsizes.h>

#include <rtems.h>
#include <rtems/score/cpu.h>
#include <rtems/bspIo.h>

#ifdef __cplusplus
extern "C" {
#endif



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

extern int rtems_3c509_driver_attach(struct rtems_bsdnet_ifconfig *config);
#define BSP_3C509_NETWORK_DRIVER_NAME    "3c1"
#define BSP_3C509_NETWORK_DRIVER_ATTACH  rtems_3c509_driver_attach

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

#if defined( RTEMS_SMP )
  extern ISR_lock_Control rtems_i386_i8254_access_lock;
#endif


/*-------------------------------------------------------------------------+
| Console Defines
|      WARNING: These Values MUST match the order in
|               Console_Configuration_Ports
+--------------------------------------------------------------------------*/
#define    BSP_CONSOLE_VGA            0
#define    BSP_CONSOLE_COM1           1
#define    BSP_CONSOLE_COM2           2

/*-------------------------------------------------------------------------+
| Convert microseconds to ticks and ticks to microseconds.
+--------------------------------------------------------------------------*/
#define US_TO_TICK(us) (((us)*105+44)/88)
#define TICK_TO_US(tk) (((tk)*88+52)/105)

/*-------------------------------------------------------------------------+
| External Variables.
+--------------------------------------------------------------------------*/
extern interrupt_gate_descriptor Interrupt_descriptor_table[IDT_SIZE];
extern segment_descriptors _Global_descriptor_table   [GDT_SIZE];

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
void          _IBMPC_initVideo(void);    /* from 'outch.c'  */
void          _IBMPC_outch    (char);    /* from 'outch.c'  */
char          _IBMPC_inch     (void);    /* from 'inch.c'   */
char          _IBMPC_inch_sleep (void);  /* from 'inch.c'   */
int           BSP_wait_polled_input(void); /* from 'inch.c' */
int           rtems_kbpoll( void );      /* from 'inch.c' */
int           getch( void );             /* from 'inch.c' */
void           add_to_queue( unsigned short b ); /* from 'inch.c' */

void Wait_X_ms(unsigned int timeToWait); /* from 'timer.c'  */
void Calibrate_loop_1ms(void);           /* from 'timer.c'  */

void rtems_irq_mngt_init(void);          /* from 'irq_init.c' */

void Clock_driver_install_handler(void);             /* from 'ckinit.c'  */
void Clock_driver_support_initialize_hardware(void); /* from 'ckinit.c'  */

void *bsp_idle_thread( uintptr_t ignored );
#define BSP_IDLE_TASK_BODY bsp_idle_thread

void kbd_reset_setup(char *str, int *ints);   /* from 'pc_keyb.c' */
size_t read_aux(char * buffer, size_t count); /* from 'ps2_mouse.c'  */

bool bsp_get_serial_mouse_device(             /* from 'serial_mouse.c' */
  const char **name,
  const char **type
);

void register_leds(                           /* from 'keyboard.c' */
  int console,
  unsigned int led,
  unsigned int *addr,
  unsigned int mask
);

/* Definitions for BSPConsolePort */
#define BSP_CONSOLE_PORT_CONSOLE (-1)
#define BSP_CONSOLE_PORT_COM1    (BSP_UART_COM1)
#define BSP_CONSOLE_PORT_COM2    (BSP_UART_COM2)

/*
 * Command line.
 */
const char* bsp_cmdline(void);
const char* bsp_cmdline_arg(const char* arg);

#if BSP_ENABLE_IDE
/*
 * IDE command line parsing.
 */
void bsp_ide_cmdline_init(void);

/*
 * indicate, that BSP has IDE driver
 */
#define RTEMS_BSP_HAS_IDE_DRIVER
#endif

/* GDB stub stuff */
void init_remote_gdb( void );
void i386_stub_glue_init(int uart);
void i386_stub_glue_init_breakin(void);
int i386_stub_glue_uart(void);
void breakpoint(void);

/*
 * Debug helper methods
 */
typedef __FILE FILE;
uint32_t BSP_irq_count_dump(FILE *f);

/*
 * Prototypes just called from .S files. This lets the .S file include
 * bsp.h just to establish the dependency.
 */
void raw_idt_notify(void);
void C_dispatch_isr(int vector);

#ifdef RTEMS_SMP
  /* CPU specific functions used by the SMP API */
  int imps_probe(void);
  void ipi_install_irq(void);
  int send_ipi(unsigned int dst, unsigned int v);
#endif

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

/** @} */

#endif /* _BSP_H */
