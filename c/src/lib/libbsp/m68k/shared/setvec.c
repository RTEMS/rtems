/*  
 *
 *  This routine installs an interrupt vector using the basic
 *  RTEMS mechanisms.  This implementation should be suitable for 
 *  most m68k based boards.  However, if the board has an unusual
 *  interrupt controller or most somehow manipulate board specific
 *  hardware to enable/disable, mask, prioritize, etc an interrupt
 *  source, then this routine should be customized to support that.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
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

m68k_isr_entry set_vector(                      /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
)
{
  m68k_isr_entry  previous_isr;

  if ( type )
    rtems_interrupt_catch( handler, vector, (rtems_isr_entry *) &previous_isr );
  else {
    _CPU_ISR_install_raw_handler( vector, handler, (void *)&previous_isr );
  }
  return previous_isr;
}

