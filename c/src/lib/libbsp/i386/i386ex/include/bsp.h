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
#include <rtems/bspIo.h>
#include <libcpu/cpu.h>
#include <bsp/irq.h>

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define rtems_bsp_delay( _microseconds ) \
  { \
    uint32_t         _counter; \
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
