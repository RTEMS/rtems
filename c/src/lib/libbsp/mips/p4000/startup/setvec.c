/*  set_vector
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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  setvec.c,v 1.2 1995/05/31 16:56:35 joel Exp
 */

#ifndef lint
static char _sccsid[] = "@(#)setvec.c 04/25/96     1.2\n";
#endif

#include <rtems.h>
#include <bsp.h>

mips_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
)
{
  mips_isr_entry previous_isr;

  if ( type )
    rtems_interrupt_catch( handler, vector, (rtems_isr_entry *) &previous_isr );
  else {
    /* XXX: install non-RTEMS ISR as "raw" interupt */
    rtems_interrupt_catch( handler, vector, (rtems_isr_entry *) &previous_isr );
  }
  return previous_isr;
}

