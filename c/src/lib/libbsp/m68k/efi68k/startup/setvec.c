/*  set_vector
 *
 *  This routine installs an interrupt vector on the efi68k.
 *
 *  INPUT:
 *    handler - interrupt handler entry point
 *    vector  - vector number
 *    type    - 0 indicates raw hardware connect
 *              1 indicates RTEMS interrupt connect
 *
 *  RETURNS:
 *    address of previous interrupt handler
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

#include <bsp.h>

m68k_isr_entry set_vector(                      /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
)
{
  int *p;
  m68k_isr_entry  previous_isr;

  if ( type )
    rtems_interrupt_catch( handler, vector, (rtems_isr_entry *) &previous_isr );
  else {
    p = (int *)(vector*6-12+2+(int)_VBR);
    previous_isr  =  (m68k_isr_entry) *p;	/* return old ISR  */
    *p = (int) handler;    /* install new ISR */
  }
  return previous_isr;
}

