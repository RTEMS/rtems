/*  isr.inl
 *
 *  This include file contains the static implementation of all
 *  inlined routines in the Interrupt Handler.
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

#ifndef __ISR_inl
#define __ISR_inl

/*PAGE
 *
 *  _ISR_Is_in_progress
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the processor is currently servicing
 *  and interrupt and FALSE otherwise.   A return value of TRUE indicates
 *  that the caller is an interrupt service routine, NOT a thread.  The
 *  directives available to an interrupt service routine are restricted.
 */

RTEMS_INLINE_ROUTINE boolean _ISR_Is_in_progress( void )
{
  return (_ISR_Nest_level != 0);
}

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
