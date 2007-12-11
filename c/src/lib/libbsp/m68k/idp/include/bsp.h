/*  bsp.h
 *
 *  This include file contains all Motorola 680x0 IDP board IO definitions.
 *
 *  $Id$
 */

#ifndef _BSP_H
#define _BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/motorola/mc68230.h>
#include <rtems/motorola/mc68681.h>

#define DUART_ADDR	0xb00003   /* base address of the MC68681 DUART */

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
