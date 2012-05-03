/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

unsigned char *duart_base;
extern struct duart_regs duart_info;

#define DELAY 5000

void led_putnum(void);

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  rtems_isr_entry       *monitors_vector_table;
  int                   index;

  /* RAM size set in linker script */
  duart_base    = (unsigned char *)DUART_ADDR;

  /*
   *  Set the VBR here to the monitor's default.
   */
  monitors_vector_table = (rtems_isr_entry *)0;
   /* This is where you set vector base register = 0 */
  m68k_set_vbr( monitors_vector_table );

  /* The vector interrupt table for the 680x0 is in appendix B-2
   *  of the M68000 Family Programmer's reference table
   */
  for ( index=2 ; index<=255 ; index++ )
    M68Kvec[ index ] = monitors_vector_table[ 32 ];

  M68Kvec[  2 ] = monitors_vector_table[  2 ];   /* bus error vector */
  M68Kvec[  4 ] = monitors_vector_table[  4 ];   /* breakpoints vector */
  M68Kvec[  9 ] = monitors_vector_table[  9 ];   /* trace vector */

  /*
   *  Set the VBR here if you do not want to use the monitor's vector table.
   */

  m68k_set_vbr( &M68Kvec );

  rtems_cache_enable_instruction();
  rtems_cache_enable_data();

/*  led_putnum('e'); * for debugging purposes only */
}
