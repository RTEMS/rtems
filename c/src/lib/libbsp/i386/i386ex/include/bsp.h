/*
 *  This include file definitions related to an Intel i386ex board.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __FORCE386_h
#define __FORCE386_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>

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

#define Install_tm27_vector( handler ) set_vector( (handler), 0x90, 1 )

#define Cause_tm27_intr()              asm volatile( "int $0x90" : : );

#define Clear_tm27_intr()

#define Lower_tm27_intr()

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( _microseconds ) \
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
 
/*
 * How many libio files we want
 */
 
#define BSP_LIBIO_MAX_FDS       20

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

extern i386_IDT_slot Interrupt_descriptor_table[ 256 ];
extern i386_GDT_slot Global_descriptor_table[ 8192 ];

BSP_EXTERN unsigned short Idt[3];  /* Interrupt Descriptor Table Address */
BSP_EXTERN unsigned short Gdt[3];  /* Global Descriptor Table Address */
BSP_EXTERN unsigned int   Idt_base;
BSP_EXTERN unsigned int   Gdt_base;

/* routines */

i386_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
