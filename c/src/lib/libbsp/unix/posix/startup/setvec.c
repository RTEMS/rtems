/*  set_vector
 *
 *  This routine installs an interrupt vector on UNIX.
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

rtems_isr_entry
set_vector(                                     /* returns old vector */
    rtems_isr_entry     handler,                /* isr routine        */
    rtems_vector_number vector,                 /* vector number      */
    int                 type                    /* RTEMS or RAW intr  */
)
{
    rtems_isr_entry  rtems_isr_ptr = 0;
    proc_ptr         raw_isr_ptr;

    if ( type ) {
      rtems_interrupt_catch( handler, vector, &rtems_isr_ptr );
      return rtems_isr_ptr;
    } else {
      _CPU_ISR_install_vector( vector, (proc_ptr) handler, &raw_isr_ptr );
      return raw_isr_ptr;
    }
   
}


