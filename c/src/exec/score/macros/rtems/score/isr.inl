/*  isr.inl
 *
 *  This include file contains the macro implementation of all
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
 *  _ISR_Handler_initialization
 *
 */

#define _ISR_Handler_initialization() \
  { \
    _ISR_Signals_to_thread_executing = FALSE; \
    _ISR_Nest_level = 0; \
  }

/*PAGE
 *
 *  _ISR_Is_in_progress
 *
 */

#define _ISR_Is_in_progress() \
        (_ISR_Nest_level != 0)

/*PAGE
 *
 *  _ISR_Is_vector_number_valid
 *
 */

#define _ISR_Is_vector_number_valid( _vector ) \
  ( (_vector) < CPU_INTERRUPT_NUMBER_OF_VECTORS )

/*PAGE
 *
 *  _ISR_Is_valid_user_handler
 *
 */

#define _ISR_Is_valid_user_handler( _handler ) \
  ((_handler) != NULL)

#endif
/* end of include file */
