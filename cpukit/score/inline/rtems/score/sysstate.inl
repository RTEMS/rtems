/*  sysstates.inl
 *
 *  This file contains the inline implementation of routines regarding the 
 *  system state.
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

#ifndef __SYSTEM_STATE_inl
#define __SYSTEM_STATE_inl

/*PAGE
 *
 *  _System_state_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the system state handler.
 */
 
RTEMS_INLINE_ROUTINE void _System_state_Handler_initialization (
  boolean  is_multiprocessing
)
{
  _System_state_Current = SYSTEM_STATE_BEFORE_INITIALIZATION;
  _System_state_Is_multiprocessing = is_multiprocessing;
}
 
/*PAGE
 *
 *  _System_state_Set
 *
 *  DESCRIPTION:
 *
 *  This routine sets the current system state to that specified by
 *  the called.
 */

RTEMS_INLINE_ROUTINE void _System_state_Set (
  System_state_Codes state
)
{
  _System_state_Current = state;
}

/*PAGE
 *
 *  _System_state_Get
 *
 *  DESCRIPTION:
 *
 *  This function returns the current system state.
 */

RTEMS_INLINE_ROUTINE System_state_Codes _System_state_Get ( void )
{
  return _System_state_Current;
}

/*PAGE
 *
 *  _System_state_Is_before_initialization
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the state is equal to the
 *  "before initialization" state, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _System_state_Is_before_initialization (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_INITIALIZATION);
}

/*PAGE
 *
 *  _System_state_Is_before_multitasking
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the state is equal to the
 *  "before multitasking" state, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _System_state_Is_before_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_MULTITASKING);
}

/*PAGE
 *
 *  _System_state_Is_begin_multitasking
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the state is equal to the
 *  "begin multitasking" state, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _System_state_Is_begin_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEGIN_MULTITASKING);
}

/*PAGE
 *
 *  _System_state_Is_up
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the state is equal to the
 *  "up" state, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _System_state_Is_up (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_UP);
}

/*PAGE
 *
 *  _System_state_Is_failed
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the state is equal to the
 *  "failed" state, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _System_state_Is_failed (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_FAILED);
}

#endif
/* end of include file */
