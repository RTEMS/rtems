/*  set_vector
 *
 *  NOTE: This function is considered OBSOLETE and may vanish soon.
 *	Calls to set_vector should be replaced by calls to
 *	rtems_interrupt_catch or _CPU_ISR_install_raw_handler.
 *
 *  This routine installs an interrupt vector on the target Board/CPU.
 *  This routine is allowed to be as board dependent as necessary.
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
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>

sh_isr_entry set_vector(      		/* returns old vector */
  rtems_isr_entry     handler,          /* isr routine        */
  rtems_vector_number vector,           /* vector number      */
  int                 type              /* RTEMS or RAW intr  */
)
{
  sh_isr_entry previous_isr;

  if ( type )
    rtems_interrupt_catch( handler, vector, (rtems_isr_entry *) &previous_isr );
  else {
    _CPU_ISR_install_raw_handler( vector, handler, (proc_ptr*) &previous_isr );
  }

  return previous_isr;
}
