/*  bsp.h
 *
 *  This include file contains all Motorola 680x0 IDP board IO definitions.
 *
 *  $Id$
 */

#ifndef __IDP_BSP_H
#define __IDP_BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/motorola/mc68230.h>
#include <rtems/motorola/mc68681.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (4 * 1024)

#define DUART_ADDR	0xb00003   /* base address of the MC68681 DUART */

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: tm27 apparently not supported.
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler )

#define Cause_tm27_intr()

#define Clear_tm27_intr()

#define Lower_tm27_intr()

/* Constants */

#define RAM_START 0
#define RAM_END   0x200000

#ifdef MIDP_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
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

extern m68k_isr_entry M68Kvec[];   /* vector table address */

/* functions */

void bsp_cleanup( void );

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

void init_pit( void );

void transmit_char( char ch );

void transmit_char_portb( char ch );

#endif
/* end of include file */
