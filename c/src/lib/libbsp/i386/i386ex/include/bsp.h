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

#ifndef __I386EX_BSP_h
#define __I386EX_BSP_h

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
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (8 * 1024)

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

#define RAM_END   0x100000

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
#define GDT_SIZE 8192

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
