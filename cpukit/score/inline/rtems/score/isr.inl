/*  isr.inl
 *
 *  This include file contains the static implementation of all
 *  inlined routines in the Interrupt Handler.
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

#ifndef __ISR_inl
#define __ISR_inl

/*PAGE
 *
 *  _ISR_Is_vector_number_valid
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the vector is a valid vector number
 *  for this processor and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _ISR_Is_vector_number_valid (
  unsigned32 vector
)
{
  return ( vector <= CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER );
}

/*PAGE
 *
 *  _ISR_Is_valid_user_handler
 *
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if handler is the entry point of a valid
 *  use interrupt service routine and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _ISR_Is_valid_user_handler (
  void *handler
)
{
  return ( handler != NULL);
}

#endif
/* end of include file */
