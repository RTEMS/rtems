/*
 *  This include file definitions related to an Intel i386ex board.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#ifndef __TS386_h
#define __TS386_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>
#include <rtems/bspIo.h>
#include <libcpu/cpu.h>
#include <irq.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (8 * 1024)
  
/*
 * Network driver configuration
 */

struct rtems_bsdnet_ifconfig;
extern int rtems_ne_driver_attach (struct rtems_bsdnet_ifconfig *config);

extern void Wait_X_ms (unsigned);

#define RTEMS_BSP_NETWORK_DRIVER_NAME         "ne1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH       rtems_ne_driver_attach

#define NE2000_BYTE_TRANSFERS

/*
 * Constants relating to the 8254 (or 8253) programmable interval timers.
 */

#define TIMER_CONFIG   0xF834
#define IO_TIMER1      0xF040

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
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Use a software interrupt for the i386.
 */

#define MUST_WAIT_FOR_INTERRUTPT 0


#define Install_tm27_vector(handler) \
{ \
  rtems_isr_entry dummy; \
  rtems_interrupt_catch(handler, 0x90, &dummy); \
}
#define Cause_tm27_intr()              asm volatile( "int $0x90" : : );

#define Clear_tm27_intr()

#define Lower_tm27_intr()

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define rtems_bsp_delay( _microseconds ) \
  { \
    rtems_unsigned32 _counter; \
    \
    _counter = (_microseconds); \
    \
    asm volatile ( "0: nop;" \
                   " mov %0,%0 ;" \
                   " loop 0b" : "=c" (_counter) \
                              : "0"  (_counter) \
                 ); \
    \
  }

/* Constants */

#define RAM_START 0

/* replaced the earlier EI kludge of 0xfffff */

#define RAM_END   0x200000

/* I/O addressing */

/*
 *#define Is_tx_ready( _status ) ( (_status) & 0x20 )
 */
/* dec 20. try the TE instead of TBE as the check */

#define Is_tx_ready( _status ) ( (_status) & 0x40 )


#define Is_rx_ready( _status ) ( (_status) & 0x01 )

/* Structures */

#ifdef F386_INIT
#undef BSP_EXTERN
#define BSP_EXTERN
#else
#undef BSP_EXTERN
#define BSP_EXTERN extern
#endif

/*
 *  Device Driver Table Entries
 */
 
/*
 * NOTE: Use the standard Console driver entry
 */
 
/*
 * NOTE: Use the standard Clock driver entry
 */
 
/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

#define IDT_SIZE 256
#define GDT_SIZE 3

extern interrupt_gate_descriptor Interrupt_descriptor_table[IDT_SIZE];
extern segment_descriptors Global_descriptor_table   [GDT_SIZE];

BSP_EXTERN unsigned short Idt[3];  /* Interrupt Descriptor Table Address */
BSP_EXTERN unsigned short Gdt[3];  /* Global Descriptor Table Address */
BSP_EXTERN unsigned int   Idt_base;
BSP_EXTERN unsigned int   Gdt_base;

/* routines */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
