/*  bsp.h
 *
 *  This include file definitions related to the Force CPU-386 board.
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
#define RAM_END   0x100000

/* I/O addressing */

/*
 *  The following determines whether Port B or the Console should
 *  be used for test I/O.  Setting ONE (and only ONE) of these to 1
 *  enables I/O on that port.
 *
 *  PORT A - DUSCC MC68562 Channel A
 *  PORT B - DUSCC MC68562 Channel B
 *  PORT C - MFP MC68901 Channel      (*** FORCEbug console ***)
 */

#define PORTB         1               /* use port b as test port */
#define PORTC         0               /* use console port as test port */

#if ( PORTB == 1 )
#define TX_STATUS     0x1b6           /* DUSCC General Status Register */
#define RX_STATUS     0x1b6           /* DUSCC General Status Register */
#define TX_BUFFER     0x1e0           /* DUSCC Transmitter Channel B */
#define RX_BUFFER     0x1e8           /* DUSCC Receiver Channel B */
#define Is_tx_ready( _status ) ( (_status) & 0x20 )
#define Is_rx_ready( _status ) ( (_status) & 0x10 )
#endif

#if ( PORTC == 1 )
#define TX_STATUS     0x12c           /* MFP Transmit Status Register */
#define RX_STATUS     0x12a           /* MFP Receive Status Register */
#define TX_BUFFER     0x12e           /* MFP Transmitter Channel  */
#define RX_BUFFER     0x12e           /* MFP Receiver Channel  */
#define Is_tx_ready( _status ) ( (_status) & 0x80 )
#define Is_rx_ready( _status ) ( (_status) & 0x80 )
#endif

/* Timer constants */

#define IERA   0x106     /* Interrupt Enable Register A */
#define IMRA   0x112     /* Interrupt Mask Register A */
#define TACR   0x118     /* Timer A Control Register */
#define TADR   0x11e     /* Timer A Data Register */

#define IERB   0x108     /* Interrupt Enable Register B */
#define TBCR   0x11a     /* Timer B Control Register */
#define TBDR   0x120     /* Timer B Data Register */

/* Structures */

#ifdef F386_INIT
#undef BSP_EXTERN
#define BSP_EXTERN
#else
#undef BSP_EXTERN
#define BSP_EXTERN extern
#endif

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
