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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
  i386_IDT_slot  idt;

  if ( type )
    rtems_interrupt_catch( handler, vector, (rtems_isr_entry *) &previous_isr );
  else {
    /* get the address of the old handler */

    idt = Interrupt_descriptor_table[ vector ];

    previous_isr = (i386_isr_entry)
                      ((idt.offset_16_31 << 16) | idt.offset_0_15);

    /* build the IDT entry */
    idt.offset_0_15      = ((rtems_unsigned32) handler) & 0xffff;
    idt.segment_selector = get_cs();
    idt.reserved         = 0x00;
    idt.p_dpl            = 0x8e;         /* present, ISR */
    idt.offset_16_31     = ((rtems_unsigned32) handler) >> 16;

    /* install the IDT entry */
    Interrupt_descriptor_table[ vector ] = idt;
  }
  return previous_isr;
}

