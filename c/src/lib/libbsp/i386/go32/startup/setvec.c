/*  set_vector
 *
 *  This routine installs an interrupt vector under go32.
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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

#include <dpmi.h>
#include <go32.h>

i386_isr_entry set_vector(                      /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
)
{
  i386_isr_entry   previous_isr;

  if ( type )  {
      rtems_interrupt_catch( handler, vector, 
			     (rtems_isr_entry *) &previous_isr );
  } else {
      /* Interrupt goes straight to the supplied ISR.  This code is	*/
      /* slightly different than that in _CPU_ISR_install_vector	*/
      /* (which is eventually called by the above) in that this code	*/
      /* returns the raw entry point as the old handler, while the	*/
      /* other version returns the old entry point pointed at by the	*/
      /* rtems ISR table.						*/
      _go32_dpmi_seginfo	handler_info;

      /* get the address of the old handler */
      _go32_dpmi_get_protected_mode_interrupt_vector( vector, &handler_info);

      /* Notice how we're failing to save the pm_segment portion of the	*/
      /* structure here?  That means we might crash the system if we	*/
      /* try to restore the ISR.  Can't fix this until i386_isr	is	*/
      /* redefined.  XXX [BHC].						*/
      previous_isr = (i386_isr_entry) handler_info.pm_offset;
     
      /* install the IDT entry */
      handler_info.pm_offset   = (u_long)handler;
      handler_info.pm_selector = _go32_my_cs();
      _go32_dpmi_set_protected_mode_interrupt_vector( vector, &handler_info);
  }
  return previous_isr;
}

