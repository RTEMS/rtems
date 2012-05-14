/*  set_vector
 *
 *  This routine installs an interrupt vector on the sun4v niagara
 *
 *  INPUT PARAMETERS:
 *    handler - interrupt handler entry point
 *    vector  - vector number
 *    type    - 0 indicates raw hardware connect
 *              1 indicates RTEMS interrupt connect
 *
 *  OUTPUT PARAMETERS:  NONE
 *
 *  RETURNS:
 *    address of previous interrupt handler
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

rtems_isr_entry set_vector(                   /* returns old vector */
  rtems_isr_entry     handler,                /* isr routine        */
  rtems_vector_number vector,                 /* vector number      */
  int                 type                    /* RTEMS or RAW intr  */
)
{
  rtems_isr_entry previous_isr;
  uint32_t      real_trap;
  uint32_t      source;
  int bit_mask;

  if ( type )
    rtems_interrupt_catch( handler, vector, &previous_isr );
  else 
    _CPU_ISR_install_raw_handler( vector, handler, (void *)&previous_isr );

  real_trap = SPARC_REAL_TRAP_NUMBER( vector );

  /* check if this is an interrupt, if so, clear and unmask interrupts for
   * this level
   */
  /* Interrupts have real_trap numbers between 0x41 and 0x4F (levels 1 - 15) */
  if (real_trap >= 0x41 && real_trap <= 0x4F) {
    source = real_trap - 0x40;
    bit_mask = 1<<source;

    sparc64_clear_interrupt_bits(bit_mask);
  }


  return previous_isr;
}
