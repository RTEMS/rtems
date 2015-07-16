/**
 *  @file
 *
 *  @ingroup idp_bsp
 *
 *  @brief This include file contains all Motorola 680x0 IDP board
 *  IO definitions.
 */

#ifndef LIBBSP_M68K_IDP_BSP_H
#define LIBBSP_M68K_IDP_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/motorola/mc68230.h>
#include <rtems/motorola/mc68681.h>

/**
 *  @defgroup idp_bsp IO definitions
 *
 *  @ingroup m68k_idp
 *
 *  @brief Motorola 680x0 IDP board IO definitions
 */

#define DUART_ADDR	0xb00003   /* base address of the MC68681 DUART */

/* Constants */

#define RAM_START 0
#define RAM_END   0x200000

extern rtems_isr_entry M68Kvec[];   /* vector table address */

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

void rtems_bsp_delay(int num);

/*
 * Prototypes for methods inside the BSP that cross file boundaries.
 */
void init_pit( void );
void transmit_char( char ch );
void transmit_char_portb( char ch );

#endif
