/*
 *  This routine installs an interrupt vector on the target Board/CPU.
 *  This routine is allowed to be as board dependent as necessary.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

/*
 *  This routine installs vector number vector.
 */
rtems_isr_entry set_vector(                     /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
)
{
  rtems_isr_entry previous_isr;
  rtems_status_code status;

  /*
   * vectors greater than PPC603e_IRQ_LAST are handled by the General purpose
   * interupt handler.
   */
  if ( vector > PPC_IRQ_LAST )  {
    set_EE_vector( handler, vector );
  }
  else  {
    status = rtems_interrupt_catch(
       handler, vector, (rtems_isr_entry *) &previous_isr );
  }
  return previous_isr;
}
