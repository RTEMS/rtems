/*  set_vector
 *
 *  This routine installs an interrupt vector on the Force CPU-386.
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

#include <rtems.h>
#include <bsp.h>

i386_isr_entry set_vector(                      /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
)
{
  i386_isr_entry previous_isr;
  interrupt_gate_descriptor  idt;

  if ( type )
    rtems_interrupt_catch( handler, vector, (rtems_isr_entry *) &previous_isr );
  else {
    /* get the address of the old handler */

    idt = Interrupt_descriptor_table[ vector ];

    previous_isr = (i386_isr_entry)
                      ((idt.offset_16_31 << 16) | idt.offset_0_15);

    /* build the IDT entry */
    create_interrupt_gate_descriptor( &idt, handler );

    /* install the IDT entry */
    Interrupt_descriptor_table[ vector ] = idt;
  }
  return previous_isr;
}

